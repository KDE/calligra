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

ResizeWidget::ResizeWidget( QWidget* _parent, QWidget* _widget, Type _type, QColor _color, WFlags f )
  : QWidget( _parent, 0, f ), m_widget( _widget ), m_type ( _type ), m_color ( _color )
{
  setBackgroundColor( m_color );
  setType( m_type );
}

ResizeWidget::ResizeWidget( QWidget* _widget, Type _type, QColor _color, WFlags f )
  : QWidget( _widget, 0, f ), m_widget( _widget ), m_type ( _type ), m_color ( _color )
{
  setBackgroundColor( m_color );
  setType( m_type );
}

ResizeWidget::~ResizeWidget()
{
}

void ResizeWidget::resizeMini()
{
  resize( RESIZEWIDGET_SIZE, RESIZEWIDGET_SIZE );
}

void ResizeWidget::setType( Type _type )
{
  m_type = _type;

  switch( m_type )
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

QSize ResizeWidget::sizeHint() const
{
  return QSize( RESIZEWIDGET_SIZE, RESIZEWIDGET_SIZE );
}

#include <qpainter.h>

void ResizeWidget::paintEvent( QPaintEvent* _event )
{
  // TODO: paint some button style stuff

  int x1, y1, x2, y2;
  QPainter p;
  QColorGroup g = colorGroup();

  rect().coords( &x1, &y1, &x2, &y2 );   // get coordinates
  int w = x2 + 1;
  int h = y2 + 1;

  p.begin( this );

  p.setPen( g.dark() );
  p.drawRect( x1, y1, x2, y2 );

  p.end();
}

void ResizeWidget::mouseMoveEvent( QMouseEvent* _event )
{
  if( _event->state() & LeftButton )
  {
    QRect newSize;
    QPoint pos = mapToParent( _event->pos() );

    switch( m_type )
    {
      case Bottom :
        newSize = QRect( 0, 0 , m_widget->width(), pos.y() );
        break;
      case Right :
        newSize = QRect( 0, 0 , pos.x(), m_widget->height() );
        break;
      case BottomRight :
        newSize = QRect( 0, 0 , pos.x(), pos.y() );
        break;
      default :
        cerr << "Hier ist der Wurm drin!!!!" << endl;
        break;
    }

    emit resizing( newSize );
    emit rearrangeResizers();
    slotRearrange();
  }
}

void ResizeWidget::slotRearrange()
{
  switch( m_type )
  {
    case Right :
      move( m_widget->x() + m_widget->width(), m_widget->y() );
      resize( RESIZEWIDGET_SIZE, m_widget->height() );
      break;
    case Bottom :
      move( m_widget->x(), m_widget->y() + m_widget->height() );
      resize( m_widget->width(), RESIZEWIDGET_SIZE );
      break;
    case BottomRight :
      move( m_widget->x() + m_widget->width(), m_widget->y() + m_widget->height() );
      resizeMini();
      break;
    default :
      cerr << "Hier ist der Wurm drin!!!!" << endl;
      break;
  }
}

#include "resizewidget.moc"

