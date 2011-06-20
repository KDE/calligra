/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2011 Mani Chandrasekar<maninc@gmail.com>
 *
 * Contact: Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QPen>
#include <QDBusConnection>

#include <KoView.h>

#include "ApplicationController.h"
#include "PresentationTool.h"

PresentationTool::PresentationTool(MainWindow * window, KoCanvasControllerWidget * controller )
    : m_controller(controller),
    m_window(window),
    m_penToolActivated(false),
    m_highlightToolActivated(false),
    scribbling(false),
    m_dbus( new PresentationToolAdaptor( this ) )
{
    QDBusConnection::sessionBus().registerObject("/presentation/tool", this);
    lastPoint.setX(0);
    lastPoint.setY(0);
}

PresentationTool::~PresentationTool()
{
}

void PresentationTool::handleMainWindowMousePressEvent( QMouseEvent * event )
{
    if( m_penToolActivated ) {
        if ( event->button() == Qt::LeftButton ) {
            emit startDrawPresentation(event->x(), event->y());
            lastPoint = event->pos();
            scribbling = true;
        }
    }
}

void PresentationTool::handleMainWindowMouseMoveEvent( QMouseEvent * event )
{
    if( m_penToolActivated ) {
        if ( ( event->buttons() & Qt::LeftButton ) && scribbling ) {
            emit drawOnPresentation(event->x(), event->y());
            drawLineTo( event->pos() );
        }
    }

    if( m_highlightToolActivated ) {
        if ( ( event->buttons() & Qt::LeftButton ) ) {
            emit highlightPresentation(event->x(), event->y());
            drawEllipse( event->pos() );
        }
    }
}

void PresentationTool::handleMainWindowMouseReleaseEvent( QMouseEvent * event )
{
    if( m_penToolActivated ) {
        if ( event->button() == Qt::LeftButton && scribbling ) {
            emit stopDrawPresentation();
            drawLineTo( event->pos() );
            scribbling = false;
        }
    }
}

void PresentationTool::togglePenTool()
{
    if( m_penToolActivated == false ) {
        m_penToolActivated = true;
        m_highlightToolActivated = false;
        QPixmap pix = QPixmap::grabWidget( m_controller );
        image = new QImage( pix.toImage() );
        m_controller->hide();
        static_cast<ApplicationController*>(m_window->controller())
                ->setFullScreenPresentationNavigationEnabled(false);
    }
    else {
        emit normalPresentation();
        static_cast<ApplicationController*>(m_window->controller())
                ->setFullScreenPresentationNavigationEnabled(true);
        m_penToolActivated = false;
        m_controller->show();
    }
}

void PresentationTool::toggleHighlightTool()
{
    if( m_highlightToolActivated == false ) {
        m_highlightToolActivated = true;
        m_penToolActivated = false;
        QPixmap pix= QPixmap::grabWidget( m_controller );
        image = new QImage( pix.toImage() );
        image1= new QImage( *image );
        m_controller->hide();
        static_cast<ApplicationController*>(m_window->controller())
                ->setFullScreenPresentationNavigationEnabled(false);
    }
    else {
        emit normalPresentation();
        static_cast<ApplicationController*>(m_window->controller())
                ->setFullScreenPresentationNavigationEnabled(true);
        m_highlightToolActivated = false;
        m_controller->show();
    }
}

void PresentationTool::drawLineTo( const QPoint &endPoint )
{
    QPainter painter( image );
    QPen pen( Qt::SolidPattern, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    pen.setColor( Qt::red );
    painter.setPen( pen );
    painter.drawLine( lastPoint, endPoint );
    m_window->update();
    lastPoint = endPoint;
}

void PresentationTool::drawEllipse( const QPoint &cpoint )
{
    delete image;
    image = new QImage( *image1 );
    QPainter painter( image );
    QColor c( Qt::black );
    c.setAlphaF( 0.5 );
    QPainterPath ellipse;
    ellipse.addEllipse( cpoint.x() - 50, cpoint.y() - 50, 100, 100 );
    QPainterPath myPath;
    myPath.addRect( QRectF(QPointF(0, 0), m_window->size()));
    // We draw the difference
    painter.fillPath( myPath.subtracted( ellipse ), c );
    m_window->update();
}

bool PresentationTool::getPenToolStatus()
{
    return m_penToolActivated;
}

bool PresentationTool::getHighlightToolStatus()
{
    return m_highlightToolActivated;
}

QImage *PresentationTool::getImage()
{
    return image;
}

void PresentationTool::deactivateTool()
{
    m_penToolActivated = false;
    m_highlightToolActivated = false;
    m_controller->show();
}

bool PresentationTool::toolsActivated()
{
    return ( m_penToolActivated || m_highlightToolActivated );
}
