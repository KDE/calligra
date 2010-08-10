/* This file is part of the KDE project
 * Copyright (C) 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * Copyright (C) 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * Copyright (C) 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "KPrPresentationHighlightWidget.h"

#include <KoPACanvas.h>

#include <QMouseEvent>
#include <QColor>
#include <QPainter>

KPrPresentationHighlightWidget::KPrPresentationHighlightWidget( KoPACanvasBase * canvas )
: KPrPresentationToolEventForwarder(canvas)
, m_size( canvas->canvasWidget()->size() )
{
    // The focus and the track for have the mouse position every time
    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( true );
    // Size of the canvas is saved because it's used in the paintEvent
    resize( m_size );

    m_center = QCursor::pos();
    update();
}

KPrPresentationHighlightWidget::~KPrPresentationHighlightWidget()
{
}

/** paintEvent call with the update in the mouseMoveEvent */
void KPrPresentationHighlightWidget::paintEvent( QPaintEvent * event )
{
    Q_UNUSED(event);
    QPainter painter( this );
    QPen myPen;
    QColor c( Qt::black );
    // TODO make alpha configurable
    c.setAlphaF( 0.5 );
    // The circle we want
    QPainterPath ellipse;
    // TODO make radius configurable
    ellipse.addEllipse( m_center.x() - 75, m_center.y() - 75, 150, 150 );
    // All the 'background'
    QPainterPath myPath;
    myPath.addRect( 0, 0, m_size.rwidth(), m_size.rheight() );
    // We draw the difference
    painter.setPen( myPen );
    painter.fillPath( myPath.subtracted( ellipse ), c );
}

/** Take the mouse position every time the mouse is moving */
void KPrPresentationHighlightWidget::mouseMoveEvent( QMouseEvent* e )
{
    // Save the position of the mouse
    m_center = e->pos();
    // Update the screen : move the circle with a paint event
    // TODO maybe only update what has changed
    update();
}
