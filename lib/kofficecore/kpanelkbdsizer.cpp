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
#include <qcursor.h>
#include <qobjectlist.h>
#include <qwidgetlist.h>

// KDE includes
#include <klocale.h>
#include <kglobal.h>
#include <kapplication.h>
#include <kmainwindow.h>
#include <kaction.h>
#include <kdebug.h>

// KPanelKbdSizer includes
#include "kpanelkbdsizer.h"
// TODO: See eventFilter method.
// #include "kpanelkbdsizer.moc"

class KPanelKbdSizerIcon : public QCursor
{
    public:
        KPanelKbdSizerIcon() :
            QCursor(Qt::SizeAllCursor),
            isActive(false)
        {
            currentPos = QPoint(-1, -1);
        }

        ~KPanelKbdSizerIcon()
        {
            hide();
        }

        void show(const QPoint p) {
            if (!isActive) {
                originalPos = QCursor::pos();
                kapp->setOverrideCursor(*this);
                isActive = true;
            }
            if (p != pos())
                setPos(p);
            currentPos = p;
        }

        void hide() {
            if (isActive) {
                kapp->restoreOverrideCursor();
                QCursor::setPos(originalPos);
            }
            isActive = false;
        }

        void setShape(int shayp)
        {
            if (shayp != shape()) {
                // Must restore and override to get the icon to refresh.
                if (isActive) kapp->restoreOverrideCursor();
                QCursor::setShape(shayp);
                if (isActive) kapp->setOverrideCursor(*this);
            }
        }

        // Return the difference between a position and where icon is supposed to be.
        QSize delta(const QPoint p)
        {
            QPoint d = p - currentPos;
            return QSize(d.x(), d.y());
        }

        // Return the difference between where the icon is currently positioned and where
        // it is supposed to be.
        QSize delta() { return delta(pos()); }

        // True if the sizing icon is visible.
        bool isActive;

    private:
        // Icon's current position.
        QPoint currentPos;
        // Mouse cursor's original position when icon is shown.
        QPoint originalPos;
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
            stepSize(10) {};

        ~KPanelKbdSizerPrivate()
        {
            delete icon;
        }

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

        // Sizing increment.
        int stepSize;
};

KPanelKbdSizer::KPanelKbdSizer(KMainWindow* parent, const char* name) :
    QObject(parent, name)
{
    // kdDebug() << "KPanelKbdSizer::KPanelKbdSizer: running." << endl;
    d = new KPanelKbdSizerPrivate;
    d->fwdAction = new KAction(i18n("Resize Panel Forward"), KShortcut("F8"),
        0, 0, parent->actionCollection(), "resize_panel_forward");
    d->revAction = new KAction(i18n("Resize Panel Reverse"), KShortcut("Shift+F8"),
        0, 0, parent->actionCollection(), "resize_panel_reverse");
    // "Disable" the shortcuts so we can see them in eventFilter.
    d->fwdAction->setEnabled(false);
    d->revAction->setEnabled(false);
    d->icon = new KPanelKbdSizerIcon();
    kapp->installEventFilter(this);
}

KPanelKbdSizer::~KPanelKbdSizer()
{
    kapp->removeEventFilter(this);
    if (d->panel) exitSizing();
    delete d;
}

int KPanelKbdSizer::stepSize() const { return d->stepSize; }

void KPanelKbdSizer::setStepSize(int s) { d->stepSize = s; }

bool KPanelKbdSizer::eventFilter( QObject *o, QEvent *e )
{
    if ( e->type() == QEvent::KeyPress ) {
        // TODO: This permits only a single-key shortcut.  For example, Alt+S,R would not work.
        // If user configures a multi-key shortcut, it is undefined what will happen here.
        // It would be better to handle these as KShortcut activate() signals, but the problem
        // is that once a QDockWindow is undocked and has focus, the KShortcut activate() signals
        // don't fire anymore.
        KShortcut fwdSc = d->fwdAction->shortcut();
        KShortcut revSc = d->revAction->shortcut();
        QKeyEvent* kev = dynamic_cast<QKeyEvent *>(e);
        KKey k = KKey(kev);
        KShortcut sc = KShortcut(k);
        // kdDebug() << "KPanelKbdSizer::eventFilter: Key press " << sc << endl;
        if (sc == fwdSc) {
            nextHandle();
            return true;
        }
        if (sc == revSc) {
            prevHandle();
            return true;
        }
        if (d->panel) {
            if (k == KKey(Key_Escape))
                exitSizing();
            else
                resizePanelFromKey(kev->key(), kev->state());
            // Eat the key.
            return true;
        } else
            return false;
    }
    else if (d->icon->isActive && e->type() == QEvent::MouseButtonPress) {
        exitSizing();
        return true;
    }
/*    else if (e->type() == QEvent::MouseMove && d->icon->isActive) {
        // Lock mouse cursor down.
        showIcon();
        dynamic_cast<QMouseEvent *>(e)->accept();
        return true;
    }*/
    else if (e->type() == QEvent::MouseMove && d->icon->isActive && d->panel) {
        // Resize according to mouse movement.
        QMouseEvent* me = dynamic_cast<QMouseEvent *>(e);
        QSize s = d->icon->delta();
        int dx = s.width();
        int dy = s.height();
        resizePanel(dx, dy, me->state());
        me->accept();
        showIcon();
        return true;
    }
    else if (e->type() == QEvent::Resize && d->panel && o == d->panel) {
        // TODO: This doesn't always work.
        showIcon();
    }
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
    // See if current panel has another handle.  If not, find next panel.
    if (panel) {
        bool advance = true;
        d->handleNdx++;
        if (panel->inherits("QSplitter"))
            advance = (d->handleNdx >= dynamic_cast<QSplitter *>(panel)->sizes().count());
        else
            // Undocked windows have only one "handle" (center).
            advance = (d->handleNdx > 2 || !dynamic_cast<QDockWindow *>(panel)->area());
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
    // See if current panel has another handle.  If not, find previous panel.
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
            if (panel) {
                if (panel->inherits("QSplitter"))
                    d->handleNdx = dynamic_cast<QSplitter *>(panel)->sizes().count() - 1;
                else {
                    if (dynamic_cast<QDockWindow *>(panel)->area())
                        d->handleNdx = 2;
                    else
                        d->handleNdx = 1;
                }
            }
        }
    } else {
        // Find last panel.
        QWidgetList* allWidgets = getAllPanels();
        panel = allWidgets->last();
        delete allWidgets;
        if (panel) {
            if (panel->inherits("QSplitter"))
                d->handleNdx = dynamic_cast<QSplitter *>(panel)->sizes().count() - 1;
            else {
                if (dynamic_cast<QDockWindow *>(panel)->area())
                    d->handleNdx = 2;
                else
                    d->handleNdx = 1;
            }
        }
    }
    d->panel = panel;
    if (panel)
        showIcon();
    else
        exitSizing();
}

void KPanelKbdSizer::exitSizing()
{
    // kdDebug() << "KPanelKbdSizer::exiting sizing mode." << endl;
    hideIcon();
    d->handleNdx = 0;
    d->panel = 0;
}

void KPanelKbdSizer::showIcon()
{
    if (!d->panel) return;
    QPoint p;
    // kdDebug() << "KPanelKbdSizer::showIcon: topLevelWidget = " << d->panel->topLevelWidget()->name() << endl;
    if (d->panel->inherits("QSplitter")) {
        QSplitter* splitter = dynamic_cast<QSplitter *>(d->panel);
        int handleNdx = d->handleNdx - 1;
        QValueList<int> sizes = splitter->sizes();
        // kdDebug() << "KPanelKbdSizer::showIcon: sizes = " << sizes << endl;
        if (splitter->orientation() == Qt::Horizontal) {
            d->icon->setShape(Qt::SizeHorCursor);
            p.setX(sizes[handleNdx] + (splitter->handleWidth() / 2));
            p.setY(splitter->height() / 2);
        } else {
            d->icon->setShape(Qt::SizeVerCursor);
            p.setX(splitter->width() / 2);
            p.setY(sizes[handleNdx] + (splitter->handleWidth() / 2));
        }
        // kdDebug() << "KPanelKbdSizer::showIcon: p = " << p << endl;
        p = splitter->mapToGlobal(p);
        // kdDebug() << "KPanelKbdSizer::showIcon: mapToGlobal = " << p << endl;
    } else {
        QDockWindow* dockWindow = dynamic_cast<QDockWindow *>(d->panel);
        p = dockWindow->pos();
        if (dockWindow->area()) {
            // kdDebug() << "KPanelKbdSizer::showIcon: pos = " << p << " of window = " << dockWindow->parentWidget()->name() << endl;
            p = dockWindow->parentWidget()->mapTo(dockWindow->topLevelWidget(), p);
            // kdDebug() << "KPanelKbdSizer::showIcon: mapTo = " << p << " of window = " << dockWindow->topLevelWidget()->name() << endl;
            // TODO: How to get the handle width?
            if (d->handleNdx == 1) {
                d->icon->setShape(Qt::SizeHorCursor);
                if (dockWindow->area()->orientation() == Qt::Vertical) {
                    if (dockWindow->area()->handlePosition() == QDockArea::Normal)
                        // Handle is to the right of the dock window.
                        p.setX(p.x() + dockWindow->width());
                        // else Handle is to the left of the dock window.
                } else
                    // Handle is to the right of the dock window.
                    p.setX(p.x() + dockWindow->width());
                p.setY(p.y() + (dockWindow->height() / 2));
            } else {
                d->icon->setShape(Qt::SizeVerCursor);
                p.setX(p.x() + (dockWindow->width() / 2));
                if (dockWindow->area()->orientation() == Qt::Vertical)
                    // Handle is below the dock window.
                    p.setY(p.y() + dockWindow->height());
                else {
                    if (dockWindow->area()->handlePosition() == QDockArea::Normal)
                        // Handle is below the dock window.
                        p.setY(p.y() + dockWindow->height());
                        // else Handle is above the dock window.
                }
            }
            p = dockWindow->topLevelWidget()->mapToGlobal(p);
        } else {
            d->icon->setShape(Qt::SizeAllCursor);
            p = QPoint(dockWindow->width() / 2, dockWindow->height() / 2);
            p = dockWindow->mapToGlobal(p);       // Undocked.  Position in center of window.
        }
    }
    // kdDebug() << "KPanelKbdSizer::showIcon: show(p) = " << p << endl;
    d->icon->show(p);
}

void KPanelKbdSizer::hideIcon()
{
    d->icon->hide();
}

void KPanelKbdSizer::resizePanel(int dx, int dy, int state)
{
    int adj = dx + dy;
    if (adj == 0) return;
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
}

void KPanelKbdSizer::resizePanelFromKey(int key, int state)
{
    // kdDebug() << "KPanelKdbSizer::resizePanelFromKey: key = " << key << " state = " << state << endl;
    if (!d->panel) return;
    int dx = 0;
    int dy = 0;
    int stepSize = d->stepSize;
    switch (key) {
        case Qt::Key_Left:      dx = -stepSize;     break;
        case Qt::Key_Right:     dx = stepSize;      break;
        case Qt::Key_Up:        dy = -stepSize;     break;
        case Qt::Key_Down:      dy = stepSize;      break;
        case Qt::Key_Prior:     dx = -5 * stepSize; break;
        case Qt::Key_Next:      dx = 5 * stepSize;  break;
    }
    int adj = dx + dy;
    // kdDebug() << "KPanelKbdSizer::resizePanelFromKey: adj = " << adj << endl;
    if (adj != 0)
        resizePanel(dx, dy, state);
    else {
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
