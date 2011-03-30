/* This file is part of the KDE project
   Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
   
   Based on qmenu.cpp from Qt 4.7

   Based on oxygenhelper.cpp

   Copyright 2009-2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
   Copyright 2008 Long Huynh Huu <long.upcase@googlemail.com>
   Copyright 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
   Copyright 2007 Casper Boemann <cbr@boemann.dk>
   Copyright 2007 Fredrik Höglund <fredrik@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KexiMenuWidget.h"
#include <KColorScheme>
#include <KColorUtils>
#include <KGlobalSettings>
#include <KIconLoader>

#include "qdebug.h"
#include "qcache.h"
#include "qstyle.h"
#include "qevent.h"
#include "qtimer.h"
#include "qlayout.h"
#include "qmenu.h"
#include "qpainter.h"
#include "qapplication.h"
#include "qdesktopwidget.h"
#ifndef QT_NO_ACCESSIBILITY
# include "qaccessible.h"
#endif
/*#ifndef QT_NO_EFFECTS
# include <private/qeffects_p.h>
#endif*/
#ifndef QT_NO_WHATSTHIS
# include <qwhatsthis.h>
#endif

#include "KexiMenuWidget_p.h"
//#include "qmenubar_p.h"
#include "qwidgetaction.h"
#include "qtoolbutton.h"
#include "qpushbutton.h"
#include <QScopedPointer>
//#include <private/qpushbutton_p.h>
//#include <private/qaction_p.h>
//#include <private/qsoftkeymanager_p.h>

// #ifdef Q_WS_X11
// #   include <private/qt_x11_p.h>
// #endif

// from oxygenhelper.cpp:
OxygenHelper::OxygenHelper()
 : _componentData("oxygen", 0, KComponentData::SkipMainComponentRegistration)
{
    _config = _componentData.config();
    _contrast = KGlobalSettings::contrastF(_config);
    // background contrast is calculated so that it is 0.9
    // when KGlobalSettings contrast value of 0.7
    _bgcontrast = qMin(1.0, 0.9*_contrast/0.7);
}

OxygenHelper::~OxygenHelper()
{
}

bool OxygenHelper::lowThreshold(const QColor &color)
{
    const quint32 key( color.rgba() );
    ColorMap::iterator iter( m_lowThreshold.find( key ) );
    if( iter != m_lowThreshold.end() ) return iter.value();
    else {

        const QColor darker( KColorScheme::shade(color, KColorScheme::MidShade, 0.5 ) );
        const bool result( KColorUtils::luma(darker) > KColorUtils::luma(color) );
        m_lowThreshold.insert( key, result );
        return result;

    }
}

bool OxygenHelper::highThreshold(const QColor &color)
{
    const quint32 key( color.rgba() );
    ColorMap::iterator iter( m_highThreshold.find( key ) );
    if( iter != m_highThreshold.end() ) return iter.value();
    else {

        const QColor lighter( KColorScheme::shade(color, KColorScheme::LightShade, 0.5 ) );
        const bool result( KColorUtils::luma(lighter) < KColorUtils::luma(color) );
        m_highThreshold.insert( key, result );
        return result;

    }
}

const QColor& OxygenHelper::backgroundTopColor(const QColor &color)
{
    const quint64 key( color.rgba() );
    QColor* out( m_backgroundTopColorCache.object( key ) );
    if( !out )
    {
        if( lowThreshold(color) ) out = new QColor( KColorScheme::shade(color, KColorScheme::MidlightShade, 0.0) );
        else {
            const qreal my( KColorUtils::luma( KColorScheme::shade(color, KColorScheme::LightShade, 0.0) ) );
            const qreal by( KColorUtils::luma(color) );
            out = new QColor( KColorUtils::shade(color, (my - by) * _bgcontrast) );
        }

        m_backgroundTopColorCache.insert( key, out );
    }

    return *out;

}

const QColor& OxygenHelper::backgroundBottomColor(const QColor &color)
{
    const quint64 key( color.rgba() );
    QColor* out( m_backgroundBottomColorCache.object( key ) );
    if( !out )
    {
        const QColor midColor( KColorScheme::shade(color, KColorScheme::MidShade, 0.0) );
        if( lowThreshold(color) ) out = new QColor( midColor );
        else {

            const qreal by( KColorUtils::luma(color) );
            const qreal my( KColorUtils::luma(midColor) );
            out = new QColor( KColorUtils::shade(color, (my - by) * _bgcontrast) );

        }

        m_backgroundBottomColorCache.insert( key, out );
    }

    return *out;

}

const QColor& OxygenHelper::backgroundRadialColor(const QColor &color)
{
    const quint64 key( color.rgba() );
    QColor* out( m_backgroundRadialColorCache.object( key ) );
    if( !out )
    {
        if( lowThreshold(color) ) out = new QColor( KColorScheme::shade(color, KColorScheme::LightShade, 0.0) );
        else if( highThreshold( color ) ) out = new QColor( color );
        else out = new QColor( KColorScheme::shade(color, KColorScheme::LightShade, _bgcontrast) );
        m_backgroundRadialColorCache.insert( key, out );
    }

    return *out;
}

QPixmap OxygenHelper::verticalGradient(const QColor &color, int height, int offset)
{
    const quint64 key( (quint64(color.rgba()) << 32) | height | 0x8000 );
    QPixmap *pixmap( m_backgroundCache.object( key ) );

    if (!pixmap)
    {
        pixmap = new QPixmap(1, height);
        pixmap->fill( Qt::transparent );

        QLinearGradient gradient(0, offset, 0, height+offset);
        gradient.setColorAt(0.0, backgroundTopColor(color));
        gradient.setColorAt(0.5, color);
        gradient.setColorAt(1.0, backgroundBottomColor(color));

        QPainter p(pixmap);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.fillRect(pixmap->rect(), gradient);

        p.end();

        m_backgroundCache.insert(key, pixmap);
    }

    return *pixmap;
}

QPixmap OxygenHelper::radialGradient(const QColor &color, int width, int height)
{
    const quint64 key( ( quint64(color.rgba()) << 32) | width | 0xb000 );
    QPixmap *pixmap( m_backgroundCache.object( key ) );

    if (!pixmap)
    {
        pixmap = new QPixmap(width, height);
        pixmap->fill(Qt::transparent);

        QColor radialColor = backgroundRadialColor(color);
        radialColor.setAlpha(255);
        QRadialGradient gradient(64, height-64, 64);
        gradient.setColorAt(0, radialColor);
        radialColor.setAlpha(101);
        gradient.setColorAt(0.5, radialColor);
        radialColor.setAlpha(37);
        gradient.setColorAt(0.75, radialColor);
        radialColor.setAlpha(0);
        gradient.setColorAt(1, radialColor);

        QPainter p(pixmap);
        p.scale(width/128.0,1);
        p.fillRect(QRect(0,0,128,height), gradient);

        p.end();

        m_backgroundCache.insert(key, pixmap);
    }

    return *pixmap;
}

void OxygenHelper::renderOxygenWindowBackground(QPainter *p, const QRect &clipRect,
                                         const QWidget *widget, const QWidget* window,
                                         const QColor& color, int y_shift,
                                         int gradientHeight)
{
    // get coordinates relative to the client area
    // this is stupid. One could use mapTo if this was taking const QWidget* and not
    // QWidget* as argument.
    const QWidget* w( widget );
    int x(0);
    int y(-y_shift);

    while ( w != window && !w->isWindow() && w != w->parentWidget() )
    {
        x += w->geometry().x();
        y += w->geometry().y();
        w = w->parentWidget();
    }

    if (clipRect.isValid())
    {
        p->save();
        p->setClipRegion(clipRect,Qt::IntersectClip);
    }

    // calculate upper part height
    // special tricks are needed
    // to handle both window contents and window decoration
    const QRect r = window->rect();
    int height( window->frameGeometry().height() );
    int width( window->frameGeometry().width() );
    if( y_shift > 0 )
    {
        height -= 2*y_shift;
        width -= 2*y_shift;
    }

    const int splitY( qMin(300, (3*height)/4) );

    // draw upper linear gradient
    const QRect upperRect(-x, -y, r.width(), splitY);
    QPixmap tile( verticalGradient(color, splitY, gradientHeight-64) );
    p->drawTiledPixmap(upperRect, tile);

    // draw lower flat part
    const QRect lowerRect(-x, splitY-y, r.width(), r.height() - splitY-y_shift);
    p->fillRect(lowerRect, backgroundBottomColor(color));

    // draw upper radial gradient
    const int radialW( qMin(600, width) );
    const QRect radialRect( (r.width() - radialW) / 2-x, -y, radialW, gradientHeight);
    if (clipRect.intersects(radialRect))
    {
        tile = radialGradient(color, radialW, gradientHeight);
        p->drawPixmap(radialRect, tile);
    }

    if (clipRect.isValid())
    { p->restore(); }
}

//!@name window background gradients
//@{
/*!
\par y_shift: shift the background gradient upwards, to fit with the windec
\par gradientHeight: the height of the generated gradient.
for different heights, the gradient is translated so that it is always at the same position from the bottom
*/
void OxygenHelper::renderWindowBackground(QPainter *p, const QRect &clipRect,
                                   const QWidget *widget, const QPalette & pal,
                                   int y_shift, int gradientHeight)
{
    renderOxygenWindowBackground(
                           p, clipRect, widget, widget->window(),
                           pal.color( widget->window()->backgroundRole() ),
                           y_shift, gradientHeight );
}

void OxygenHelper::renderMenuBackground( QPainter* p, const QRect& clipRect,
                                         const QWidget* widget, const QPalette& pal)
{
    renderMenuBackground( p, clipRect, widget, pal.color( widget->window()->backgroundRole() ) );
}

void OxygenHelper::renderMenuBackground( QPainter* p, const QRect& clipRect,
                                         const QWidget* widget, const QColor& color )
{

    // get coordinates relative to the client area
    // this is stupid. One could use mapTo if this was taking const QWidget* and not
    // QWidget* as argument.
    const QWidget* w( widget );
    int x(0);
    int y(0);

    while( !w->isWindow() && w != w->parentWidget() )
    {
        x += w->geometry().x();
        y += w->geometry().y();
        w = w->parentWidget();
    }

    if (clipRect.isValid()) {
        p->save();
        p->setClipRegion(clipRect,Qt::IntersectClip);
    }

    // calculate upper part height
    // special tricks are needed
    // to handle both window contents and window decoration
    QRect r = w->rect();
    const int height( w->frameGeometry().height() );
    const int splitY( qMin(200, (3*height)/4) );

    const QRect upperRect( QRect(0, 0, r.width(), splitY) );
    const QPixmap tile( verticalGradient(color, splitY) );
    p->drawTiledPixmap(upperRect, tile);

    const QRect lowerRect( 0,splitY, r.width(), r.height() - splitY );
    p->fillRect(lowerRect, backgroundBottomColor(color));

    if (clipRect.isValid())
    { p->restore(); }

}

// </oxygen>

class KexiMenuWidgetActionPrivate
{
public:
    KexiMenuWidgetActionPrivate()
     : persistentlySelected(false)
    {
    }
    bool persistentlySelected;
};

KexiMenuWidgetAction::KexiMenuWidgetAction(QObject *parent)
 : KAction(parent)
 , d(new KexiMenuWidgetActionPrivate)
{
}

KexiMenuWidgetAction::KexiMenuWidgetAction(const QString &text, QObject *parent)
 : KAction(text, parent)
 , d(new KexiMenuWidgetActionPrivate)
{
}

KexiMenuWidgetAction::KexiMenuWidgetAction(const KIcon &icon, const QString &text,
                                           QObject *parent)
 : KAction(icon, text, parent)
 , d(new KexiMenuWidgetActionPrivate)
{
}

KexiMenuWidgetAction::KexiMenuWidgetAction(KStandardAction::StandardAction id, QObject *parent)
 : KAction(parent)
 , d(new KexiMenuWidgetActionPrivate)
{
    QScopedPointer<KAction> tmp(KStandardAction::create(id, 0, 0, 0));
    setIcon(tmp->icon());
    setText(tmp->text());
    setShortcut(tmp->shortcut(DefaultShortcut), DefaultShortcut);
    setShortcut(tmp->shortcut(ActiveShortcut), ActiveShortcut);
}


void KexiMenuWidgetAction::setPersistentlySelected(bool set)
{
    if (set == d->persistentlySelected)
        return;
    d->persistentlySelected = set;
}

bool KexiMenuWidgetAction::persistentlySelected() const
{
    return d->persistentlySelected;
}

// from qobject_p.h
class QBoolBlocker
{
public:
    inline QBoolBlocker(bool &b, bool value=true):block(b), reset(b){block = value;}
    inline ~QBoolBlocker(){block = reset; }
private:
    bool &block;
    bool reset;
};

KexiMenuWidget *KexiMenuWidgetPrivate::mouseDown = 0;
int KexiMenuWidgetPrivate::sloppyDelayTimer = 0;

void KexiMenuWidgetPrivate::init()
{
    oxygenHelper = q->style()->objectName() == "oxygen" ? new OxygenHelper : 0;
    
#ifndef QT_NO_WHATSTHIS
    //q->setAttribute(Qt::WA_CustomWhatsThis);
#endif
    //q->setAttribute(Qt::WA_X11NetWmWindowTypePopupMenu);
    defaultMenuAction = menuAction = new QAction(q);
    //menuAction->d_func()->menu = q;
    q->setMouseTracking(q->style()->styleHint(QStyle::SH_Menu_MouseTracking, 0, q));
    if (q->style()->styleHint(QStyle::SH_Menu_Scrollable, 0, q)) {
        scroll = new KexiMenuWidgetPrivate::QMenuScroller;
        scroll->scrollFlags = KexiMenuWidgetPrivate::QMenuScroller::ScrollNone;
    }

#ifdef QT_SOFTKEYS_ENABLED
    selectAction = QSoftKeyManager::createKeyedAction(QSoftKeyManager::SelectSoftKey, Qt::Key_Select, q);
    cancelAction = QSoftKeyManager::createKeyedAction(QSoftKeyManager::CancelSoftKey, Qt::Key_Back, q);
    selectAction->setPriority(QAction::HighPriority);
    cancelAction->setPriority(QAction::HighPriority);
    q->addAction(selectAction);
    q->addAction(cancelAction);
#endif
    q->setFocusPolicy(Qt::StrongFocus);
}

int KexiMenuWidgetPrivate::scrollerHeight() const
{
    return qMax(QApplication::globalStrut().height(), q->style()->pixelMetric(QStyle::PM_MenuScrollerHeight, 0, q));
}

//Windows and KDE allows menus to cover the taskbar, while GNOME and Mac don't
QRect KexiMenuWidgetPrivate::popupGeometry(const QWidget *widget) const
{
    return QApplication::desktop()->screenGeometry(widget);
}

//Windows and KDE allows menus to cover the taskbar, while GNOME and Mac don't
QRect KexiMenuWidgetPrivate::popupGeometry(int screen) const
{
    return QApplication::desktop()->screenGeometry(screen);
}

QList<QPointer<QWidget> > KexiMenuWidgetPrivate::calcCausedStack() const
{
    QList<QPointer<QWidget> > ret;
    for(QWidget *widget = causedPopup.widget; widget; ) {
        ret.append(widget);
        if (KexiMenuWidget *qmenu = qobject_cast<KexiMenuWidget*>(widget))
            widget = qmenu->causedPopup().widget;
        else
            break;
    }
    return ret;
}

void KexiMenuWidgetPrivate::updateActionRects() const
{
    if (!itemsDirty)
        return;

    q->ensurePolished();

    //let's reinitialize the buffer
    QList<QAction*> actionsList = q->actions();
    actionRects.resize(actionsList.count());
    actionRects.fill(QRect());

    //let's try to get the last visible action
    int lastVisibleAction = actionsList.count() - 1;
    for(;lastVisibleAction >= 0; --lastVisibleAction) {
        const QAction *action = actionsList.at(lastVisibleAction);
        if (action->isVisible()) {
            //removing trailing separators
            if (action->isSeparator() && collapsibleSeparators)
                continue;
            break;
        }
    }

    int max_column_width = 0,
        dh = popupGeometry(q).height(),
        y = 0;
    QStyle *style = q->style();
    QStyleOption opt;
    opt.init(q);
    const int hmargin = style->pixelMetric(QStyle::PM_MenuHMargin, &opt, q),
              vmargin = style->pixelMetric(QStyle::PM_MenuVMargin, &opt, q),
              icone = KIconLoader::SizeMedium;
#warning todo adjust this size for smaller displays
              //style->pixelMetric(QStyle::PM_SmallIconSize, &opt, q);
    const int fw = style->pixelMetric(QStyle::PM_MenuPanelWidth, &opt, q);
    const int deskFw = frameWidth(&opt);
    //const int tearoffHeight = tearoff ? style->pixelMetric(QStyle::PM_MenuTearoffHeight, &opt, q) : 0;

    //for compatibility now - will have to refactor this away
    tabWidth = 0;
    maxIconWidth = 0;
    hasCheckableItems = false;
    ncols = 1;
    sloppyAction = 0;

    for (int i = 0; i < actionsList.count(); ++i) {
        QAction *action = actionsList.at(i);
        if (action->isSeparator() || !action->isVisible() || widgetItems.contains(action))
            continue;
        //..and some members
        hasCheckableItems |= action->isCheckable();
        QIcon is = action->icon();
        if (!is.isNull()) {
            maxIconWidth = qMax<uint>(maxIconWidth, icone + 4);
        }
    }

    //calculate size
    QFontMetrics qfm = q->fontMetrics();
    bool previousWasSeparator = true; // this is true to allow removing the leading separators
    for(int i = 0; i <= lastVisibleAction; i++) {
        QAction *action = actionsList.at(i);

        if (!action->isVisible() ||
            (collapsibleSeparators && previousWasSeparator && action->isSeparator()))
            continue; // we continue, this action will get an empty QRect

        previousWasSeparator = action->isSeparator();

        //let the style modify the above size..
        QStyleOptionMenuItem opt;
        q->initStyleOption(&opt, action);
        const QFontMetrics &fm = opt.fontMetrics;

        QSize sz;
        if (QWidget *w = widgetItems.value(action)) {
          sz = w->sizeHint().expandedTo(w->minimumSize()).expandedTo(w->minimumSizeHint()).boundedTo(w->maximumSize());
        } else {
            //calc what I think the size is..
            if (action->isSeparator()) {
                sz = QSize(2, 2);
            } else {
                QString s = action->text();
                int t = s.indexOf(QLatin1Char('\t'));
                if (t != -1) {
                    tabWidth = qMax(int(tabWidth), qfm.width(s.mid(t+1)));
                    s = s.left(t);
    #ifndef QT_NO_SHORTCUT
                } else {
                    QKeySequence seq = action->shortcut();
                    if (!seq.isEmpty())
                        tabWidth = qMax(int(tabWidth), qfm.width(seq));
    #endif
                }
                sz.setWidth(fm.boundingRect(QRect(), Qt::TextSingleLine | Qt::TextShowMnemonic, s).width());
                sz.setHeight(qMax(fm.height(), qfm.height()));

                QIcon is = action->icon();
                //(not need because iconless items have to be of the same size as icon ones): if (!is.isNull()) {
                    QSize is_sz = QSize(icone, icone);
                    if (is_sz.height() > sz.height())
                        sz.setHeight(is_sz.height());
                //}
            }
            sz = style->sizeFromContents(QStyle::CT_MenuItem, &opt, sz, q);
        }


        if (!sz.isEmpty()) {
            max_column_width = qMax(max_column_width, sz.width());
            //wrapping
            if (!scroll &&
               y+sz.height()+vmargin > dh - (deskFw * 2)) {
                ncols++;
                y = vmargin;
            }
            y += sz.height();
            //update the item
            actionRects[i] = QRect(0, 0, sz.width(), sz.height());
        }
    }

    max_column_width += tabWidth; //finally add in the tab width
    const int sfcMargin = style->sizeFromContents(QStyle::CT_Menu, &opt, QApplication::globalStrut(), q).width() - QApplication::globalStrut().width();
    int leftmargin, topmargin, rightmargin, bottommargin;
    q->getContentsMargins(&leftmargin, &topmargin, &rightmargin, &bottommargin);
    const int min_column_width = q->minimumWidth() - (sfcMargin + leftmargin + rightmargin + 2 * (fw + hmargin));
    max_column_width = qMax(min_column_width, max_column_width);


    //calculate position
    const int base_y = vmargin + fw + topmargin +
        (scroll ? scroll->scrollOffset : 0)
        /*+ tearoffHeight*/;
    int x = hmargin + fw + leftmargin;
    y = base_y;

    for(int i = 0; i < actionsList.count(); i++) {
        QRect &rect = actionRects[i];
        if (rect.isNull())
            continue;
        if (!scroll &&
           y+rect.height() > dh - deskFw * 2) {
            x += max_column_width + hmargin;
            y = base_y;
        }
        rect.translate(x, y);                        //move
        rect.setWidth(max_column_width); //uniform width

        //we need to update the widgets geometry
        if (QWidget *widget = widgetItems.value(actionsList.at(i))) {
            widget->setGeometry(rect);
            widget->setVisible(actionsList.at(i)->isVisible());
        }

        y += rect.height();
    }
    itemsDirty = 0;
}

QRect KexiMenuWidgetPrivate::actionRect(QAction *act) const
{
    QList<QAction*> actionsList = q->actions();
    int index = actionsList.indexOf(act);
    if (index == -1)
        return QRect();

    updateActionRects();

    //we found the action
    return actionRects.at(index);
}

void KexiMenuWidgetPrivate::hideUpToMenuBar()
{
    bool fadeMenus = q->style()->styleHint(QStyle::SH_Menu_FadeOutOnHide);
    QWidget *caused = causedPopup.widget;
    hideMenu(q); //hide after getting causedPopup
    while(caused) {
        if (KexiMenuWidget *m = qobject_cast<KexiMenuWidget*>(caused)) {
            caused = m->causedPopup().widget;
            hideMenu(m, fadeMenus);
            if (!fadeMenus) // Mac doesn't clear the action until after hidden.
                m->setCurrentAction(0);
        } else {
            caused = 0;
        }
    }
    setCurrentAction(0);
}

void KexiMenuWidgetPrivate::hideMenu(KexiMenuWidget *menu, bool justRegister)
{
    return; // js
    if (!menu)
        return;
    if (!justRegister)
        menu->hide();
}

#if 0
void KexiMenuWidgetPrivate::popupAction(QAction *action, int delay, bool activateFirst)
{
    Q_UNUSED(activateFirst);
    if (action && action->isEnabled()) {
        if (!delay)
            q->internalDelayedPopup();
        else if (!menuDelayTimer.isActive() && (!action->menu() || !action->menu()->isVisible()))
            menuDelayTimer.start(delay, q);
/*        if (activateFirst && action->menu())
            action->menu()->d_func()->setFirstActionActive();*/
    } else if (KexiMenuWidget *menu = activeMenu) {  //hide the current item
        activeMenu = 0;
        hideMenu(menu);
    }
}
#endif

void KexiMenuWidgetPrivate::setSyncAction()
{
    QAction *current = currentAction;
    if(current && (!current->isEnabled() || current->menu() || current->isSeparator()))
        current = 0;
    for(QWidget *caused = q; caused;) {
        if (KexiMenuWidget *m = qobject_cast<KexiMenuWidget*>(caused)) {
            caused = m->causedPopup().widget;
            if (m->eventLoop())
                m->setSyncAction(current); // synchronous operation
        } else {
            break;
        }
    }
}


void KexiMenuWidgetPrivate::setFirstActionActive()
{
    updateActionRects();
    QList<QAction*> actionsList = q->actions();
    for(int i = 0, saccum = 0; i < actionsList.count(); i++) {
        const QRect &rect = actionRects.at(i);
        if (rect.isNull())
            continue;
        if (scroll && scroll->scrollFlags & QMenuScroller::ScrollUp) {
            saccum -= rect.height();
            if (saccum > scroll->scrollOffset - scrollerHeight())
                continue;
        }
        QAction *act = actionsList.at(i);
        if (!act->isSeparator() &&
           (q->style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, q)
            || act->isEnabled())) {
            setCurrentAction(act);
            break;
        }
    }
}

// popup == -1 means do not popup, 0 means immediately, others mean use a timer
void KexiMenuWidgetPrivate::setCurrentAction(QAction *action, int popup, KexiMenuWidget::SelectionReason reason, bool activateFirst)
{
    Q_UNUSED(activateFirst);
    //tearoffHighlighted = 0;
    if (currentAction)
        q->update(actionRect(currentAction));

    sloppyAction = 0;
    if (!sloppyRegion.isEmpty())
        sloppyRegion = QRegion();
    KexiMenuWidget *hideActiveMenu = activeMenu;
#ifndef QT_NO_STATUSTIP
    QAction *previousAction = currentAction;
#endif

    currentAction = action;
    if (action) {
        if (!action->isSeparator()) {
            activateAction(action, QAction::Hover);
            /*if (popup != -1) {
                hideActiveMenu = 0; //will be done "later"
                // if the menu is visible then activate the required action,
                // otherwise we just mark the action as currentAction
                // and activate it when the menu will be popuped.
                if (q->isVisible())
                    popupAction(currentAction, popup, activateFirst);
            }*/
            q->update(actionRect(action));

            if (reason == KexiMenuWidget::SelectedFromKeyboard) {
                QWidget *widget = widgetItems.value(action);
                if (widget) {
                    if (widget->focusPolicy() != Qt::NoFocus)
                        widget->setFocus(Qt::TabFocusReason);
                } else {
                    //when the action has no QWidget, the KexiMenuWidget itself should
                    // get the focus
                    // Since the menu is a pop-up, it uses the popup reason.
                    if (!q->hasFocus()) {
                        q->setFocus(Qt::PopupFocusReason);
                    }
                }
            }
        } else { //action is a separator
            if (popup != -1)
                hideActiveMenu = 0; //will be done "later"
        }
#ifndef QT_NO_STATUSTIP
    }  else if (previousAction) {
        //previousAction->d_func()->showStatusText(topCausedWidget(), QString());
#endif
    }
    if (hideActiveMenu) {
        activeMenu = 0;
        hideMenu(hideActiveMenu);
    }
}

//return the top causedPopup.widget that is not a KexiMenuWidget
QWidget *KexiMenuWidgetPrivate::topCausedWidget() const
{
    QWidget* top = causedPopup.widget;
    while (KexiMenuWidget* m = qobject_cast<KexiMenuWidget *>(top))
        top = m->causedPopup().widget;
    return top;
}

QAction *KexiMenuWidgetPrivate::actionAt(QPoint p) const
{
    if (!q->rect().contains(p))     //sanity check
       return 0;

    QList<QAction*> actionsList = q->actions();
    for(int i = 0; i < actionRects.count(); i++) {
        if (actionRects.at(i).contains(p))
            return actionsList.at(i);
    }
    return 0;
}

void KexiMenuWidgetPrivate::setOverrideMenuAction(QAction *a)
{
    QObject::disconnect(menuAction, SIGNAL(destroyed()), q, SLOT(overrideMenuActionDestroyed()));
    if (a) {
        menuAction = a;
        QObject::connect(a, SIGNAL(destroyed()), q, SLOT(overrideMenuActionDestroyed()));
    } else { //we revert back to the default action created by the KexiMenuWidget itself
        menuAction = defaultMenuAction;
    }
}

void KexiMenuWidget::overrideMenuActionDestroyed()
{
    d->menuAction = d->defaultMenuAction;
}


void KexiMenuWidgetPrivate::updateLayoutDirection()
{
    //we need to mimic the cause of the popup's layout direction
    //to allow setting it on a mainwindow for example
    //we call setLayoutDirection_helper to not overwrite a user-defined value
    if (!q->testAttribute(Qt::WA_SetLayoutDirection)) {
        if (QWidget *w = causedPopup.widget)
            setLayoutDirection_helper(w, w->layoutDirection());
        else if (QWidget *w = q->parentWidget())
            setLayoutDirection_helper(w, w->layoutDirection());
        else
            setLayoutDirection_helper(w, QApplication::layoutDirection());
    }
}

bool KexiMenuWidgetPrivate::actionPersistentlySelected(const QAction* action) const
{
    const KexiMenuWidgetAction* kaction = qobject_cast<const KexiMenuWidgetAction*>(action);
    return kaction ? kaction->persistentlySelected() : false;
}

void KexiMenuWidgetPrivate::setActionPersistentlySelected(QAction* action, bool set)
{
    KexiMenuWidgetAction* kaction = qobject_cast<KexiMenuWidgetAction*>(action);
    if (previousPersistentlySelectedAction)
        previousPersistentlySelectedAction->setPersistentlySelected(false);
    if (kaction)
        kaction->setPersistentlySelected(set);
    previousPersistentlySelectedAction = kaction;
}

void KexiMenuWidgetPrivate::toggleActionPersistentlySelected(QAction* action)
{
    KexiMenuWidgetAction* kaction = qobject_cast<KexiMenuWidgetAction*>(action);
    if (!kaction)
        return;
    setActionPersistentlySelected(kaction, !kaction->persistentlySelected());
}

/*!
    Returns the action associated with this menu.
*/
QAction *KexiMenuWidget::menuAction() const
{
    return d->menuAction;
}

/*!
  \property KexiMenuWidget::title
  \brief The title of the menu

  This is equivalent to the QAction::text property of the menuAction().

  By default, this property contains an empty string.
*/
QString KexiMenuWidget::title() const
{
    return d->menuAction->text();
}

void KexiMenuWidget::setTitle(const QString &text)
{
    d->menuAction->setText(text);
}

/*!
  \property KexiMenuWidget::icon

  \brief The icon of the menu

  This is equivalent to the QAction::icon property of the menuAction().

  By default, if no icon is explicitly set, this property contains a null icon.
*/
QIcon KexiMenuWidget::icon() const
{
    return d->menuAction->icon();
}

void KexiMenuWidget::setIcon(const QIcon &icon)
{
    d->menuAction->setIcon(icon);
}


//actually performs the scrolling
void KexiMenuWidgetPrivate::scrollMenu(QAction *action, QMenuScroller::ScrollLocation location, bool active)
{
    return;
    if (!scroll || !scroll->scrollFlags)
        return;
    updateActionRects();
    int newOffset = 0;
    const int topScroll = (scroll->scrollFlags & QMenuScroller::ScrollUp)   ? scrollerHeight() : 0;
    const int botScroll = (scroll->scrollFlags & QMenuScroller::ScrollDown) ? scrollerHeight() : 0;
    const int vmargin = q->style()->pixelMetric(QStyle::PM_MenuVMargin, 0, q);
    const int fw = q->style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, q);

    QList<QAction*> actionsList = q->actions();
    if (location == QMenuScroller::ScrollTop) {
        for(int i = 0, saccum = 0; i < actionsList.count(); i++) {
            if (actionsList.at(i) == action) {
                newOffset = topScroll - saccum;
                break;
            }
            saccum += actionRects.at(i).height();
        }
    } else {
        for(int i = 0, saccum = 0; i < actionsList.count(); i++) {
            saccum += actionRects.at(i).height();
            if (actionsList.at(i) == action) {
                if (location == QMenuScroller::ScrollCenter)
                    newOffset = ((q->height() / 2) - botScroll) - (saccum - topScroll);
                else
                    newOffset = (q->height() - botScroll) - saccum;
                break;
            }
        }
        if(newOffset)
            newOffset -= fw * 2;
    }

    //figure out which scroll flags
    uint newScrollFlags = QMenuScroller::ScrollNone;
    if (newOffset < 0) //easy and cheap one
        newScrollFlags |= QMenuScroller::ScrollUp;
    int saccum = newOffset;
    for(int i = 0; i < actionRects.count(); i++) {
        saccum += actionRects.at(i).height();
        if (saccum > q->height()) {
            newScrollFlags |= QMenuScroller::ScrollDown;
            break;
        }
    }

    if (!(newScrollFlags & QMenuScroller::ScrollDown) && (scroll->scrollFlags & QMenuScroller::ScrollDown)) {
        newOffset = q->height() - (saccum - newOffset) - fw*2 - vmargin;    //last item at bottom
    }

    if (!(newScrollFlags & QMenuScroller::ScrollUp) && (scroll->scrollFlags & QMenuScroller::ScrollUp)) {
        newOffset = 0;  //first item at top
    }

    if (newScrollFlags & QMenuScroller::ScrollUp)
        newOffset -= vmargin;

    QRect screen = popupGeometry(q);
    const int desktopFrame = frameWidth();
    if (q->height() < screen.height()-(desktopFrame*2)-1) {
        QRect geom = q->geometry();
        if (newOffset > scroll->scrollOffset && (scroll->scrollFlags & newScrollFlags & QMenuScroller::ScrollUp)) { //scroll up
            const int newHeight = geom.height()-(newOffset-scroll->scrollOffset);
            if(newHeight > geom.height())
                geom.setHeight(newHeight);
        } else if(scroll->scrollFlags & newScrollFlags & QMenuScroller::ScrollDown) {
            int newTop = geom.top() + (newOffset-scroll->scrollOffset);
            if (newTop < desktopFrame+screen.top())
                newTop = desktopFrame+screen.top();
            if (newTop < geom.top()) {
                geom.setTop(newTop);
                newOffset = 0;
                newScrollFlags &= ~QMenuScroller::ScrollUp;
            }
        }
        if (geom.bottom() > screen.bottom() - desktopFrame)
            geom.setBottom(screen.bottom() - desktopFrame);
        if (geom.top() < desktopFrame+screen.top())
            geom.setTop(desktopFrame+screen.top());
        if (geom != q->geometry()) {
#if 0
            if (newScrollFlags & QMenuScroller::ScrollDown &&
               q->geometry().top() - geom.top() >= -newOffset)
                newScrollFlags &= ~QMenuScroller::ScrollDown;
#endif
            q->setGeometry(geom);
        }
    }

    //actually update flags
    const int delta = qMin(0, newOffset) - scroll->scrollOffset; //make sure the new offset is always negative
    if (!itemsDirty && delta) {
        //we've scrolled so we need to update the action rects
        for (int i = 0; i < actionRects.count(); ++i) {
            QRect &current = actionRects[i];
            current.moveTop(current.top() + delta);

            //we need to update the widgets geometry
            if (QWidget *w = widgetItems.value(actionsList.at(i)))
                w->setGeometry(current);
        }
    }
    scroll->scrollOffset += delta;
    scroll->scrollFlags = newScrollFlags;
    if (active)
        setCurrentAction(action);

    q->update();     //issue an update so we see all the new state..
}

void KexiMenuWidgetPrivate::scrollMenu(QMenuScroller::ScrollLocation location, bool active)
{
    return;
    updateActionRects();
    QList<QAction*> actionsList = q->actions();
    if(location == QMenuScroller::ScrollBottom) {
        for(int i = actionsList.size()-1; i >= 0; --i) {
            QAction *act = actionsList.at(i);
            if (actionRects.at(i).isNull())
                continue;
            if (!act->isSeparator() &&
                (q->style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, q)
                 || act->isEnabled())) {
                if(scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollDown)
                    scrollMenu(act, KexiMenuWidgetPrivate::QMenuScroller::ScrollBottom, active);
                else if(active)
                    setCurrentAction(act, /*popup*/-1, KexiMenuWidget::SelectedFromKeyboard);
                break;
            }
        }
    } else if(location == QMenuScroller::ScrollTop) {
        for(int i = 0; i < actionsList.size(); ++i) {
            QAction *act = actionsList.at(i);
            if (actionRects.at(i).isNull())
                continue;
            if (!act->isSeparator() &&
                (q->style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, q)
                 || act->isEnabled())) {
                if(scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollUp)
                    scrollMenu(act, KexiMenuWidgetPrivate::QMenuScroller::ScrollTop, active);
                else if(active)
                    setCurrentAction(act, /*popup*/-1, KexiMenuWidget::SelectedFromKeyboard);
                break;
            }
        }
    }
}

//only directional
void KexiMenuWidgetPrivate::scrollMenu(QMenuScroller::ScrollDirection direction, bool page, bool active)
{
    return;
    if (!scroll || !(scroll->scrollFlags & direction)) //not really possible...
        return;
    updateActionRects();
    const int topScroll = (scroll->scrollFlags & QMenuScroller::ScrollUp)   ? scrollerHeight() : 0;
    const int botScroll = (scroll->scrollFlags & QMenuScroller::ScrollDown) ? scrollerHeight() : 0;
    const int vmargin = q->style()->pixelMetric(QStyle::PM_MenuVMargin, 0, q);
    const int fw = q->style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, q);
    const int offset = topScroll ? topScroll-vmargin : 0;
    QList<QAction*> actionsList = q->actions();
    if (direction == QMenuScroller::ScrollUp) {
        for(int i = 0, saccum = 0; i < actionsList.count(); i++) {
            saccum -= actionRects.at(i).height();
            if (saccum <= scroll->scrollOffset-offset) {
                scrollMenu(actionsList.at(i), page ? QMenuScroller::ScrollBottom : QMenuScroller::ScrollTop, active);
                break;
            }
        }
    } else if (direction == QMenuScroller::ScrollDown) {
        bool scrolled = false;
        for(int i = 0, saccum = 0; i < actionsList.count(); i++) {
            const int iHeight = actionRects.at(i).height();
            saccum -= iHeight;
            if (saccum <= scroll->scrollOffset-offset) {
                const int scrollerArea = q->height() - botScroll - fw*2;
                int visible = (scroll->scrollOffset-offset) - saccum;
                for(i++ ; i < actionsList.count(); i++) {
                    visible += actionRects.at(i).height();
                    if (visible > scrollerArea - topScroll) {
                        scrolled = true;
                        scrollMenu(actionsList.at(i), page ? QMenuScroller::ScrollTop : QMenuScroller::ScrollBottom, active);
                        break;
                    }
                }
                break;
            }
        }
        if(!scrolled) {
            scroll->scrollFlags &= ~QMenuScroller::ScrollDown;
            q->update();
        }
    }
}

/* This is poor-mans eventfilters. This avoids the use of
   eventFilter (which can be nasty for users of QMenuBar's). */
bool KexiMenuWidgetPrivate::mouseEventTaken(QMouseEvent *e)
{
    QPoint pos = q->mapFromGlobal(e->globalPos());
    if (scroll && !activeMenu) { //let the scroller "steal" the event
        bool isScroll = false;
        if (pos.x() >= 0 && pos.x() < q->width()) {
            for(int dir = QMenuScroller::ScrollUp; dir <= QMenuScroller::ScrollDown; dir = dir << 1) {
                if (scroll->scrollFlags & dir) {
                    if (dir == QMenuScroller::ScrollUp)
                        isScroll = (pos.y() <= scrollerHeight());
                    else if (dir == QMenuScroller::ScrollDown)
                        isScroll = (pos.y() >= q->height() - scrollerHeight());
                    if (isScroll) {
                        scroll->scrollDirection = dir;
                        break;
                    }
                }
            }
        }
        if (isScroll) {
            scroll->scrollTimer.start(50, q);
            return true;
        } else {
            scroll->scrollTimer.stop();
        }
    }

//     if (tearoff) { //let the tear off thingie "steal" the event..
//         QRect tearRect(0, 0, q->width(), q->style()->pixelMetric(QStyle::PM_MenuTearoffHeight, 0, q));
//         if (scroll && scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollUp)
//             tearRect.translate(0, scrollerHeight());
//         q->update(tearRect);
//         if (tearRect.contains(pos) && hasMouseMoved(e->globalPos())) {
//             setCurrentAction(0);
//             tearoffHighlighted = 1;
//             if (e->type() == QEvent::MouseButtonRelease) {
//                 hideUpToMenuBar();
//             }
//             return true;
//         }
//         tearoffHighlighted = 0;
//     }

    if (q->frameGeometry().contains(e->globalPos())) //otherwise if the event is in our rect we want it..
        return false;

    for(QWidget *caused = causedPopup.widget; caused;) {
        bool passOnEvent = false;
        QWidget *next_widget = 0;
        QPoint cpos = caused->mapFromGlobal(e->globalPos());
        if (KexiMenuWidget *m = qobject_cast<KexiMenuWidget*>(caused)) {
            passOnEvent = m->rect().contains(cpos);
            next_widget = m->causedPopup().widget;
        }
        if (passOnEvent) {
            if(e->type() != QEvent::MouseButtonRelease || mouseDown == caused) {
            QMouseEvent new_e(e->type(), cpos, e->button(), e->buttons(), e->modifiers());
            QApplication::sendEvent(caused, &new_e);
            return true;
        }
        }
        if (!next_widget)
            break;
        caused = next_widget;
    }
    return false;
}

void KexiMenuWidgetPrivate::activateCausedStack(const QList<QPointer<QWidget> > &causedStack, QAction *action, QAction::ActionEvent action_e, bool self)
{
    QBoolBlocker guard(activationRecursionGuard);
    if(self)
        action->activate(action_e);

    for(int i = 0; i < causedStack.size(); ++i) {
        QPointer<QWidget> widget = causedStack.at(i);
        if (!widget)
            continue;
        //fire
        if (KexiMenuWidget *qmenu = qobject_cast<KexiMenuWidget*>(widget)) {
            widget = qmenu->causedPopup().widget;
            if (action_e == QAction::Trigger) {
                emit qmenu->triggered(action);
           } else if (action_e == QAction::Hover) {
                emit qmenu->hovered(action);
            }
        }
    }
}

void KexiMenuWidgetPrivate::activateAction(QAction *action, QAction::ActionEvent action_e, bool self)
{
#ifndef QT_NO_WHATSTHIS
    bool inWhatsThisMode = QWhatsThis::inWhatsThisMode();
#endif
    if (!action || !q->isEnabled()
        || (action_e == QAction::Trigger
#ifndef QT_NO_WHATSTHIS
            && !inWhatsThisMode
#endif
            && (action->isSeparator() ||!action->isEnabled())))
        return;

    /* I have to save the caused stack here because it will be undone after popup execution (ie in the hide).
       Then I iterate over the list to actually send the events. --Sam
    */
    const QList<QPointer<QWidget> > causedStack = calcCausedStack();
    if (action_e == QAction::Trigger) {
#ifndef QT_NO_WHATSTHIS
        if (!inWhatsThisMode)
            actionAboutToTrigger = action;
#endif

        if (q->testAttribute(Qt::WA_DontShowOnScreen)) {
            hideUpToMenuBar();
        } else {
            for(QWidget *widget = QApplication::activePopupWidget(); widget; ) {
                if (KexiMenuWidget *qmenu = qobject_cast<KexiMenuWidget*>(widget)) {
                    if(qmenu == q)
                        hideUpToMenuBar();
                    widget = qmenu->causedPopup().widget;
                } else {
                    break;
                }
            }
        }

#ifndef QT_NO_WHATSTHIS
        if (inWhatsThisMode) {
            QString s = action->whatsThis();
            if (s.isEmpty())
                s = q->whatsThis();
            QWhatsThis::showText(q->mapToGlobal(actionRect(action).center()), s, q);
            return;
        }
#endif
    }


    activateCausedStack(causedStack, action, action_e, self);


    if (action_e == QAction::Hover) {
#ifndef QT_NO_ACCESSIBILITY
        if (QAccessible::isActive()) {
            int actionIndex = indexOf(action) + 1;
            QAccessible::updateAccessibility(q, actionIndex, QAccessible::Focus);
            QAccessible::updateAccessibility(q, actionIndex, QAccessible::Selection);
        }
#endif
        action->showStatusText(topCausedWidget());
    } else {
        actionAboutToTrigger = 0;
    }
}

void KexiMenuWidget::actionTriggered()
{
    if (QAction *action = qobject_cast<QAction *>(sender())) {
        QWeakPointer<QAction> actionGuard = action;
        emit triggered(action);

        if (!d->activationRecursionGuard && actionGuard) {
            //in case the action has not been activated by the mouse
            //we check the parent hierarchy
            QList< QPointer<QWidget> > list;
            for(QWidget *widget = parentWidget(); widget; ) {
                if (qobject_cast<KexiMenuWidget*>(widget))
                {
                    list.append(widget);
                    widget = widget->parentWidget();
                } else {
                    break;
                }
            }
            d->activateCausedStack(list, action, QAction::Trigger, false);
        }
    }
}

void KexiMenuWidget::actionHovered()
{
    if (QAction * action = qobject_cast<QAction *>(sender())) {
        emit hovered(action);
    }
}

bool KexiMenuWidgetPrivate::hasMouseMoved(const QPoint &globalPos)
{
    //determines if the mouse has moved (ie its initial position has
    //changed by more than QApplication::startDragDistance()
    //or if there were at least 6 mouse motions)
    return motions > 6 ||
        QApplication::startDragDistance() < (mousePopupPos - globalPos).manhattanLength();
}

// from QWidgetPrivate
void KexiMenuWidgetPrivate::setLayoutDirection_helper(QWidget* w, Qt::LayoutDirection direction)
{
    if (!w || (direction == Qt::RightToLeft) == w->testAttribute(Qt::WA_RightToLeft))
        return;
    w->setAttribute(Qt::WA_RightToLeft, (direction == Qt::RightToLeft));
    foreach (QObject *obj, w->children()) {
        QWidget *widget = qobject_cast<QWidget*>(obj);
        if (widget && !widget->isWindow() && !widget->testAttribute(Qt::WA_SetLayoutDirection))
            setLayoutDirection_helper(widget, direction);
    }
    QEvent e(QEvent::LayoutDirectionChange);
    QApplication::sendEvent(w, &e);
}

int KexiMenuWidgetPrivate::frameWidth(const QStyleOption* opt) const
{
    if (!hasFrame)
        return 0;
    return q->style()->pixelMetric(QStyle::PM_MenuPanelWidth, opt, q);
}

bool KexiMenuWidget::hasFrame() const
{
    return d->hasFrame;
}

void KexiMenuWidget::setFrame(bool set)
{
    d->hasFrame = set;
}

// void QWidgetPrivate::resolveLayoutDirection()
// {
//     Q_Q(const QWidget);
//     if (!q->testAttribute(Qt::WA_SetLayoutDirection))
//         setLayoutDirection_helper(q->isWindow() ? QApplication::layoutDirection() : q->parentWidget()->layoutDirection());
// }

/*!
    Initialize \a option with the values from this menu and information from \a action. This method
    is useful for subclasses when they need a QStyleOptionMenuItem, but don't want
    to fill in all the information themselves.

    \sa QStyleOption::initFrom() QMenuBar::initStyleOption()
*/
void KexiMenuWidget::initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const
{
    if (!option || !action)
        return;

    option->initFrom(this);
    option->palette = palette();
    option->state = QStyle::State_None;

    if (window()->isActiveWindow())
        option->state |= QStyle::State_Active;
    if (isEnabled() && action->isEnabled()
            && (!action->menu() || action->menu()->isEnabled()))
        option->state |= QStyle::State_Enabled;
    else
        option->palette.setCurrentColorGroup(QPalette::Disabled);

    option->font = action->font().resolve(font());
    option->fontMetrics = QFontMetrics(option->font);

    if (d->currentAction && d->currentAction == action && !d->currentAction->isSeparator()) {
        option->state |= QStyle::State_Selected
                     | (d->mouseDown ? QStyle::State_Sunken : QStyle::State_None);
    }

    option->menuHasCheckableItems = d->hasCheckableItems;
    if (!action->isCheckable()) {
        option->checkType = QStyleOptionMenuItem::NotCheckable;
    } else {
        option->checkType = (action->actionGroup() && action->actionGroup()->isExclusive())
                            ? QStyleOptionMenuItem::Exclusive : QStyleOptionMenuItem::NonExclusive;
        option->checked = action->isChecked();
    }
    if (action->menu())
        option->menuItemType = QStyleOptionMenuItem::SubMenu;
    else if (action->isSeparator())
        option->menuItemType = QStyleOptionMenuItem::Separator;
    else if (d->defaultAction == action)
        option->menuItemType = QStyleOptionMenuItem::DefaultItem;
    else
        option->menuItemType = QStyleOptionMenuItem::Normal;
    if (action->isIconVisibleInMenu())
        option->icon = action->icon();
    QString textAndAccel = action->text();
#ifndef QT_NO_SHORTCUT
    if (textAndAccel.indexOf(QLatin1Char('\t')) == -1) {
        QKeySequence seq = action->shortcut();
        if (!seq.isEmpty())
            textAndAccel += QLatin1Char('\t') + QString(seq);
    }
#endif
    option->text = textAndAccel;
    option->tabWidth = d->tabWidth;
    option->maxIconWidth = d->maxIconWidth;
    option->menuRect = rect();
}

/*!
    Constructs a menu with parent \a parent.

    Although a popup menu is always a top-level widget, if a parent is
    passed the popup menu will be deleted when that parent is
    destroyed (as with any other QObject).
*/
KexiMenuWidget::KexiMenuWidget(QWidget *parent)
    : QWidget(parent), d(new KexiMenuWidgetPrivate(this))
{
    d->init();
}

/*!
    Constructs a menu with a \a title and a \a parent.

    Although a popup menu is always a top-level widget, if a parent is
    passed the popup menu will be deleted when that parent is
    destroyed (as with any other QObject).

    \sa title
*/
KexiMenuWidget::KexiMenuWidget(const QString &title, QWidget *parent)
    : QWidget(parent), d(new KexiMenuWidgetPrivate(this))
{
    d->init();
    d->menuAction->setText(title);
}

/*! \internal
 */
// KexiMenuWidget::KexiMenuWidget(KexiMenuWidgetPrivate &dd, QWidget *parent)
//     : QWidget(dd, parent, Qt::Popup)
// {
//     d->init();
// }

/*!
    Destroys the menu.
*/
KexiMenuWidget::~KexiMenuWidget()
{
    if (!d->widgetItems.isEmpty()) {  // avoid detach on shared null hash
        QHash<QAction *, QWidget *>::iterator it = d->widgetItems.begin();
        for (; it != d->widgetItems.end(); ++it) {
            if (QWidget *widget = it.value()) {
                QWidgetAction *action = static_cast<QWidgetAction *>(it.key());
                action->releaseWidget(widget);
                *it = 0;
            }
        }
    }

    if (d->eventLoop)
        d->eventLoop->exit();
    //hideTearOffMenu();
    delete d;
}

const KexiMenuWidgetCaused& KexiMenuWidget::causedPopup() const
{
    return d->causedPopup;
}

void KexiMenuWidget::setCurrentAction(QAction *a, int popup, KexiMenuWidget::SelectionReason reason, bool activateFirst)
{
    d->setCurrentAction(a, popup, reason, activateFirst);
}

QEventLoop *KexiMenuWidget::eventLoop() const
{
    return d->eventLoop;
}

void KexiMenuWidget::setSyncAction(QAction *a)
{
    d->syncAction = a;
}

/*!
    \overload

    This convenience function creates a new action with \a text.
    The function adds the newly created action to the menu's
    list of actions, and returns it.

    \sa QWidget::addAction()
*/
QAction *KexiMenuWidget::addAction(const QString &text)
{
    QAction *ret = new QAction(text, this);
    addAction(ret);
    return ret;
}

/*!
    \overload

    This convenience function creates a new action with an \a icon
    and some \a text. The function adds the newly created action to
    the menu's list of actions, and returns it.

    \sa QWidget::addAction()
*/
QAction *KexiMenuWidget::addAction(const QIcon &icon, const QString &text)
{
    QAction *ret = new QAction(icon, text, this);
    addAction(ret);
    return ret;
}

/*!
    \overload

    This convenience function creates a new action with the text \a
    text and an optional shortcut \a shortcut. The action's
    \l{QAction::triggered()}{triggered()} signal is connected to the
    \a receiver's \a member slot. The function adds the newly created
    action to the menu's list of actions and returns it.

    \sa QWidget::addAction()
*/
QAction *KexiMenuWidget::addAction(const QString &text, const QObject *receiver, const char* member, const QKeySequence &shortcut)
{
    QAction *action = new QAction(text, this);
#ifdef QT_NO_SHORTCUT
    Q_UNUSED(shortcut);
#else
    action->setShortcut(shortcut);
#endif
    QObject::connect(action, SIGNAL(triggered(bool)), receiver, member);
    addAction(action);
    return action;
}

/*!
    \overload

    This convenience function creates a new action with an \a icon and
    some \a text and an optional shortcut \a shortcut. The action's
    \l{QAction::triggered()}{triggered()} signal is connected to the
    \a member slot of the \a receiver object. The function adds the
    newly created action to the menu's list of actions, and returns it.

    \sa QWidget::addAction()
*/
QAction *KexiMenuWidget::addAction(const QIcon &icon, const QString &text, const QObject *receiver,
                          const char* member, const QKeySequence &shortcut)
{
    QAction *action = new QAction(icon, text, this);
#ifdef QT_NO_SHORTCUT
    Q_UNUSED(shortcut);
#else
    action->setShortcut(shortcut);
#endif
    QObject::connect(action, SIGNAL(triggered(bool)), receiver, member);
    addAction(action);
    return action;
}

/*!
    This convenience function creates a new separator action, i.e. an
    action with QAction::isSeparator() returning true, and adds the new
    action to this menu's list of actions. It returns the newly
    created action.

    \sa QWidget::addAction()
*/
QAction *KexiMenuWidget::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
    return action;
}

/*!
    This convenience function creates a new separator action, i.e. an
    action with QAction::isSeparator() returning true. The function inserts
    the newly created action into this menu's list of actions before
    action \a before and returns it.

    \sa QWidget::insertAction(), addSeparator()
*/
QAction *KexiMenuWidget::insertSeparator(QAction *before)
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    insertAction(before, action);
    return action;
}

/*!
  This sets the default action to \a act. The default action may have
  a visual cue, depending on the current QStyle. A default action
  usually indicates what will happen by default when a drop occurs.

  \sa defaultAction()
*/
void KexiMenuWidget::setDefaultAction(QAction *act)
{
    d->defaultAction = act;
}

/*!
  Returns the current default action.

  \sa setDefaultAction()
*/
QAction *KexiMenuWidget::defaultAction() const
{
    return d->defaultAction;
}

/*!
  Sets the currently highlighted action to \a act.
*/
void KexiMenuWidget::setActiveAction(QAction *act)
{
    d->setCurrentAction(act, 0);
    if (d->scroll)
        d->scrollMenu(act, KexiMenuWidgetPrivate::QMenuScroller::ScrollCenter);
}


/*!
    Returns the currently highlighted action, or 0 if no
    action is currently highlighted.
*/
QAction *KexiMenuWidget::activeAction() const
{
    return d->currentAction;
}

/*!
    \since 4.2

    Returns true if there are no visible actions inserted into the menu, false
    otherwise.

    \sa QWidget::actions()
*/

bool KexiMenuWidget::isEmpty() const
{
    bool ret = true;
    QList<QAction*> actionsList = this->actions();
    for(int i = 0; ret && i < actionsList.count(); ++i) {
        const QAction *action = actionsList.at(i);
        if (!action->isSeparator() && action->isVisible()) {
            ret = false;
        }
    }
    return ret;
}

/*!
    Removes all the menu's actions. Actions owned by the menu and not
    shown in any other widget are deleted.

    \sa removeAction()
*/
void KexiMenuWidget::clear()
{
    QList<QAction*> acts = actions();

    for(int i = 0; i < acts.size(); i++) {
#ifdef QT_SOFTKEYS_ENABLED
        // Lets not touch to our internal softkey actions
        if(acts[i] == d->selectAction || acts[i] == d->cancelAction)
            continue;
#endif
        removeAction(acts[i]);
        if (acts[i]->parent() == this && acts[i]->associatedWidgets().isEmpty())
            delete acts[i];
    }
}

/*!
  If a menu does not fit on the screen it lays itself out so that it
  does fit. It is style dependent what layout means (for example, on
  Windows it will use multiple columns).

  This functions returns the number of columns necessary.
*/
int KexiMenuWidget::columnCount() const
{
    return d->ncols;
}

/*!
  Returns the item at \a pt; returns 0 if there is no item there.
*/
QAction *KexiMenuWidget::actionAt(const QPoint &pt) const
{
    if (QAction *ret = d->actionAt(pt))
        return ret;
    return 0;
}

/*!
  Returns the geometry of action \a act.
*/
QRect KexiMenuWidget::actionGeometry(QAction *act) const
{
    return d->actionRect(act);
}

/*!
    \reimp
*/
QSize KexiMenuWidget::sizeHint() const
{
    d->updateActionRects();

    QSize s;
    for (int i = 0; i < d->actionRects.count(); ++i) {
        const QRect &rect = d->actionRects.at(i);
        if (rect.isNull())
            continue;
        if (rect.bottom() >= s.height())
            s.setHeight(rect.y() + rect.height());
        if (rect.right() >= s.width())
            s.setWidth(rect.x() + rect.width());
    }
    // Note that the action rects calculated above already include
    // the top and left margins, so we only need to add margins for
    // the bottom and right.
    QStyleOption opt(0);
    opt.init(this);
    const int fw = d->frameWidth(&opt);
    int leftmargin, topmargin, rightmargin, bottommargin;
    getContentsMargins(&leftmargin, &topmargin, &rightmargin, &bottommargin);
    s.rwidth() += style()->pixelMetric(QStyle::PM_MenuHMargin, &opt, this)
        + fw + rightmargin + 2 /*frame*/;
    s.rheight() += style()->pixelMetric(QStyle::PM_MenuVMargin, &opt, this) + fw + bottommargin;

    return style()->sizeFromContents(QStyle::CT_Menu, &opt,
                                    s.expandedTo(QApplication::globalStrut()), this);
}

/*!
    Displays the menu so that the action \a atAction will be at the
    specified \e global position \a p. To translate a widget's local
    coordinates into global coordinates, use QWidget::mapToGlobal().

    When positioning a menu with exec() or popup(), bear in mind that
    you cannot rely on the menu's current size(). For performance
    reasons, the menu adapts its size only when necessary, so in many
    cases, the size before and after the show is different. Instead,
    use sizeHint() which calculates the proper size depending on the
    menu's current contents.

    \sa QWidget::mapToGlobal(), exec()
*/
void KexiMenuWidget::popup(const QPoint &p, QAction *atAction)
{
#ifndef Q_OS_SYMBIAN
    if (d->scroll) { // reset scroll state from last popup
        d->scroll->scrollOffset = 0;
        d->scroll->scrollFlags = KexiMenuWidgetPrivate::QMenuScroller::ScrollNone;
    }
#endif
    //d->tearoffHighlighted = 0;
    d->motions = 0;
    //d->doChildEffects = true;
    d->updateLayoutDirection();

    ensurePolished(); // Get the right font
    emit aboutToShow();
    //const bool actionListChanged = d->itemsDirty;
    d->updateActionRects();
    QPoint pos;
    /*QPushButton *causedButton = qobject_cast<QPushButton*>(d->causedPopup().widget);
    if (actionListChanged && causedButton)
        pos = QPushButtonPrivate::get(causedButton)->adjustedMenuPosition();
    else*/
        pos = p;

    QSize size = sizeHint();
    QRect screen;
#ifndef QT_NO_GRAPHICSVIEW
    /*bool isEmbedded = !bypassGraphicsProxyWidget(this) && d->nearestGraphicsProxyWidget(this);
    if (isEmbedded)
        screen = d->popupGeometry(this);
    else*/
#endif
    screen = d->popupGeometry(QApplication::desktop()->screenNumber(p));

    const int desktopFrame = d->frameWidth();
    bool adjustToDesktop = !window()->testAttribute(Qt::WA_DontShowOnScreen);
    QList<QAction*> actionsList = this->actions();
#ifdef QT_KEYPAD_NAVIGATION
    if (!atAction && QApplication::keypadNavigationEnabled()) {
        // Try to have one item activated
        if (d->defaultAction && d->defaultAction->isEnabled()) {
            atAction = d->defaultAction;
            // TODO: This works for first level menus, not yet sub menus
        } else {
            foreach (QAction *action, actionsList)
                if (action->isEnabled()) {
                    atAction = action;
                    break;
                }
        }
        d->currentAction = atAction;
    }
#endif
    if (d->ncols > 1) {
        pos.setY(screen.top() + desktopFrame);
    } else if (atAction) {
        for (int i = 0, above_height = 0; i < actionsList.count(); i++) {
            QAction *action = actionsList.at(i);
            if (action == atAction) {
                int newY = pos.y() - above_height;
                if (d->scroll && newY < desktopFrame) {
                    d->scroll->scrollFlags = d->scroll->scrollFlags
                                             | KexiMenuWidgetPrivate::QMenuScroller::ScrollUp;
                    d->scroll->scrollOffset = newY;
                    newY = desktopFrame;
                }
                pos.setY(newY);

                if (d->scroll && d->scroll->scrollFlags != KexiMenuWidgetPrivate::QMenuScroller::ScrollNone
                    && !style()->styleHint(QStyle::SH_Menu_FillScreenWithScroll, 0, this)) {
                    int below_height = above_height + d->scroll->scrollOffset;
                    for (int i2 = i; i2 < d->actionRects.count(); i2++)
                        below_height += d->actionRects.at(i2).height();
                    size.setHeight(below_height);
                }
                break;
            } else {
                above_height += d->actionRects.at(i).height();
            }
        }
    }

    QPoint mouse = QCursor::pos();
    d->mousePopupPos = mouse;
    const bool snapToMouse = (QRect(p.x() - 3, p.y() - 3, 6, 6).contains(mouse));

    if (adjustToDesktop) {
        // handle popup falling "off screen"
        if (isRightToLeft()) {
            if (snapToMouse) // position flowing left from the mouse
                pos.setX(mouse.x() - size.width());

            if (pos.x() < screen.left() + desktopFrame)
                pos.setX(qMax(p.x(), screen.left() + desktopFrame));
            if (pos.x() + size.width() - 1 > screen.right() - desktopFrame)
                pos.setX(qMax(p.x() - size.width(), screen.right() - desktopFrame - size.width() + 1));
        } else {
            if (pos.x() + size.width() - 1 > screen.right() - desktopFrame)
                pos.setX(screen.right() - desktopFrame - size.width() + 1);
            if (pos.x() < screen.left() + desktopFrame)
                pos.setX(screen.left() + desktopFrame);
        }
        if (pos.y() + size.height() - 1 > screen.bottom() - desktopFrame) {
            if(snapToMouse)
                pos.setY(qMin(mouse.y() - (size.height() + desktopFrame), screen.bottom()-desktopFrame-size.height()+1));
            else
                pos.setY(qMax(p.y() - (size.height() + desktopFrame), screen.bottom()-desktopFrame-size.height()+1));
        } else if (pos.y() < screen.top() + desktopFrame) {
            pos.setY(screen.top() + desktopFrame);
        }

        if (pos.y() < screen.top() + desktopFrame)
            pos.setY(screen.top() + desktopFrame);
        if (pos.y() + size.height() - 1 > screen.bottom() - desktopFrame) {
            if (d->scroll) {
                d->scroll->scrollFlags |= uint(KexiMenuWidgetPrivate::QMenuScroller::ScrollDown);
                int y = qMax(screen.y(),pos.y());
                size.setHeight(screen.bottom() - (desktopFrame * 2) - y);
            } else {
                // Too big for screen, bias to see bottom of menu (for some reason)
                pos.setY(screen.bottom() - size.height() + 1);
            }
        }
    }
    //setGeometry(QRect(pos, size));
    //resize(size);
    show();

#ifndef QT_NO_ACCESSIBILITY
    //QAccessible::updateAccessibility(this, 0, QAccessible::PopupMenuStart);
#endif
}

#if 0
/*!
    Executes this menu synchronously.

    This is equivalent to \c{exec(pos())}.

    This returns the triggered QAction in either the popup menu or one
    of its submenus, or 0 if no item was triggered (normally because
    the user pressed Esc).

    In most situations you'll want to specify the position yourself,
    for example, the current mouse position:
    \snippet doc/src/snippets/code/src_gui_widgets_qmenu.cpp 0
    or aligned to a widget:
    \snippet doc/src/snippets/code/src_gui_widgets_qmenu.cpp 1
    or in reaction to a QMouseEvent *e:
    \snippet doc/src/snippets/code/src_gui_widgets_qmenu.cpp 2
*/
QAction *KexiMenuWidget::exec()
{
    return exec(pos());
}


/*!
    \overload

    Executes this menu synchronously.

    Pops up the menu so that the action \a action will be at the
    specified \e global position \a p. To translate a widget's local
    coordinates into global coordinates, use QWidget::mapToGlobal().

    This returns the triggered QAction in either the popup menu or one
    of its submenus, or 0 if no item was triggered (normally because
    the user pressed Esc).

    Note that all signals are emitted as usual. If you connect a
    QAction to a slot and call the menu's exec(), you get the result
    both via the signal-slot connection and in the return value of
    exec().

    Common usage is to position the menu at the current mouse
    position:
    \snippet doc/src/snippets/code/src_gui_widgets_qmenu.cpp 3
    or aligned to a widget:
    \snippet doc/src/snippets/code/src_gui_widgets_qmenu.cpp 4
    or in reaction to a QMouseEvent *e:
    \snippet doc/src/snippets/code/src_gui_widgets_qmenu.cpp 5

    When positioning a menu with exec() or popup(), bear in mind that
    you cannot rely on the menu's current size(). For performance
    reasons, the menu adapts its size only when necessary. So in many
    cases, the size before and after the show is different. Instead,
    use sizeHint() which calculates the proper size depending on the
    menu's current contents.

    \sa popup(), QWidget::mapToGlobal()
*/
QAction *KexiMenuWidget::exec(const QPoint &p, QAction *action)
{
    createWinId();
    QEventLoop eventLoop;
    d->eventLoop = &eventLoop;
    popup(p, action);

    QPointer<QObject> guard = this;
    (void) eventLoop.exec();
    if (guard.isNull())
        return 0;

    action = d->syncAction;
    d->syncAction = 0;
    d->eventLoop = 0;
    return action;
}

/*!
    \overload

    Executes a menu synchronously.

    The menu's actions are specified by the list of \a actions. The menu will
    pop up so that the specified action, \a at, appears at global position \a
    pos. If \a at is not specified then the menu appears at position \a
    pos. \a parent is the menu's parent widget; specifying the parent will
    provide context when \a pos alone is not enough to decide where the menu
    should go (e.g., with multiple desktops or when the parent is embedded in
    QGraphicsView).

    The function returns the triggered QAction in either the popup
    menu or one of its submenus, or 0 if no item was triggered
    (normally because the user pressed Esc).

    This is equivalent to:
    \snippet doc/src/snippets/code/src_gui_widgets_qmenu.cpp 6

    \sa popup(), QWidget::mapToGlobal()
*/
QAction *KexiMenuWidget::exec(QList<QAction*> actions, const QPoint &pos, QAction *at, QWidget *parent)
{
    KexiMenuWidget menu(parent);
    menu.addActions(actions);
    return menu.exec(pos, at);
}

/*!
    \overload

    Executes a menu synchronously.

    The menu's actions are specified by the list of \a actions. The menu
    will pop up so that the specified action, \a at, appears at global
    position \a pos. If \a at is not specified then the menu appears
    at position \a pos.

    The function returns the triggered QAction in either the popup
    menu or one of its submenus, or 0 if no item was triggered
    (normally because the user pressed Esc).

    This is equivalent to:
    \snippet doc/src/snippets/code/src_gui_widgets_qmenu.cpp 6

    \sa popup(), QWidget::mapToGlobal()
*/
QAction *KexiMenuWidget::exec(QList<QAction*> actions, const QPoint &pos, QAction *at)
{
    // ### Qt 5: merge
    return exec(actions, pos, at, 0);
}
#endif

/*!
  \reimp
*/
void KexiMenuWidget::hideEvent(QHideEvent *)
{
    emit aboutToHide();
    if (d->eventLoop)
        d->eventLoop->exit();
    d->setCurrentAction(0);
#ifndef QT_NO_ACCESSIBILITY
    QAccessible::updateAccessibility(this, 0, QAccessible::PopupMenuEnd);
#endif
    d->mouseDown = 0;
    d->hasHadMouse = false;
    d->causedPopup.widget = 0;
    d->causedPopup.action = 0;
    if (d->scroll)
        d->scroll->scrollTimer.stop(); //make sure the timer stops
    // this unmarks the previous persistent action
    d->setActionPersistentlySelected(0, false);
}

/*!
  \reimp
*/
void KexiMenuWidget::paintEvent(QPaintEvent *e)
{
    d->updateActionRects();
    QPainter p(this);
    QRegion emptyArea = QRegion(rect());

    if (d->oxygenHelper) {
        //d->oxygenHelper->renderWindowBackground(&p, rect(), this, palette());
        d->oxygenHelper->renderMenuBackground(&p, rect(), this, palette());
//        d->oxygenHelper->drawFloatFrame(&p, rect(), palette().window().color(), true);
    }

    QStyleOptionMenuItem menuOpt;
    menuOpt.initFrom(this);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
    menuOpt.tabWidth = 0;
    style()->drawPrimitive(QStyle::PE_PanelMenu, &menuOpt, &p, this);
    
    {
        QStyleOptionFrameV3 opt;
        opt.initFrom(this);
        opt.frameShape = QFrame::VLine;
        opt.state |= QStyle::State_Raised;
        opt.lineWidth = 1;
        opt.midLineWidth = 0;
        //opt.rect = QRect(opt.rect.topRight() - QPoint(0, 0), opt.rect.bottomRight() + QPoint(10, 0));
        //const QRect cr = style()->subElementRect(QStyle::SE_ShapedFrameContents, &opt);
        //opt.lineWidth = opt.rect.right() - cr.right();
        opt.rect.setX(opt.rect.width() - opt.lineWidth);
        opt.rect.setWidth(opt.lineWidth);
        style()->drawControl(QStyle::CE_ShapedFrame, &opt, &p);
        
        /*
        QStyleOption opt;
        opt.initFrom(this);
        opt.state |= QStyle::State_Horizontal;
        opt.rect = QRect(opt.rect.topRight() - QPoint(10, 0), opt.rect.bottomRight() + QPoint(10, 0));
        style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p, this);
        */
    }

    //draw the items that need updating..
    QList<QAction*> actionsList = this->actions();
    for (int i = 0; i < actionsList.count(); ++i) {
        QAction *action = actionsList.at(i);
        QRect adjustedActionRect = d->actionRects.at(i);
        if (!e->rect().intersects(adjustedActionRect)
            || d->widgetItems.value(action))
           continue;
        //set the clip region to be extra safe (and adjust for the scrollers)
        QRegion adjustedActionReg(adjustedActionRect);
        emptyArea -= adjustedActionReg;
        p.setClipRegion(adjustedActionReg);

        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        opt.rect = adjustedActionRect;
        if (d->actionPersistentlySelected(action)) {
            opt.state |= QStyle::State_Selected;
            opt.palette.setBrush(QPalette::Window, opt.palette.brush(QPalette::Highlight));
            opt.palette.setBrush(QPalette::WindowText, opt.palette.brush(QPalette::HighlightedText));
        }
        else if (!action->isSeparator() && (opt.state & QStyle::State_Selected)) {
            // lighten the highlight to make it different from
            // the persistently selected item
            opt.palette.setColor(QPalette::Highlight,
                                 KColorUtils::mix(
                                    opt.palette.color(QPalette::Highlight),
                                    opt.palette.color(QPalette::Window)));
            opt.palette.setColor(QPalette::HighlightedText, opt.palette.color(QPalette::Text));
        }
        
        // Depending on style Button or Background brush may be used
        // to fill background of deselected items. Make it transparent.
        bool transparentBackground = !(opt.state & QStyle::State_Selected);
        if (d->oxygenHelper && action->isSeparator()) {
            transparentBackground = false;
        }
        if (transparentBackground) {
            opt.palette.setBrush(QPalette::Button, QBrush(Qt::transparent));
            opt.palette.setBrush(QPalette::Background, QBrush(Qt::transparent));
        }

        style()->drawControl(QStyle::CE_MenuItem, &opt, &p, this);
    }

    const int fw = d->frameWidth();
    //draw the scroller regions..
    if (d->scroll) {
        menuOpt.menuItemType = QStyleOptionMenuItem::Scroller;
        menuOpt.state |= QStyle::State_Enabled;
        if (d->scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollUp) {
            menuOpt.rect.setRect(fw, fw, width() - (fw * 2), d->scrollerHeight());
            emptyArea -= QRegion(menuOpt.rect);
            p.setClipRect(menuOpt.rect);
            style()->drawControl(QStyle::CE_MenuScroller, &menuOpt, &p, this);
        }
        if (d->scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollDown) {
            menuOpt.rect.setRect(fw, height() - d->scrollerHeight() - fw, width() - (fw * 2),
                                     d->scrollerHeight());
            emptyArea -= QRegion(menuOpt.rect);
            menuOpt.state |= QStyle::State_DownArrow;
            p.setClipRect(menuOpt.rect);
            style()->drawControl(QStyle::CE_MenuScroller, &menuOpt, &p, this);
        }
    }
    //paint the tear off..
/*    if (d->tearoff) {
        menuOpt.menuItemType = QStyleOptionMenuItem::TearOff;
        menuOpt.rect.setRect(fw, fw, width() - (fw * 2),
                             style()->pixelMetric(QStyle::PM_MenuTearoffHeight, 0, this));
        if (d->scroll && d->scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollUp)
            menuOpt.rect.translate(0, d->scrollerHeight());
        emptyArea -= QRegion(menuOpt.rect);
        p.setClipRect(menuOpt.rect);
        menuOpt.state = QStyle::State_None;
        if (d->tearoffHighlighted)
            menuOpt.state |= QStyle::State_Selected;
        style()->drawControl(QStyle::CE_MenuTearoff, &menuOpt, &p, this);
    }*/
    //draw border
    if (fw > 0) {
        QRegion borderReg;
        borderReg += QRect(0, 0, fw, height()); //left
        borderReg += QRect(width()-fw, 0, fw, height()); //right
        borderReg += QRect(0, 0, width(), fw); //top
        borderReg += QRect(0, height()-fw, width(), fw); //bottom
        p.setClipRegion(borderReg);
        emptyArea -= borderReg;
        QStyleOptionFrame frame;
        frame.rect = rect();
        frame.palette = palette();
        frame.state = QStyle::State_None;
        frame.lineWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth);
        frame.midLineWidth = 0;
        style()->drawPrimitive(QStyle::PE_FrameMenu, &frame, &p, this);
        // full frame:
        style()->drawPrimitive(QStyle::PE_FrameWindow, &frame, &p, this);
    }

    //finally the rest of the space
    p.setClipRegion(emptyArea);
    menuOpt.state = QStyle::State_None;
    menuOpt.menuItemType = QStyleOptionMenuItem::EmptyArea;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.rect = rect();
    menuOpt.menuRect = rect();
    style()->drawControl(QStyle::CE_MenuEmptyArea, &menuOpt, &p, this);
}

#ifndef QT_NO_WHEELEVENT
/*!
  \reimp
*/
void KexiMenuWidget::wheelEvent(QWheelEvent *e)
{
    if (d->scroll && rect().contains(e->pos()))
        d->scrollMenu(e->delta() > 0 ?
                      KexiMenuWidgetPrivate::QMenuScroller::ScrollUp : KexiMenuWidgetPrivate::QMenuScroller::ScrollDown);
}
#endif

/*!
  \reimp
*/
void KexiMenuWidget::mousePressEvent(QMouseEvent *e)
{
    if (d->aboutToHide || d->mouseEventTaken(e))
        return;
    if (!rect().contains(e->pos())) {
         if (d->noReplayFor
             && QRect(d->noReplayFor->mapToGlobal(QPoint()), d->noReplayFor->size()).contains(e->globalPos()))
             setAttribute(Qt::WA_NoMouseReplay);
         if (d->eventLoop) // synchronous operation
             d->syncAction = 0;
        d->hideUpToMenuBar();
        return;
    }
    d->mouseDown = this;

    QAction *action = d->actionAt(e->pos());
    d->setCurrentAction(action, 20);
    d->toggleActionPersistentlySelected(action);
    update();
}

/*!
  \reimp
*/
void KexiMenuWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (d->aboutToHide || d->mouseEventTaken(e))
        return;
    if(d->mouseDown != this) {
        d->mouseDown = 0;
        return;
    }

    d->mouseDown = 0;
    d->setSyncAction();
    QAction *action = d->actionAt(e->pos());

    if (action && action == d->currentAction) {
        if (!action->menu()){
#if defined(Q_WS_WIN)
            //On Windows only context menus can be activated with the right button
            if (e->button() == Qt::LeftButton || d->topCausedWidget() == 0)
#endif
                d->activateAction(action, QAction::Trigger);
        }
    } else if (d->hasMouseMoved(e->globalPos())) {
        d->hideUpToMenuBar();
    }
}

/*!
  \reimp
*/
void KexiMenuWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::StyleChange || e->type() == QEvent::FontChange ||
        e->type() == QEvent::LayoutDirectionChange) {
        d->itemsDirty = 1;
        setMouseTracking(style()->styleHint(QStyle::SH_Menu_MouseTracking, 0, this));
        if (isVisible())
            resize(sizeHint());
        if (!style()->styleHint(QStyle::SH_Menu_Scrollable, 0, this)) {
            delete d->scroll;
            d->scroll = 0;
        } else if (!d->scroll) {
            d->scroll = new KexiMenuWidgetPrivate::QMenuScroller;
            d->scroll->scrollFlags = KexiMenuWidgetPrivate::QMenuScroller::ScrollNone;
        }
    } else if (e->type() == QEvent::EnabledChange) {
        d->menuAction->setEnabled(isEnabled());
    }
    QWidget::changeEvent(e);
}


/*!
  \reimp
*/
bool
KexiMenuWidget::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::Polish:
        d->updateLayoutDirection();
        break;
    case QEvent::ShortcutOverride: {
            QKeyEvent *kev = static_cast<QKeyEvent*>(e);
            if (kev->key() == Qt::Key_Up || kev->key() == Qt::Key_Down
                || kev->key() == Qt::Key_Left || kev->key() == Qt::Key_Right
                || kev->key() == Qt::Key_Enter || kev->key() == Qt::Key_Return
                || kev->key() == Qt::Key_Escape) {
                e->accept();
                return true;
            }
        }
        break;
    case QEvent::KeyPress: {
        QKeyEvent *ke = (QKeyEvent*)e;
        if (ke->key() == Qt::Key_Tab || ke->key() == Qt::Key_Backtab) {
            keyPressEvent(ke);
            return true;
        }
    } break;
    /*case QEvent::ContextMenu:
        if(d->menuDelayTimer.isActive()) {
            d->menuDelayTimer.stop();
            internalDelayedPopup();
        }
        break;*/
    case QEvent::Resize: {
        QStyleHintReturnMask menuMask;
        QStyleOption option;
        option.initFrom(this);
        if (style()->styleHint(QStyle::SH_Menu_Mask, &option, this, &menuMask)) {
            setMask(menuMask.region);
        }
        d->itemsDirty = 1;
        d->updateActionRects();
        break; }
    case QEvent::Show:
        d->mouseDown = 0;
        d->updateActionRects();
        show();
//         if (d->currentAction)
//             d->popupAction(d->currentAction, 0, false);
        break;
#ifndef QT_NO_WHATSTHIS
    case QEvent::QueryWhatsThis:
        e->setAccepted(whatsThis().size());
        if (QAction *action = d->actionAt(static_cast<QHelpEvent*>(e)->pos())) {
            if (action->whatsThis().size() || action->menu())
                e->accept();
        }
        return true;
#endif
#ifdef QT_SOFTKEYS_ENABLED
    case QEvent::LanguageChange: {
        d->selectAction->setText(QSoftKeyManager::standardSoftKeyText(QSoftKeyManager::SelectSoftKey));
        d->cancelAction->setText(QSoftKeyManager::standardSoftKeyText(QSoftKeyManager::CancelSoftKey));
        }
        break;
#endif
    default:
        break;
    }
    return QWidget::event(e);
}

/*!
    \reimp
*/
bool KexiMenuWidget::focusNextPrevChild(bool next)
{
    setFocus();
    QKeyEvent ev(QEvent::KeyPress, next ? Qt::Key_Tab : Qt::Key_Backtab, Qt::NoModifier);
    keyPressEvent(&ev);
    return true;
}

/*!
  \reimp
*/
void KexiMenuWidget::keyPressEvent(QKeyEvent *e)
{
    d->updateActionRects();
    int key = e->key();
    if (isRightToLeft()) {  // in reverse mode open/close key for submenues are reversed
        if (key == Qt::Key_Left)
            key = Qt::Key_Right;
        else if (key == Qt::Key_Right)
            key = Qt::Key_Left;
    }
#ifndef Q_WS_MAC
    if (key == Qt::Key_Tab) //means down
        key = Qt::Key_Down;
    if (key == Qt::Key_Backtab) //means up
        key = Qt::Key_Up;
#endif

    bool key_consumed = false;
    QList<QAction*> actionsList = this->actions();
    switch(key) {
    case Qt::Key_Home:
        key_consumed = true;
        if (d->scroll)
            d->scrollMenu(KexiMenuWidgetPrivate::QMenuScroller::ScrollTop, true);
        break;
    case Qt::Key_End:
        key_consumed = true;
        if (d->scroll)
            d->scrollMenu(KexiMenuWidgetPrivate::QMenuScroller::ScrollBottom, true);
        break;
    case Qt::Key_PageUp:
        key_consumed = true;
        if (d->currentAction && d->scroll) {
            if(d->scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollUp)
                d->scrollMenu(KexiMenuWidgetPrivate::QMenuScroller::ScrollUp, true, true);
            else
                d->scrollMenu(KexiMenuWidgetPrivate::QMenuScroller::ScrollTop, true);
        }
        break;
    case Qt::Key_PageDown:
        key_consumed = true;
        if (d->currentAction && d->scroll) {
            if(d->scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollDown)
                d->scrollMenu(KexiMenuWidgetPrivate::QMenuScroller::ScrollDown, true, true);
            else
                d->scrollMenu(KexiMenuWidgetPrivate::QMenuScroller::ScrollBottom, true);
        }
        break;
    case Qt::Key_Up:
    case Qt::Key_Down: {
        key_consumed = true;
        QAction *nextAction = 0;
        KexiMenuWidgetPrivate::QMenuScroller::ScrollLocation scroll_loc = KexiMenuWidgetPrivate::QMenuScroller::ScrollStay;
        if (!d->currentAction) {
            if(key == Qt::Key_Down) {
                for(int i = 0; i < actionsList.count(); ++i) {
                    QAction *act = actionsList.at(i);
                    if (d->actionRects.at(i).isNull())
                        continue;
                    if (!act->isSeparator() &&
                        (style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this)
                         || act->isEnabled())) {
                        nextAction = act;
                        break;
                    }
                }
            } else {
                for(int i = actionsList.count()-1; i >= 0; --i) {
                    QAction *act = actionsList.at(i);
                    if (d->actionRects.at(i).isNull())
                        continue;
                    if (!act->isSeparator() &&
                        (style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this)
                         || act->isEnabled())) {
                        nextAction = act;
                        break;
                    }
                }
            }
        } else {
            for(int i = 0, y = 0; !nextAction && i < actionsList.count(); i++) {
                QAction *act = actionsList.at(i);
                if (act == d->currentAction) {
                    if (key == Qt::Key_Up) {
                        for(int next_i = i-1; true; next_i--) {
                            if (next_i == -1) {
                                if(!style()->styleHint(QStyle::SH_Menu_SelectionWrap, 0, this))
                                    break;
                                if (d->scroll)
                                    scroll_loc = KexiMenuWidgetPrivate::QMenuScroller::ScrollBottom;
                                next_i = d->actionRects.count()-1;
                            }
                            QAction *next = actionsList.at(next_i);
                            if (next == d->currentAction)
                                break;
                            if (d->actionRects.at(next_i).isNull())
                                continue;
                            if (next->isSeparator() ||
                               (!next->isEnabled() &&
                                !style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this)))
                                continue;
                            nextAction = next;
                            if (d->scroll && (d->scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollUp)) {
                                int topVisible = d->scrollerHeight();
/*                                if (d->tearoff)
                                    topVisible += style()->pixelMetric(QStyle::PM_MenuTearoffHeight, 0, this);*/
                                if (((y + d->scroll->scrollOffset) - topVisible) <= d->actionRects.at(next_i).height())
                                    scroll_loc = KexiMenuWidgetPrivate::QMenuScroller::ScrollTop;
                            }
                            break;
                        }
/*                        if (!nextAction && d->tearoff)
                            d->tearoffHighlighted = 1;*/
                    } else {
                        y += d->actionRects.at(i).height();
                        for(int next_i = i+1; true; next_i++) {
                            if (next_i == d->actionRects.count()) {
                                if(!style()->styleHint(QStyle::SH_Menu_SelectionWrap, 0, this))
                                    break;
                                if (d->scroll)
                                    scroll_loc = KexiMenuWidgetPrivate::QMenuScroller::ScrollTop;
                                next_i = 0;
                            }
                            QAction *next = actionsList.at(next_i);
                            if (next == d->currentAction)
                                break;
                            if (d->actionRects.at(next_i).isNull())
                                continue;
                            if (next->isSeparator() ||
                               (!next->isEnabled() &&
                                !style()->styleHint(QStyle::SH_Menu_AllowActiveAndDisabled, 0, this)))
                                continue;
                            nextAction = next;
                            if (d->scroll && (d->scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollDown)) {
                                int bottomVisible = height() - d->scrollerHeight();
                                if (d->scroll->scrollFlags & KexiMenuWidgetPrivate::QMenuScroller::ScrollUp)
                                    bottomVisible -= d->scrollerHeight();
/*                                if (d->tearoff)
                                    bottomVisible -= style()->pixelMetric(QStyle::PM_MenuTearoffHeight, 0, this);*/
                                if ((y + d->scroll->scrollOffset + d->actionRects.at(next_i).height()) > bottomVisible)
                                    scroll_loc = KexiMenuWidgetPrivate::QMenuScroller::ScrollBottom;
                            }
                            break;
                        }
                    }
                    break;
                }
                y += d->actionRects.at(i).height();
            }
        }
        if (nextAction) {
            if (d->scroll && scroll_loc != KexiMenuWidgetPrivate::QMenuScroller::ScrollStay) {
                d->scroll->scrollTimer.stop();
                d->scrollMenu(nextAction, scroll_loc);
            }
            d->setCurrentAction(nextAction, /*popup*/-1, KexiMenuWidget::SelectedFromKeyboard);
        }
        break; }

    /*case Qt::Key_Right:
        if (d->currentAction && d->currentAction->isEnabled() && d->currentAction->menu()) {
            d->popupAction(d->currentAction, 0, true);
            key_consumed = true;
            break;
        }
        //FALL THROUGH*/
    case Qt::Key_Left: {
        if (d->currentAction && !d->scroll) {
            QAction *nextAction = 0;
            if (key == Qt::Key_Left) {
                QRect actionR = d->actionRect(d->currentAction);
                for(int x = actionR.left()-1; !nextAction && x >= 0; x--)
                    nextAction = d->actionAt(QPoint(x, actionR.center().y()));
            } else {
                QRect actionR = d->actionRect(d->currentAction);
                for(int x = actionR.right()+1; !nextAction && x < width(); x++)
                    nextAction = d->actionAt(QPoint(x, actionR.center().y()));
            }
            if (nextAction) {
                d->setCurrentAction(nextAction, /*popup*/-1, KexiMenuWidget::SelectedFromKeyboard);
                key_consumed = true;
            }
        }
        if (!key_consumed && key == Qt::Key_Left && qobject_cast<KexiMenuWidget*>(d->causedPopup.widget)) {
            QPointer<QWidget> caused = d->causedPopup.widget;
            d->hideMenu(this);
            if (caused)
                caused->setFocus();
            key_consumed = true;
        }
        break; }

    case Qt::Key_Alt:

        key_consumed = true;
        if (style()->styleHint(QStyle::SH_MenuBar_AltKeyNavigation, 0, this))
        {
            d->hideMenu(this);
        }
        break;

    case Qt::Key_Escape:
#ifdef QT_KEYPAD_NAVIGATION
    case Qt::Key_Back:
#endif
        key_consumed = true;
        {
            QPointer<QWidget> caused = d->causedPopup.widget;
            d->hideMenu(this); // hide after getting causedPopup
        }
        break;

    case Qt::Key_Space:
        if (!style()->styleHint(QStyle::SH_Menu_SpaceActivatesItem, 0, this))
            break;
        // for motif, fall through
#ifdef QT_KEYPAD_NAVIGATION
    case Qt::Key_Select:
#endif
    case Qt::Key_Return:
    case Qt::Key_Enter: {
        if (!d->currentAction) {
            d->setFirstActionActive();
            key_consumed = true;
            break;
        }

        d->setSyncAction();

        if (d->currentAction->menu())
            ; /*d->popupAction(d->currentAction, 0, true);*/
        else
            d->activateAction(d->currentAction, QAction::Trigger);
        key_consumed = true;
        break; }

#ifndef QT_NO_WHATSTHIS
    case Qt::Key_F1:
        if (!d->currentAction || d->currentAction->whatsThis().isNull())
            break;
        QWhatsThis::enterWhatsThisMode();
        d->activateAction(d->currentAction, QAction::Trigger);
        return;
#endif
    default:
        key_consumed = false;
    }

    if (!key_consumed) {                                // send to menu bar
        if ((!e->modifiers() || e->modifiers() == Qt::AltModifier || e->modifiers() == Qt::ShiftModifier) &&
           e->text().length()==1) {
            bool activateAction = false;
            QAction *nextAction = 0;
            if (style()->styleHint(QStyle::SH_Menu_KeyboardSearch, 0, this) && !e->modifiers()) {
                int best_match_count = 0;
                d->searchBufferTimer.start(2000, this);
                d->searchBuffer += e->text();
                for(int i = 0; i < actionsList.size(); ++i) {
                    int match_count = 0;
                    if (d->actionRects.at(i).isNull())
                        continue;
                    QAction *act = actionsList.at(i);
                    const QString act_text = act->text();
                    for(int c = 0; c < d->searchBuffer.size(); ++c) {
                        if(act_text.indexOf(d->searchBuffer.at(c), 0, Qt::CaseInsensitive) != -1)
                            ++match_count;
                    }
                    if(match_count > best_match_count) {
                        best_match_count = match_count;
                        nextAction = act;
                    }
                }
            }
#ifndef QT_NO_SHORTCUT
            else {
                int clashCount = 0;
                QAction *first = 0, *currentSelected = 0, *firstAfterCurrent = 0;
                QChar c = e->text().at(0).toUpper();
                for(int i = 0; i < actionsList.size(); ++i) {
                    if (d->actionRects.at(i).isNull())
                        continue;
                    QAction *act = actionsList.at(i);
                    QKeySequence sequence = QKeySequence::mnemonic(act->text());
                    int key = sequence[0] & 0xffff;
                    if (key == c.unicode()) {
                        clashCount++;
                        if (!first)
                            first = act;
                        if (act == d->currentAction)
                            currentSelected = act;
                        else if (!firstAfterCurrent && currentSelected)
                            firstAfterCurrent = act;
                    }
                }
                if (clashCount == 1)
                    activateAction = true;
                if (clashCount >= 1) {
                    if (clashCount == 1 || !currentSelected || !firstAfterCurrent)
                        nextAction = first;
                    else
                        nextAction = firstAfterCurrent;
                }
            }
#endif
            if (nextAction) {
                key_consumed = true;
                if(d->scroll)
                    d->scrollMenu(nextAction, KexiMenuWidgetPrivate::QMenuScroller::ScrollCenter, false);
                d->setCurrentAction(nextAction, 20, KexiMenuWidget::SelectedFromElsewhere, true);
                if (!nextAction->menu() && activateAction) {
                    d->setSyncAction();
                    d->activateAction(nextAction, QAction::Trigger);
                }
            }
        }
        if (!key_consumed) {
        }

#ifdef Q_OS_WIN32
        if (key_consumed && (e->key() == Qt::Key_Control || e->key() == Qt::Key_Shift || e->key() == Qt::Key_Meta))
            QApplication::beep();
#endif // Q_OS_WIN32
    }
    if (key_consumed)
        e->accept();
    else
        e->ignore();
}

/*!
  \reimp
*/
void KexiMenuWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!isVisible() || d->aboutToHide || d->mouseEventTaken(e))
        return;
    d->motions++;
    if (d->motions == 0) // ignore first mouse move event (see enterEvent())
        return;
    d->hasHadMouse = d->hasHadMouse || rect().contains(e->pos());

    QAction *action = d->actionAt(e->pos());
    if (!action) {
        if (d->hasHadMouse
            && (!d->currentAction
                || !(d->currentAction->menu() && d->currentAction->menu()->isVisible())))
            d->setCurrentAction(0);
        return;
    } else if(e->buttons()) {
        d->mouseDown = this;
    }
    if (d->sloppyRegion.contains(e->pos())) {
        d->sloppyAction = action;
        KexiMenuWidgetPrivate::sloppyDelayTimer = startTimer(style()->styleHint(QStyle::SH_Menu_SubMenuPopupDelay, 0, this)*6);
    } else {
        d->setCurrentAction(action, style()->styleHint(QStyle::SH_Menu_SubMenuPopupDelay, 0, this));
    }
}

/*!
  \reimp
*/
void KexiMenuWidget::enterEvent(QEvent *)
{
    d->motions = -1; // force us to ignore the generate mouse move in mouseMoveEvent()
}

/*!
  \reimp
*/
void KexiMenuWidget::leaveEvent(QEvent *)
{
    d->sloppyAction = 0;
    if (!d->sloppyRegion.isEmpty())
        d->sloppyRegion = QRegion();
    if (!d->activeMenu && d->currentAction)
        setActiveAction(0);
}

/*!
  \reimp
*/
void
KexiMenuWidget::timerEvent(QTimerEvent *e)
{
    if (d->scroll && d->scroll->scrollTimer.timerId() == e->timerId()) {
        d->scrollMenu((KexiMenuWidgetPrivate::QMenuScroller::ScrollDirection)d->scroll->scrollDirection);
        if (d->scroll->scrollFlags == KexiMenuWidgetPrivate::QMenuScroller::ScrollNone)
            d->scroll->scrollTimer.stop();
    }/* else if(d->menuDelayTimer.timerId() == e->timerId()) {
        d->menuDelayTimer.stop();
        internalDelayedPopup();
    }*/ else if(KexiMenuWidgetPrivate::sloppyDelayTimer == e->timerId()) {
        killTimer(KexiMenuWidgetPrivate::sloppyDelayTimer);
        KexiMenuWidgetPrivate::sloppyDelayTimer = 0;
        internalSetSloppyAction();
    } else if(d->searchBufferTimer.timerId() == e->timerId()) {
        d->searchBuffer.clear();
    }
}

/*!
  \reimp
*/
void KexiMenuWidget::actionEvent(QActionEvent *e)
{
    d->itemsDirty = 1;
    //setAttribute(Qt::WA_Resized, false);
    if (e->type() == QEvent::ActionAdded) {
        connect(e->action(), SIGNAL(triggered()), this, SLOT(actionTriggered()));
        connect(e->action(), SIGNAL(hovered()), this, SLOT(actionHovered()));
        if (QWidgetAction *wa = qobject_cast<QWidgetAction *>(e->action())) {
            QWidget *widget = wa->requestWidget(this);
            if (widget)
                d->widgetItems.insert(wa, widget);
        }
    } else if (e->type() == QEvent::ActionRemoved) {
        e->action()->disconnect(this);
        if (e->action() == d->currentAction)
            d->currentAction = 0;
        if (QWidgetAction *wa = qobject_cast<QWidgetAction *>(e->action())) {
            if (QWidget *widget = d->widgetItems.value(wa))
                wa->releaseWidget(widget);
        }
        d->widgetItems.remove(e->action());
    }

    if (isVisible()) {
        d->updateActionRects();
        resize(sizeHint());
        update();
    }
}

/*!
  \internal
*/
void KexiMenuWidget::internalSetSloppyAction()
{
    if (d->sloppyAction)
        d->setCurrentAction(d->sloppyAction, 0);
}

#if 0
/*!
  \internal
*/
void KexiMenuWidget::internalDelayedPopup()
{
    //hide the current item
    if (KexiMenuWidget *menu = d->activeMenu) {
        d->activeMenu = 0;
        d->hideMenu(menu);
    }

    if (!d->currentAction || !d->currentAction->isEnabled() || !d->currentAction->menu() ||
        !d->currentAction->menu()->isEnabled() || d->currentAction->menu()->isVisible())
        return;

    //setup
    d->activeMenu = d->currentAction->menu();
    d->activeMenu->causedPopup().widget = this;
    d->activeMenu->causedPopup().action = d->currentAction;

    int subMenuOffset = style()->pixelMetric(QStyle::PM_SubMenuOverlap, 0, this);
    const QRect actionRect(d->actionRect(d->currentAction));
    const QSize menuSize(d->activeMenu->sizeHint());
    const QPoint rightPos(mapToGlobal(QPoint(actionRect.right() + subMenuOffset + 1, actionRect.top())));
    const QPoint leftPos(mapToGlobal(QPoint(actionRect.left() - subMenuOffset - menuSize.width(), actionRect.top())));

    QPoint pos(rightPos);
    KexiMenuWidget *caused = qobject_cast<KexiMenuWidget*>(d->activeMenu->causedPopup().widget);

    const QRect availGeometry(d->popupGeometry(caused));
    if (isRightToLeft()) {
        pos = leftPos;
        if ((caused && caused->x() < x()) || pos.x() < availGeometry.left()) {
            if(rightPos.x() + menuSize.width() < availGeometry.right())
                pos = rightPos;
            else
                pos.rx() = availGeometry.left();
        }
    } else {
        if ((caused && caused->x() > x()) || pos.x() + menuSize.width() > availGeometry.right()) {
            if(leftPos.x() < availGeometry.left())
                pos.rx() = availGeometry.right() - menuSize.width();
            else
                pos = leftPos;
        }
    }

    //calc sloppy focus buffer
    if (style()->styleHint(QStyle::SH_Menu_SloppySubMenus, 0, this)) {
        QPoint cur = QCursor::pos();
        if (actionRect.contains(mapFromGlobal(cur))) {
            QPoint pts[4];
            pts[0] = QPoint(cur.x(), cur.y() - 2);
            pts[3] = QPoint(cur.x(), cur.y() + 2);
            if (pos.x() >= cur.x())        {
                pts[1] = QPoint(geometry().right(), pos.y());
                pts[2] = QPoint(geometry().right(), pos.y() + menuSize.height());
            } else {
                pts[1] = QPoint(pos.x() + menuSize.width(), pos.y());
                pts[2] = QPoint(pos.x() + menuSize.width(), pos.y() + menuSize.height());
            }
            QPolygon points(4);
            for(int i = 0; i < 4; i++)
                points.setPoint(i, mapFromGlobal(pts[i]));
            d->sloppyRegion = QRegion(points);
        }
    }

    //do the popup
    d->activeMenu->popup(pos);
}
#endif

/*!\internal
*/
void KexiMenuWidget::setNoReplayFor(QWidget *noReplayFor)
{
#ifdef Q_WS_WIN
    d->noReplayFor = noReplayFor;
#else
    Q_UNUSED(noReplayFor);
#endif
}

/*!
  \property KexiMenuWidget::separatorsCollapsible
  \since 4.2

  \brief whether consecutive separators should be collapsed

  This property specifies whether consecutive separators in the menu
  should be visually collapsed to a single one. Separators at the
  beginning or the end of the menu are also hidden.

  By default, this property is true.
*/
bool KexiMenuWidget::separatorsCollapsible() const
{
    return d->collapsibleSeparators;
}

void KexiMenuWidget::setSeparatorsCollapsible(bool collapse)
{
    if (d->collapsibleSeparators == collapse)
        return;

    d->collapsibleSeparators = collapse;
    d->itemsDirty = 1;
    if (isVisible()) {
        d->updateActionRects();
        update();
    }
}

#include "KexiMenuWidget.moc"
