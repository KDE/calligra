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
  : m_widget( _widget )
{
  _widget->reparent( this, f, QPoint( 0, 0 ), TRUE );

  setFixedSize( _widget->size() );

  // only for debugging
  _widget->hide();
}
 
WidgetWrapper::~WidgetWrapper()
{
}

void WidgetWrapper::mousePressEvent( QMouseEvent* _event )
{
  cerr << "WidgetWrapper::mousePressEvent()" << endl;
}

void WidgetWrapper::resizeEvent( QResizeEvent* _event )
{
}

void WidgetWrapper::paintEvent( QPaintEvent* _event )
{
  QPainter p;

  p.begin( this );

  p.drawRect( 2, 2, width() - 3, height() - 3 );

  p.fillRect( 0, 0, 5, 5, QBrush( SolidPattern ) );
  p.fillRect( width() - 5, 0, width(), 5, QBrush( SolidPattern ) );
  p.fillRect( 0, height() - 5, 5, height(), QBrush( SolidPattern ) );
  p.fillRect( width() - 5, height() - 5, width(), height(), QBrush( SolidPattern ) );

  p.end();
}

