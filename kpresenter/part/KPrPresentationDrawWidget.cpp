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
    
    i = 0;
}

KPrPresentationDrawWidget::~KPrPresentationDrawWidget()
{
  
} 
void KPrPresentationDrawWidget::paintEvent(QPaintEvent * event)
{
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
}
void KPrPresentationDrawWidget::mouseMoveEvent( QMouseEvent* e )
{
kDebug() << "1";
  point[i] = e->pos();
  i++;
  
  this->update();
  kDebug() << "2";
}