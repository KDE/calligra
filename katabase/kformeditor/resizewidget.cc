/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Michael Koch <koch@kde.org>
 
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

#include "resizewidget.h"

// only for debug
#include <iostream.h>

ResizeWidget::ResizeWidget( QWidget* _widget, Type _type, QColor _color, WFlags f )
  : QWidget( _widget, 0, f ), m_widget( _widget ), m_type ( _type ), m_color ( _color )
{
  setBackgroundColor( m_color );

  switch( _type )
  {
    case TopLeft :
    case BottomRight :
      setCursor( sizeFDiagCursor );
      break;
    case TopRight :
    case BottomLeft :
      setCursor( sizeBDiagCursor );
      break;
    case Top :
    case Bottom :
      setCursor( sizeVerCursor );
      break;
    case Left :
    case Right :
      setCursor( sizeHorCursor );
      break;
    default:
      break;
  }
}

ResizeWidget::~ResizeWidget()
{
}

void ResizeWidget::resizeMini()
{
  resize( RESIZEWIDGET_SIZE, RESIZEWIDGET_SIZE );
}

QSize ResizeWidget::sizeHint() const
{
  return QSize( RESIZEWIDGET_SIZE, RESIZEWIDGET_SIZE );
}

void ResizeWidget::mouseMoveEvent( QMouseEvent* _event )
{
  if( _event->state() & LeftButton )
  {
    QPoint pos;

    pos = mapToParent( _event->pos() );

    emit resizing( QRect( QPoint( 0, 0 ), pos ) );
  }
}

#include "resizewidget.moc"

