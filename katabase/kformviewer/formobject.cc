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

#include "formobject.h"

FormObject::FormObject( int _posx, int _posy, int _sizex, int _sizey )
  : m_posx( _posx )
  , m_posy( _posy )
  , m_sizex( _sizex )
  , m_sizey( _sizey )
{
};
  
FormObject::~FormObject()
{
};

void FormObject::move( int _posx, int _posy )
{
  m_posx = _posx;
  m_posy = _posy;
};

void FormObject::resize( int _sizex, int _sizey )
{
  m_sizex = _sizex;
  m_sizey = _sizey;
};

void FormObject::setGeometry( QWidget* _widget )
{
  _widget->setGeometry( m_posx, m_posy, m_sizex, m_sizey );
};

FormButton::FormButton( int _posx, int _posy, int _sizex, int _sizey, QString _title )
  : FormObject( _posx, _posy, _sizex, _sizey )
  , m_title( _title )
{
};

FormButton::~FormButton()
{
};

QWidget* FormButton::create( QWidget* _parent = 0L, const char* _name = 0L )
{
  QPushButton* button = new QPushButton( _parent, _name );
  setGeometry( button );
  button->setText( m_title );
  return button;
};

