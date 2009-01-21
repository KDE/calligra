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

#include <QWidget>
#include <QMouseEvent>

#include <KoPACanvas.h>
#include <QPoint>
#include <QImage>
#include <QColor>
#include <QPainter>
#include <kdebug.h>

#include "KPrPresentationHighlightWidget.h"

KPrPresentationHighlightWidget::KPrPresentationHighlightWidget(KoPACanvas * canvas) : QWidget(canvas)
{
    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( true );
    m_size = canvas->size();
    resize( m_size );
    
    /*QPixmap newPage( m_size );

    QColor c( Qt::black ); c.setAlphaF( 0.5 ); newPage.fill( c );

    resize( m_size );

    m_blackBackgroundframe = new QFrame(this);
    QVBoxLayout *frameLayout2 = new QVBoxLayout();
    m_label = new QLabel();
    
    m_label->setPixmap(newPage);
    
    frameLayout2->addWidget( m_label, 0, Qt::AlignCenter );
    m_blackBackgroundframe->setLayout( frameLayout2 );
    m_blackBackgroundframe->move( -4,-4 );
*/
    

}

KPrPresentationHighlightWidget::~KPrPresentationHighlightWidget()
{
   // delete m_blackBackgroundframe;  
}

void KPrPresentationHighlightWidget::paintEvent ( QPaintEvent * event )
{
    QPainter *painter = new QPainter( this );
    painter->setPen( Qt::black );
    
    for( int i = 0 ; i < m_size.rwidth () ; i++ )
    {
	for ( int j = 0 ; j < m_size.rheight (); j++ )
	{
	    if(  sqrt( (m_center.x()-i)*(m_center.x()-i) + (m_center.y()-j)*(m_center.y()-j) ) >= 100 )
	    {
		painter->drawPoint( i, j );
	    }
	    /*else
	    {
		painter->setPen( Qt::white );
		painter->drawPoint( i, j );
	    }*/
	}
    }
    
    //painter->setPen( Qt::red );
    //painter->drawEllipse( m_center, 50, 50 );
}

void KPrPresentationHighlightWidget::mouseMoveEvent( QMouseEvent* e )
{
    m_center = e->pos();
    
    update();
}
/*
void KPrPresentationHighlightWidget::drawCircle( QPoint p )
{
    // QColor c( Qt::red ); c.setAlphaF( 0.5 ); newPage.fill( c );
    
    QImage image( m_size, QImage::Format_ARGB32 );

    for( int i = 0 ; i < m_size.rwidth () ; i++ )
    {
	for ( int j = 0 ; j < m_size.rheight (); j++ )
	{
	    if(  sqrt( (p.x()-i)*(p.x()-i) + (p.y()-j)+(p.y()-j) ) < 50 )
		image.setPixel( i, j, qRgba(0,0,0,150) );
	    else
		image.setPixel( i, j, qRgba(0,0,0,150) );
	}
    }
    
    QPixmap newPage( m_size );
    newPage.fromImage( image );
    m_label->setPixmap(newPage);
    update();
}*/