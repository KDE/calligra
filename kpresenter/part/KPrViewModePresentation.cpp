/* This file is part of the KDE project
 * Copyright (  C ) 2007 Thorsten Zachmann <zachmann@kde.org>
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

#include <KoToolProxy.h>
#include <KoShapeManager.h>
#include "KoPACanvas.h"
#include "KoPADocument.h"
#include "KoPAPage.h"
#include "KoPAMasterPage.h"
#include "KoPAView.h"

KPrViewModePresentation::KPrViewModePresentation( KoPAView * view, KoPACanvas * canvas )
: KoPAViewMode( view, canvas )
{
}

KPrViewModePresentation::~KPrViewModePresentation()
{
}

void KPrViewModePresentation::paintEvent( QPaintEvent* event )
{
    QPainter painter( m_canvas );
    painter.translate( -m_canvas->documentOffset() );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setClipRect( event->rect().translated( m_canvas->documentOffset() ) );

    m_canvas->masterShapeManager()->paint( painter, *( m_view->viewConverter() ), false );
    m_canvas->shapeManager()->paint( painter, *( m_view->viewConverter() ), false );
    m_toolProxy->paint( painter, *( m_view->viewConverter() ) );
}

void KPrViewModePresentation::tabletEvent( QTabletEvent *event, const QPointF &point )
{
    m_toolProxy->tabletEvent( event, point );
}

void KPrViewModePresentation::mousePressEvent( QMouseEvent *event, const QPointF &point )
{
    m_toolProxy->mousePressEvent( event, point );
}

void KPrViewModePresentation::mouseDoubleClickEvent( QMouseEvent *event, const QPointF &point )
{
    m_toolProxy->mouseDoubleClickEvent( event, point );
}

void KPrViewModePresentation::mouseMoveEvent( QMouseEvent *event, const QPointF &point )
{
    m_toolProxy->mouseMoveEvent( event, point );
}

void KPrViewModePresentation::mouseReleaseEvent( QMouseEvent *event, const QPointF &point )
{
    m_toolProxy->mouseReleaseEvent( event, point );
}

void KPrViewModePresentation::keyPressEvent( QKeyEvent *event )
{
    m_toolProxy->keyPressEvent( event );

    if ( ! event->isAccepted() ) {
        event->accept();

        switch ( event->key() )
        {
            case Qt::Key_Escape:
                // TODO check if this is what I want or maybe use a single shot timer
                m_view->setViewMode( m_savedViewMode );
                break;
            case Qt::Key_Home:
                m_view->navigatePage( KoPageApp::PageFirst );
                break;
            case Qt::Key_PageUp:
                m_view->navigatePage( KoPageApp::PagePrevious );
                break;
            case Qt::Key_PageDown:
                m_view->navigatePage( KoPageApp::PageNext );
                break;
            case Qt::Key_End:
                m_view->navigatePage( KoPageApp::PageLast );
                break;
            default:    
                event->ignore();
                break;
        }
    }
    m_canvas->repaint();
}

void KPrViewModePresentation::keyReleaseEvent( QKeyEvent *event )
{
    m_toolProxy->keyReleaseEvent( event );
}

void KPrViewModePresentation::wheelEvent( QWheelEvent * event, const QPointF &point )
{
    m_toolProxy->wheelEvent( event, point );
}

void KPrViewModePresentation::activate( KoPAViewMode * previousViewMode )
{
    m_savedViewMode = previousViewMode;
    m_view->setCanvasMode( true );
}

void KPrViewModePresentation::deactivate()
{
    m_canvas->setFocus();
    m_view->setCanvasMode( false );
}
