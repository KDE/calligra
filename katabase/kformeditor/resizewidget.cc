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

ResizeWidget::ResizeWidget( Type _type, QColor _color, QWidget* _parent, WFlags f )
  : QWidget( _parent, 0, f ), m_type ( _type ), m_color ( _color )
{
  resize( 8, 8 );
  raise();
  setBackgroundColor( m_color );
}
 
ResizeWidget::~ResizeWidget()
{
}

void ResizeWidget::mouseMoveEvent( QMouseEvent* _event )
{
  if( _event->state() & LeftButton )
  {
    // TODO: hier das parent resizen
  }
}

#include "resizewidget.moc"

