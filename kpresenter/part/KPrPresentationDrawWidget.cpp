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
    setMouseTracking( false );
    m_size = canvas->size();

    resize( m_size );
    m_nbPolyline = 0;
    m_nbPoints = 0;
}

KPrPresentationDrawWidget::~KPrPresentationDrawWidget()
{
} 
void KPrPresentationDrawWidget::paintEvent(QPaintEvent * event)
{
/**
  QPainter * painter = new QPainter(this);
  painter->setPen( Qt::black );
  for(int j = 0; j<i; j++)
  {
    if(point[j].x()>0 && point[j].x()<m_size.rwidth() && point[j].y()>0 && point[j].y()<m_size.rwidth())
    {
    painter->drawPoint(point[j]);
    painter->drawPoint((point[j].x())+1,(point[j].y())+1 );
    painter->drawPoint(point[j].x(),point[j].y()+1 );
    painter->drawPoint(point[j].x()-1,point[j].y()+1 );
    painter->drawPoint(point[j].x()+1,point[j].y()-1 );
    painter->drawPoint(point[j].x(),point[j].y()-1 );
    painter->drawPoint(point[j].x()-1,point[j].y()-1 );
    painter->drawPoint(point[j].x()+1,point[j].y() );
    painter->drawPoint(point[j].x()-1,point[j].y() );
    }
  }
  delete painter;
*/
  m_painter = new QPainter(this);    
m_painter->setClipRect(event->rect(), Qt::NoClip);
m_painter->restore();
QBrush brush(Qt::SolidPattern);
QPen pen(brush, 10, Qt::CustomDashLine, Qt::RoundCap, Qt::RoundJoin);
//QPen pen(brush, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
pen.setColor(Qt::black);
m_painter->setPen(pen);
/*    m_painter->drawPoint(m_point);
    m_painter->drawPoint((m_point.x())+1,(m_point.y())+1 );
    m_painter->drawPoint(m_point.x(),m_point.y()+1 );
    m_painter->drawPoint(m_point.x()-1,m_point.y()+1 );
    m_painter->drawPoint(m_point.x()+1,m_point.y()-1 );
    m_painter->drawPoint(m_point.x(),m_point.y()-1 );
    m_painter->drawPoint(m_point.x()-1,m_point.y()-1 );
    m_painter->drawPoint(m_point.x()+1,m_point.y() );
    m_painter->drawPoint(m_point.x()-1,m_point.y() );
*/
for(int i=0; i < m_point.count(); i++)
  m_painter->drawPolyline(QPolygonF((QVector<QPointF>)m_point.at(i)));

//m_painter->drawPoint(m_point);
m_painter->save();
delete m_painter;
}

void KPrPresentationDrawWidget::mousePressEvent( QMouseEvent* e ){
m_nbPoints=0;
m_point.append(QVector<QPointF>());
m_nbPolyline++;
/*  kDebug() << m_nbPolyline;*/
}

void KPrPresentationDrawWidget::mouseMoveEvent( QMouseEvent* e )
{
  QVector<QPointF> * list = &m_point.last();
  list->append(QPointF(e->pos()));
  this->update();
}
