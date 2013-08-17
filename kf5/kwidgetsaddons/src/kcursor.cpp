/* This file is part of the KDE libraries
   Copyright (C) 1998 Kurt Granroth (granroth@kde.org)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kcursor.h"
#include "kcursor_p.h"

#include <QCursor>
#include <QEvent>
#include <QAbstractScrollArea>
#include <QTimer>
#include <QWidget>
#include <QFile>

void KCursor::setAutoHideCursor( QWidget *w, bool enable,
                                 bool customEventFilter )
{
    KCursorPrivate::self()->setAutoHideCursor( w, enable, customEventFilter );
}

void KCursor::autoHideEventFilter( QObject *o, QEvent *e )
{
    KCursorPrivate::self()->eventFilter( o, e );
}

void KCursor::setHideCursorDelay( int ms )
{
    KCursorPrivate::self()->hideCursorDelay = ms;
}

int KCursor::hideCursorDelay()
{
    return KCursorPrivate::self()->hideCursorDelay;
}

// **************************************************************************

KCursorPrivateAutoHideEventFilter::KCursorPrivateAutoHideEventFilter( QWidget* widget )
    : m_widget( widget )
    , m_wasMouseTracking( m_widget->hasMouseTracking() )
    , m_isCursorHidden( false )
    , m_isOwnCursor( false )
{
    mouseWidget()->setMouseTracking( true );
    connect( &m_autoHideTimer, SIGNAL(timeout()),
             this, SLOT(hideCursor()) );
}

KCursorPrivateAutoHideEventFilter::~KCursorPrivateAutoHideEventFilter()
{
    if( m_widget != NULL )
        mouseWidget()->setMouseTracking( m_wasMouseTracking );
}

void KCursorPrivateAutoHideEventFilter::resetWidget()
{
    m_widget = NULL;
}

void KCursorPrivateAutoHideEventFilter::hideCursor()
{
    m_autoHideTimer.stop();

    if ( m_isCursorHidden )
        return;

    m_isCursorHidden = true;

    QWidget* w = mouseWidget();

    m_isOwnCursor = w->testAttribute(Qt::WA_SetCursor);
    if ( m_isOwnCursor )
        m_oldCursor = w->cursor();

    w->setCursor( QCursor( Qt::BlankCursor ) );
}

void KCursorPrivateAutoHideEventFilter::unhideCursor()
{
    m_autoHideTimer.stop();

    if ( !m_isCursorHidden )
        return;

    m_isCursorHidden = false;

    QWidget* w = mouseWidget();

    if ( w->cursor().shape() != Qt::BlankCursor ) // someone messed with the cursor already
	return;

    if ( m_isOwnCursor )
        w->setCursor( m_oldCursor );
    else
        w->unsetCursor();
}

// The widget which gets mouse events, and that shows the cursor
// (that is the viewport, for a QAbstractScrollArea)
QWidget* KCursorPrivateAutoHideEventFilter::mouseWidget() const
{
    QWidget* w = m_widget;

    // Is w a QAbstractScrollArea ? Call setCursor on the viewport in that case.
    QAbstractScrollArea * sv = qobject_cast<QAbstractScrollArea *>( w );
    if ( sv )
        w = sv->viewport();

    return w;
}

bool KCursorPrivateAutoHideEventFilter::eventFilter( QObject *o, QEvent *e )
{
    Q_UNUSED(o);
    // o is m_widget or its viewport
    //Q_ASSERT( o == m_widget );

    switch ( e->type() )
    {
    case QEvent::Leave:
    case QEvent::FocusOut:
    case QEvent::WindowDeactivate:
        unhideCursor();
        break;
    case QEvent::KeyPress:
    case QEvent::ShortcutOverride:
        hideCursor();
        break;
    case QEvent::Enter:
    case QEvent::FocusIn:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::Show:
    case QEvent::Hide:
    case QEvent::Wheel:
        unhideCursor();
        if ( m_widget->hasFocus() )
        {
            m_autoHideTimer.setSingleShot( true );
            m_autoHideTimer.start( KCursorPrivate::self()->hideCursorDelay );
        }
        break;
    default:
        break;
    }

    return false;
}

KCursorPrivate * KCursorPrivate::s_self = 0L;

KCursorPrivate * KCursorPrivate::self()
{
    if ( !s_self )
        s_self = new KCursorPrivate;
    // WABA: Don't delete KCursorPrivate, it serves no real purpose.
    // Even worse it causes crashes because it seems to get deleted
    // during ~QApplication and ~QApplication doesn't seem to like it
    // when we delete a QCursor. No idea if that is a bug itself.

    return s_self;
}

KCursorPrivate::KCursorPrivate()
{
    hideCursorDelay = 5000; // 5s default value
}

KCursorPrivate::~KCursorPrivate()
{
}

void KCursorPrivate::setAutoHideCursor( QWidget *w, bool enable, bool customEventFilter )
{
    if ( !w || !enabled )
        return;

    QWidget* viewport = 0;
    QAbstractScrollArea * sv = qobject_cast<QAbstractScrollArea *>( w );
    if ( sv )
        viewport = sv->viewport();

    if ( enable )
    {
        if ( m_eventFilters.contains( w ) )
            return;
        KCursorPrivateAutoHideEventFilter* filter = new KCursorPrivateAutoHideEventFilter( w );
        m_eventFilters.insert( w, filter );
        if (viewport) {
            m_eventFilters.insert( viewport, filter );
            connect(viewport, SIGNAL(destroyed(QObject*)), this, SLOT(slotViewportDestroyed(QObject*)));
        }
        if ( !customEventFilter ) {
            w->installEventFilter( filter ); // for key events
            if (viewport)
                viewport->installEventFilter( filter ); // for mouse events
        }
        connect( w, SIGNAL(destroyed(QObject*)),
                 this, SLOT(slotWidgetDestroyed(QObject*)) );
    }
    else
    {
        KCursorPrivateAutoHideEventFilter* filter = m_eventFilters.take( w );
        if ( filter == 0 )
            return;
        w->removeEventFilter( filter );
        if (viewport) {
            m_eventFilters.remove( viewport );
            disconnect(viewport, SIGNAL(destroyed(QObject*)), this, SLOT(slotViewportDestroyed(QObject*)));
            viewport->removeEventFilter( filter );
        }
        delete filter;
        disconnect( w, SIGNAL(destroyed(QObject*)),
                    this, SLOT(slotWidgetDestroyed(QObject*)) );
    }
}

bool KCursorPrivate::eventFilter( QObject *o, QEvent *e )
{
    if ( !enabled )
        return false;

    KCursorPrivateAutoHideEventFilter* filter = m_eventFilters.value( o );

    Q_ASSERT( filter != 0 );
    if ( filter == 0 )
        return false;

    return filter->eventFilter( o, e );
}

void KCursorPrivate::slotViewportDestroyed(QObject *o)
{
    m_eventFilters.remove(o);
}

void KCursorPrivate::slotWidgetDestroyed( QObject* o )
{
    KCursorPrivateAutoHideEventFilter* filter = m_eventFilters.take( o );

    Q_ASSERT( filter != 0 );

    filter->resetWidget(); // so that dtor doesn't access it
    delete filter;
}

#include "moc_kcursor_p.cpp"
