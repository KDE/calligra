/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Michael Koch <m_koch@bigfoot.de>
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qpainter.h>

#include "widgetwrapper.h"

// only for debug
#include <iostream.h>

WidgetWrapper::WidgetWrapper()
  : m_widget( NULL ) 
{
}

WidgetWrapper::WidgetWrapper( QWidget* _widget, WFlags f )
  : m_selected ( FALSE ), m_widget( _widget )
{
  m_widget->reparent( this, f, QPoint( 0, 0 ), TRUE );
  //reparent( m_widget, f, QPoint( 0, 0 ), TRUE );

  setFixedSize( m_widget->size() );

  //m_widget->setBackgroundMode( NoBackground );
  setBackgroundMode( NoBackground );

  // TODO: child widget may not process events (only paint events)

  // only for debugging
  //m_widget->hide();
}
 
WidgetWrapper::~WidgetWrapper()
{
}

void WidgetWrapper::select( bool _selected )
{
  if( _selected )
    cerr << "WidgetWrapper::select" << endl;

  m_selected = _selected;

  QWidget::update();
}

bool WidgetWrapper::selected()
{
  return m_selected;
}

void WidgetWrapper::slotUnselect()
{
  select( FALSE );
}

void WidgetWrapper::mousePressEvent( QMouseEvent*  )
{
  cerr << "WidgetWrapper::mousePressEvent()" << endl;

  emit clicked( this );
}

void WidgetWrapper::resizeEvent( QResizeEvent*  )
{
}

void WidgetWrapper::paintEvent( QPaintEvent* )
{
  m_widget->repaint();

  if( m_selected )
  {
    QPainter p;

    p.begin( this );

    // paint the rectangle
    p.drawRect( 2, 2, width() - 3, height() - 3 );

    // paint the edge points
    p.fillRect( 0, 0, 5, 5, QBrush( SolidPattern ) );
    p.fillRect( width() - 5, 0, 5, 5, QBrush( SolidPattern ) );
    p.fillRect( 0, height() - 5, 5, 5, QBrush( SolidPattern ) );
    p.fillRect( width() - 5, height() - 5, 5, 5, QBrush( SolidPattern ) );

    // paint the middle points on the horizontal line
    if( width() > 27 )
    {
      p.fillRect( width() / 2 - 2, 0, 5, 5, QBrush( SolidPattern ) );
      p.fillRect( width() / 2 - 2, height() - 5, 5, 5, QBrush( SolidPattern ) );
    }

    // paint the middle points on the vertical line
    if( height() > 27 )
    {
      p.fillRect( 0, height() / 2 - 2, 5, 5, QBrush( SolidPattern ) );
      p.fillRect( width() - 5, height() / 2 - 2, 5, 5, QBrush( SolidPattern ) );
    }

    p.end();
  }
}

#include "widgetwrapper.moc"

