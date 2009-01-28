/* This file is part of the KDE project
 * Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
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

#include <QtGui/QVBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

#include <KoPACanvas.h>
#include <KoPointerEvent.h>
#include <kdebug.h>

#include "KPrPresentationDrawWidget.h"

KPrPresentationDrawWidget::KPrPresentationDrawWidget(KoPACanvas * canvas) : QWidget(canvas)
{
    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( true );
    m_size = canvas->size();

    resize( m_size );
    
    m_draw = false;
}

KPrPresentationDrawWidget::~KPrPresentationDrawWidget()
{
}

void KPrPresentationDrawWidget::paintEvent(QPaintEvent * event)
{
    QPainter painter( this );    
    QBrush brush( Qt::SolidPattern );
    QPen pen( brush, 10, Qt::CustomDashLine, Qt::RoundCap, Qt::RoundJoin );
    pen.setColor( Qt::black );
    painter.setPen( pen );
    for( int i=0; i < m_pointVectors.count(); i++ )
      painter.drawPolyline( QPolygonF( m_pointVectors.at(i) ) );
}

void KPrPresentationDrawWidget::mousePressEvent( QMouseEvent* e )
{
    m_pointVectors.append( QVector<QPointF>() << e->pos() );
    switchDraw();
}

void KPrPresentationDrawWidget::mouseMoveEvent( QMouseEvent* e )
{
    if(m_draw)
    {
	m_pointVectors.last() << e->pos();
	update();
    }
}

void KPrPresentationDrawWidget::switchDraw()
{
    m_draw = !m_draw;
}
