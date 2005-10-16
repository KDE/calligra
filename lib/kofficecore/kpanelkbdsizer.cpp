/* This file is part of the KDE project
   Copyright (C) 2005, Gary Cramblitt <garycramblitt@comcast.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Qt includes
#include <qsplitter.h>
#include <qdockwindow.h>
#include <qdockarea.h>
#include <qevent.h>
#include <qlabel.h>
#include <qobjectlist.h>
#include <qwidgetlist.h>
#include <qbitmap.h>

// KDE includes
#include <klocale.h>
#include <kglobal.h>
#include <kapplication.h>
#include <kmainwindow.h>
#include <kiconloader.h>
#include <kaction.h>
#include <kdebug.h>

// KPanelKbdSizer includes
#include "kpanelkbdsizer.h"
// TODO: See eventFilter method.
// #include "kpanelkbdsizer.moc"

class KPanelKbdSizerIcon : public QLabel
{
    public:
        KPanelKbdSizerIcon(QWidget* parent, const char * name = 0, WFlags f = 0) :
            QLabel(parent, name, f)
        {
            // Load icon.
            QPixmap icon = KGlobal::iconLoader()->loadIcon("move", KIcon::Small, 0, true);
            // kdDebug() << "KPanelKbdSizerIcon::KPanelKbdSizerIcon: icon size = " << icon.size() << endl;
            resize(icon.size());
            setPixmap(icon);
            // Make transparent.
            // TODO: This only works the first time the icon is shown.  When the icon is moved,
            // whatever was in its background when first shown, moves with it.  Ugh.  Anybody
            // know a solution, short of Xorg and composite thingy?
            setBackgroundMode(Qt::NoBackground);
        }
};

class KPanelKbdSizerPrivate
{
    public:
        KPanelKbdSizerPrivate() :
            fwdAction(0),
            revAction(0),
            panel(0),
            handleNdx(0),
            icon(0),
            focusedWidget(0) {};

        // Action that starts panel sizing (defaults to F8), forward and reverse;
        KAction* fwdAction;
        KAction* revAction;

        // The splitter or dockwindow currently being sized.  If 0, sizing is not in progress.
        QWidget* panel;

        // Index of current handle of the panel.  When panel is a QDockWindow:
        //      1 = size horizontally
        //      2 = size vertically
        uint handleNdx;

        // Sizing icon.
        KPanelKbdSizerIcon* icon;

        // Widget that had focus when sizing began.
        QWidget* focusedWidget;
};

KPanelKbdSizer::KPanelKbdSizer(KMainWindow* parent, const char* name) :
    QObject(parent, name)
{
    d = new KPanelKbdSizerPrivate;
    d->fwdAction = new KAction(i18n("Resize Panel Forward"), KShortcut("F8"),
        0, 0, parent->actionCollection(), "resize_panel_forward");
    d->fwdAction->setEnabled(false);
    d->revAction = new KAction(i18n("Resize Panel Reverse"), KShortcut("Shift+F8"),
        0, 0, parent->actionCollection(), "resize_panel_reverse");
    d->revAction->setEnabled(false);
    kapp->installEventFilter(this);
}

KPanelKbdSizer::~KPanelKbdSizer()
{
    kapp->removeEventFilter(this);
    d->focusedWidget = 0;
    if (d->panel) exitSizing();
    if (d->icon)
        if (!d->icon->parent()) delete d->icon;
    delete d;
}

bool KPanelKbdSizer::eventFilter( QObject *o, QEvent *e )
{
    if ( e->type() == QEvent::KeyPress ) {
        // TODO: This permits only a single-sequence shortcut.  For example, Alt+S,R would not work.
        // If user configures a multi-sequence shortcut, it is undefined what will happen here.
        // It would be better to handle these as KShortcut activate() signals, but the problem
        // is that once a QDockWindow is undocked and has focus, the KShortcut activate() signals
        // don't fire anymore.
        KKey fwdKey = d->fwdAction->shortcut().seq(0).key(0);
        KKey revKey = d->revAction->shortcut().seq(0).key(0);
        QKeyEvent* kev = dynamic_cast<QKeyEvent *>(e);
        KKey k = KKey(kev);
        // kdDebug() << "KPanelKbdSizer::eventFilter: At key press %d" << k.keyCodeQt() << endl;
        if (k == fwdKey) {
            nextHandle();
            return true;
        }
        if (k == revKey) {
            prevHandle();
            return true;
        }
        if (d->panel) {
            if (k == KKey(Key_Escape))
                exitSizing();
            else
                resizePanel(kev->key(), kev->state());
            // Eat the key.
            return true;
        } else
            return false;
    } else if (e->type() == QEvent::FocusOut && d->icon && o == d->icon) {
        d->focusedWidget = 0;       // Do not restore focus to original when sizing began.
        exitSizing();
        // Don't eat the event.
    }
    else if (e->type() == QEvent::Resize && d->icon && o == d->icon->parent()) {
        // TODO: This doesn't always work.  For example, hit the maximize/restore button
        // and the icon won't relocate.
         showIcon();
        // TODO: This causes focus to become random.
        // d->focusedWidget = 0;
        // exitSizing();
    }
    else if (e->type() == QEvent::LayoutHint && d->icon && o == d->icon->parent()) {
         showIcon();
    }
    // TODO: If user resizes panel with mouse, icon does not disappear.
    // standard event processing
    return false;
}

QWidgetList* KPanelKbdSizer::getAllPanels()
{
    QWidgetList* allWidgets = kapp->allWidgets();
    QWidgetList* allPanels = new QWidgetList;
    QWidget* widget = allWidgets->first();
    while (widget) {
        if (widget->isVisible()) {
            if (widget->inherits("QSplitter")) {
                // Only size QSplitters with at least two handles (there is always one hidden).
                if (dynamic_cast<QSplitter *>(widget)->sizes().count() >= 2)
                    allPanels->append(widget);
            } else if (widget->inherits("QDockWindow")) {
                if (dynamic_cast<QDockWindow *>(widget)->isResizeEnabled()) {
                    // kdDebug() << "KPanelKbdSizer::getAllPanels: QDockWindow = " << widget->name() << endl;
                    allPanels->append(widget);
                }
            }
        }
        widget = allWidgets->next();
    }
    delete allWidgets;
    return allPanels;
}

void KPanelKbdSizer::nextHandle()
{
    QWidget* panel = d->panel;
    // If entering sizing mode, record widget which has the focus.
    if (!panel) d->focusedWidget = kapp->focusWidget();
    // See if current panel has another handle.  If not, find next panel.
    if (panel) {
        bool advance = true;
        d->handleNdx++;
        if (panel->inherits("QSplitter"))
            advance = (d->handleNdx >= dynamic_cast<QSplitter *>(panel)->sizes().count());
        else
            advance = (d->handleNdx > 2);
        if (advance) {
            QWidgetList* allWidgets = getAllPanels();
            allWidgets->findRef(panel);
            panel = 0;
            if (allWidgets->current()) panel = allWidgets->next();
            delete allWidgets;
            d->handleNdx = 1;
        }
    } else {
        // Find first panel.
        QWidgetList* allWidgets = getAllPanels();
        panel = allWidgets->first();
        delete allWidgets;
        d->handleNdx = 1;
    }
    d->panel = panel;
    if (panel)
        showIcon();
    else
        exitSizing();
}

void KPanelKbdSizer::prevHandle()
{
    QWidget* panel = d->panel;
    // If entering sizing mode, record widget which has the focus.
    if (!panel) d->focusedWidget = kapp->focusWidget();
    // See if current panel has another handle.  If not, find next panel.
    if (panel) {
        bool rewind = true;
        d->handleNdx--;
        rewind = (d->handleNdx < 1);
        if (rewind) {
            QWidgetList* allWidgets = getAllPanels();
            allWidgets->findRef(panel);
            panel = 0;
            if (allWidgets->current()) panel = allWidgets->prev();
            delete allWidgets;
            if (panel && panel->inherits("QSplitter"))
                d->handleNdx = dynamic_cast<QSplitter *>(panel)->sizes().count() - 1;
            else
                d->handleNdx = 2;
        }
    } else {
        // Find last panel.
        QWidgetList* allWidgets = getAllPanels();
        panel = allWidgets->last();
        delete allWidgets;
        if (panel && panel->inherits("QSplitter"))
            d->handleNdx = dynamic_cast<QSplitter *>(panel)->sizes().count() - 1;
        else
            d->handleNdx = 0;
    }
    d->panel = panel;
    if (panel)
        showIcon();
    else
        exitSizing();
}

void KPanelKbdSizer::exitSizing()
{
    // kdDebug() << "KPanelKbdSizer::exitSizing: running." << endl;
    // Return focus to the widget that had it when resizing began.
    if (d->focusedWidget) {
        d->focusedWidget->setFocus();
        d->focusedWidget = 0;
    }
    hideIcon();
    d->handleNdx = 0;
    d->panel = 0;
}

void KPanelKbdSizer::showIcon()
{
    if (!d->panel) return;
    QPoint p;
    if (!d->icon) {
        d->icon = new KPanelKbdSizerIcon(d->panel->topLevelWidget());
        d->icon->installEventFilter(this);
    }
    if (d->icon->parent() != d->panel->topLevelWidget())
        d->icon->reparent(d->panel->topLevelWidget(), p);
    // kdDebug() << "KPanelKbdSizer::showIcon: topLevelWidget = " << d->panel->topLevelWidget()->name() << endl;
    if (d->panel->inherits("QSplitter")) {
        QSplitter* splitter = dynamic_cast<QSplitter *>(d->panel);
        int handleNdx = d->handleNdx - 1;
        QValueList<int> sizes = splitter->sizes();
        // kdDebug() << "KPanelKbdSizer::showIcon: sizes = " << sizes << endl;
        if (splitter->orientation() == Qt::Horizontal) {
            p.setX(sizes[handleNdx] + (splitter->handleWidth() / 2) - (d->icon->width() / 2));
            p.setY((splitter->height() / 2) - (d->icon->height() / 2));
        } else {
            p.setX((splitter->width() / 2) - (d->icon->width() / 2));
            p.setY(sizes[handleNdx] + (splitter->handleWidth() / 2) - (d->icon->height() / 2));
        }
        // kdDebug() << "KPanelKbdSizer::showIcon: p = " << p << endl;
        p = splitter->mapTo(splitter->topLevelWidget(), p);
        // kdDebug() << "KPanelKbdSizer::showIcon: mapToParent = " << p << endl;
    } else {
        QDockWindow* dockWindow = dynamic_cast<QDockWindow *>(d->panel);
        p = dockWindow->pos();
        if (dockWindow->area()) {
            // kdDebug() << "KPanelKbdSizer::showIcon: pos = " << p << " of window = " << dockWindow->parentWidget()->name() << endl;
            p = dockWindow->parentWidget()->mapTo(dockWindow->topLevelWidget(), p);
            // kdDebug() << "KPanelKbdSizer::showIcon: mapTo = " << p << " of window = " << dockWindow->topLevelWidget()->name() << endl;
            // TODO: How to get the handle width?
            if (d->handleNdx == 1) {
                if (dockWindow->area()->orientation() == Qt::Vertical) {
                    if (dockWindow->area()->handlePosition() == QDockArea::Normal)
                        // Handle is to the right of the dock window.
                        p.setX(p.x() + dockWindow->width() - (d->icon->width() / 2));
                    else
                        // Handle is to the left of the dock window.
                        p.setX(p.x() - (d->icon->width() / 2));
                } else
                    // Handle is to the right of the dock window.
                    p.setX(p.x() + dockWindow->width() - (d->icon->width() / 2));
                p.setY(p.y() + (dockWindow->height() / 2) - (d->icon->height() / 2));
            } else {
                p.setX(p.x() + (dockWindow->width() / 2) - (d->icon->width() / 2));
                if (dockWindow->area()->orientation() == Qt::Vertical)
                    // Handle is below the dock window.
                    p.setY(p.y() + dockWindow->height() - (d->icon->height() / 2));
                else {
                    if (dockWindow->area()->handlePosition() == QDockArea::Normal)
                        // Handle is below the dock window.
                        p.setY(p.y() + dockWindow->height() - (d->icon->height() / 2));
                    else
                        // Handle is above the dock window.
                        p.setY(p.y() - (d->icon->height() / 2));
                }
            }
        } else
            p = QPoint();       // Undocked.  Position in upperleft corner.
    }
    // kdDebug() << "KPanelKbdSizer::showIcon: move(p) = " << p << endl;
    d->icon->move(p);
    d->icon->show();
    d->icon->setFocus();
}

void KPanelKbdSizer::hideIcon()
{
    if (!d->icon) return;
    d->icon->hide();
    d->icon->reparent(0, QPoint());
}

void KPanelKbdSizer::resizePanel(int key, int state)
{
    // kdDebug() << "KPanelKdbSizer::resizePanel: key = " << key << " state = " << state << endl;
    if (!d->panel) return;
    int dx = 0;
    int dy = 0;
    switch (key) {
        case Qt::Key_Left:      dx = -10;   break;
        case Qt::Key_Right:     dx = 10;    break;
        case Qt::Key_Up:        dy = -10;   break;
        case Qt::Key_Down:      dy = 10;    break;
        case Qt::Key_Prior:     dx = -50;   break;
        case Qt::Key_Next:      dx = 50;    break;
    }
    int adj = dx + dy;
    // kdDebug() << "KPanelKbdSizer::resizePanel: adj = " << adj << endl;
    if (adj != 0) {
        // kdDebug() << "KPanelKbdSizer::resizePanel: panel = " << d->panel->name() << endl;
        if (d->panel->inherits("QSplitter")) {
            QSplitter* splitter = dynamic_cast<QSplitter *>(d->panel);
            int handleNdx = d->handleNdx - 1;
            QValueList<int> sizes = splitter->sizes();
            // kdDebug() << "KPanelKbdSizer::resizePanel: before sizes = " << sizes << endl;
            sizes[handleNdx] = sizes[handleNdx] + adj;
            // kdDebug() << "KPanelKbdSizer::resizePanel: setSizes = " << sizes << endl;
            splitter->setSizes(sizes);
            QApplication::postEvent(splitter, new QEvent(QEvent::LayoutHint));
        } else {
            // TODO: How to get the handle width?
            QDockWindow* dockWindow = dynamic_cast<QDockWindow *>(d->panel);
            if (dockWindow->area()) {
                // kdDebug() << "KPanelKbdSizer::resizePanel: fixedExtent = " << dockWindow->fixedExtent() << endl;
                QSize fe = dockWindow->fixedExtent();
                if (d->handleNdx == 1) {
                    // When vertically oriented and dock area is on right side of screen, pressing
                    // left arrow increases size.
                    if (dockWindow->area()->orientation() == Qt::Vertical &&
                        dockWindow->area()->handlePosition() == QDockArea::Reverse) adj = -adj;
                    int w = fe.width();
                    if (w < 0) w = dockWindow->width();
                    w = w + adj;
                    if (w > 0 ) dockWindow->setFixedExtentWidth(w);
                } else {
                    // When horizontally oriented and dock area is at bottom of screen,
                    // pressing up arrow increases size.
                    if (dockWindow->area()->orientation() == Qt::Horizontal &&
                        dockWindow->area()->handlePosition() == QDockArea::Reverse) adj = -adj;
                    int h = fe.height();
                    if (h < 0) h = dockWindow->height();
                    h = h + adj;
                    if (h > 0) dockWindow->setFixedExtentHeight(h);
                }
                dockWindow->updateGeometry();
                QApplication::postEvent(dockWindow->area(), new QEvent(QEvent::LayoutHint));
                // kdDebug() << "KPanelKbdSizer::resizePanel: fixedExtent = " << dockWindow->fixedExtent() << endl;
            } else {
                if (state == Qt::ShiftButton) {
                    QSize s = dockWindow->size();
                    s.setWidth(s.width() + dx);
                    s.setHeight(s.height() + dy);
                    dockWindow->resize(s);
                } else {
                    QPoint p = dockWindow->pos();
                    p.setX(p.x() + dx);
                    p.setY(p.y() + dy);
                    dockWindow->move(p);
                }
            }
        }
    } else {
        // TODO: This is purely experimental right now.  It is interesting that with this here,
        // I can get focus to palette tabs, but can't do that using normal tab order.
        if (key == Qt::Key_F6) {
            QWidget* p = d->panel;
            uint ndx = d->handleNdx;
            exitSizing();
            if (p->inherits("QDockWindow")) {
                QDockWindow* dockWindow = dynamic_cast<QDockWindow *>(p);
                if (dockWindow->widget()) dockWindow->widget()->setFocus();
            } else {
                QObjectList *l = p->queryList( "QWidget" );
                QObjectListIt it( *l ); // iterate over the buttons
                QObject *obj;
                ndx--;
                while ( (obj = it.current()) != 0 && ndx > 0 ) { ndx--; ++it; }
                if (obj != 0)
                    dynamic_cast<QWidget*>(obj)->setFocus();
            }
        } else {
            if (key == Qt::Key_Enter && d->panel->inherits("QDockWindow")) {
                QDockWindow* dockWindow = dynamic_cast<QDockWindow *>(d->panel);
                if (dockWindow->area())
                    dockWindow->undock();
                else
                    dockWindow->dock();
            }
        }
    }
    showIcon();
}
