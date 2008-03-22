/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPrViewModePresentation.h"

#include <QEvent>
#include <QKeyEvent>
#include <QPainter>

#include <kdebug.h>

#include <KoPointerEvent.h>
#include <KoCanvasController.h>
#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPAView.h>
#include "KPrPresentationTool.h"

KPrViewModePresentation::KPrViewModePresentation( KoPAView * view, KoPACanvas * canvas )
: KoPAViewMode( view, canvas )
, m_savedParent( 0 )
, m_tool( new KPrPresentationTool( *this ) )
, m_animationDirector( 0 )
{
}

KPrViewModePresentation::~KPrViewModePresentation()
{
    delete m_animationDirector;
    delete m_tool;
}

KoViewConverter * KPrViewModePresentation::viewConverter()
{
    if(!m_animationDirector)
        return 0;

    return m_animationDirector->viewConverter();
}

void KPrViewModePresentation::paintEvent( QPaintEvent* event )
{
    if ( m_animationDirector )
    {
        m_animationDirector->paintEvent( event );
    }
}

void KPrViewModePresentation::tabletEvent( QTabletEvent *event, const QPointF &point )
{

}

void KPrViewModePresentation::mousePressEvent( QMouseEvent *event, const QPointF &point )
{
    KoPointerEvent ev( event, point );
    m_tool->mousePressEvent( &ev );
}

void KPrViewModePresentation::mouseDoubleClickEvent( QMouseEvent *event, const QPointF &point )
{
    KoPointerEvent ev( event, point );
    m_tool->mouseDoubleClickEvent( &ev );
}

void KPrViewModePresentation::mouseMoveEvent( QMouseEvent *event, const QPointF &point )
{
    KoPointerEvent ev( event, point );
    m_tool->mouseMoveEvent( &ev );
}

void KPrViewModePresentation::mouseReleaseEvent( QMouseEvent *event, const QPointF &point )
{
    KoPointerEvent ev( event, point );
    m_tool->mouseReleaseEvent( &ev );
}

void KPrViewModePresentation::keyPressEvent( QKeyEvent *event )
{
    m_tool->keyPressEvent( event );
}

void KPrViewModePresentation::keyReleaseEvent( QKeyEvent *event )
{
    m_tool->keyReleaseEvent( event );
}

void KPrViewModePresentation::wheelEvent( QWheelEvent * event, const QPointF &point )
{
    KoPointerEvent ev( event, point );
    m_tool->wheelEvent( &ev );
}

void KPrViewModePresentation::activate( KoPAViewMode * previousViewMode )
{
    m_savedViewMode = previousViewMode;               // store the previous view mode
    m_savedParent = m_canvas->parentWidget();
    m_canvas->setParent( ( QWidget* )0, Qt::Window ); // set parent to 0 and
    m_canvas->setWindowState( m_canvas->windowState() | Qt::WindowFullScreen ); // detach widget to make
    m_canvas->show();
    m_canvas->setFocus();                             // it shown full screen

    m_animationDirector = new KPrAnimationDirector( m_view, m_view->kopaDocument()->pages(), m_view->activePage() );
}

void KPrViewModePresentation::deactivate()
{
    m_canvas->setParent( m_savedParent, Qt::Widget );
    m_canvas->setFocus();
    m_canvas->setWindowState( m_canvas->windowState() & ~Qt::WindowFullScreen ); // reset
    m_canvas->show();
    m_view->updateActivePage(m_view->activePage());
    delete m_animationDirector;
    m_animationDirector = 0;
}

void KPrViewModePresentation::activateSavedViewMode()
{
    m_view->setViewMode( m_savedViewMode );
}

KPrAnimationDirector * KPrViewModePresentation::animationDirector()
{
    return m_animationDirector;
}

void KPrViewModePresentation::navigate( KPrAnimationDirector::Navigation navigation )
{
    m_animationDirector->navigate( navigation );
}
