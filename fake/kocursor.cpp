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

#include "kocursor.h"
#include "kocursor_p.h"

#include <QCursor>
#include <QEvent>
#include <QAbstractScrollArea>
#include <QTimer>
#include <QWidget>
#include <QFile>

void KoCursor::setAutoHideCursor( QWidget *w, bool enable,
                                 bool customEventFilter )
{
    KoCursorPrivate::self()->setAutoHideCursor( w, enable, customEventFilter );
}

void KoCursor::autoHideEventFilter( QObject *o, QEvent *e )
{
    KoCursorPrivate::self()->eventFilter( o, e );
}

void KoCursor::setHideCursorDelay( int ms )
{
    KoCursorPrivate::self()->hideCursorDelay = ms;
}

int KoCursor::hideCursorDelay()
{
    return KoCursorPrivate::self()->hideCursorDelay;
}

// **************************************************************************

KoCursorPrivateAutoHideEventFilter::KoCursorPrivateAutoHideEventFilter( QWidget* widget )
    : m_widget( widget )
    , m_wasMouseTracking( m_widget->hasMouseTracking() )
    , m_isCursorHidden( false )
    , m_isOwnCursor( false )
{
    mouseWidget()->setMouseTracking( true );
    connect( &m_autoHideTimer, SIGNAL(timeout()),
             this, SLOT(hideCursor()) );
}

KoCursorPrivateAutoHideEventFilter::~KoCursorPrivateAutoHideEventFilter()
{
    if( m_widget != NULL )
        mouseWidget()->setMouseTracking( m_wasMouseTracking );
}

void KoCursorPrivateAutoHideEventFilter::resetWidget()
{
    m_widget = NULL;
}

void KoCursorPrivateAutoHideEventFilter::hideCursor()
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

void KoCursorPrivateAutoHideEventFilter::unhideCursor()
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
QWidget* KoCursorPrivateAutoHideEventFilter::mouseWidget() const
{
    QWidget* w = m_widget;

    // Is w a QAbstractScrollArea ? Call setCursor on the viewport in that case.
    QAbstractScrollArea * sv = qobject_cast<QAbstractScrollArea *>( w );
    if ( sv )
        w = sv->viewport();

    return w;
}

bool KoCursorPrivateAutoHideEventFilter::eventFilter( QObject *o, QEvent *e )
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
            m_autoHideTimer.start( KoCursorPrivate::self()->hideCursorDelay );
        }
        break;
    default:
        break;
    }

    return false;
}

KoCursorPrivate * KoCursorPrivate::s_self = 0L;

KoCursorPrivate * KoCursorPrivate::self()
{
    if ( !s_self )
        s_self = new KoCursorPrivate;
    // WABA: Don't delete KoCursorPrivate, it serves no real purpose.
    // Even worse it causes crashes because it seems to get deleted
    // during ~QApplication and ~QApplication doesn't seem to like it
    // when we delete a QCursor. No idea if that is a bug itself.

    return s_self;
}

KoCursorPrivate::KoCursorPrivate()
{
    hideCursorDelay = 5000; // 5s default value
    enabled = false;
}

KoCursorPrivate::~KoCursorPrivate()
{
}

void KoCursorPrivate::setAutoHideCursor( QWidget *w, bool enable, bool customEventFilter )
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
        KoCursorPrivateAutoHideEventFilter* filter = new KoCursorPrivateAutoHideEventFilter( w );
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
        KoCursorPrivateAutoHideEventFilter* filter = m_eventFilters.take( w );
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

bool KoCursorPrivate::eventFilter( QObject *o, QEvent *e )
{
    if ( !enabled )
        return false;

    KoCursorPrivateAutoHideEventFilter* filter = m_eventFilters.value( o );

    Q_ASSERT( filter != 0 );
    if ( filter == 0 )
        return false;

    return filter->eventFilter( o, e );
}

void KoCursorPrivate::slotViewportDestroyed(QObject *o)
{
    m_eventFilters.remove(o);
}

void KoCursorPrivate::slotWidgetDestroyed( QObject* o )
{
    KoCursorPrivateAutoHideEventFilter* filter = m_eventFilters.take( o );

    Q_ASSERT( filter != 0 );

    filter->resetWidget(); // so that dtor doesn't access it
    delete filter;
}

#include "moc_kocursor_p.cpp"
