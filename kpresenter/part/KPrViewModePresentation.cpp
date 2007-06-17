/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
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

#include <KoPACanvas.h>
#include <KoPADocument.h>
#include <KoPADocument.h>
#include <KoPAView.h>

KPrViewModePresentation::KPrViewModePresentation( KoPAView * view, KoPACanvas * canvas )
: KoPAViewMode( view, canvas )
, m_savedParent( 0 )
, m_animationDirector( 0 )    
{
}

KPrViewModePresentation::~KPrViewModePresentation()
{
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
}

void KPrViewModePresentation::mouseDoubleClickEvent( QMouseEvent *event, const QPointF &point )
{
}

void KPrViewModePresentation::mouseMoveEvent( QMouseEvent *event, const QPointF &point )
{
}

void KPrViewModePresentation::mouseReleaseEvent( QMouseEvent *event, const QPointF &point )
{
}

void KPrViewModePresentation::keyPressEvent( QKeyEvent *event )
{
    event->accept();

    // move to a presentation tool
    switch ( event->key() )
    {
        case Qt::Key_Escape:
            m_view->setViewMode( m_savedViewMode );
            break;
        case Qt::Key_Home:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
        case Qt::Key_End:
        case Qt::Key_Space:
            if ( m_animationDirector )
            {
                m_animationDirector->navigate();
            }
            break;
        default:    
            event->ignore();
            break;
    }
}

void KPrViewModePresentation::keyReleaseEvent( QKeyEvent *event )
{
}

void KPrViewModePresentation::wheelEvent( QWheelEvent * event, const QPointF &point )
{
}

void KPrViewModePresentation::activate( KoPAViewMode * previousViewMode )
{
    m_savedViewMode = previousViewMode;
    m_savedParent = m_canvas->parentWidget();
    m_canvas->setParent( ( QWidget* )0, Qt::Window );
    m_canvas->showFullScreen();
    m_canvas->setFocus();

    m_animationDirector = new KPrAnimationDirector( m_view, m_view->kopaDocument()->pages() );
}

void KPrViewModePresentation::deactivate()
{
    m_canvas->setParent( m_savedParent, Qt::Widget );
    m_canvas->setFocus();
    m_canvas->showNormal();
    delete m_animationDirector;
    m_animationDirector = 0;
}
