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

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlistbox.h>

FormObject::FormObject()
  : m_type( Label )
  , m_posx( 0 )
  , m_posy( 0 )
  , m_sizex( 100 )
  , m_sizey( 30 )
  , m_text( "text" )
  , m_action( "action" )
{
}

FormObject::FormObject( Type _type, int _posx, int _posy, int _sizex, int _sizey )
  : m_type( _type )
  , m_posx( _posx )
  , m_posy( _posy )
  , m_sizex( _sizex )
  , m_sizey( _sizey )
  , m_text( "text" )
  , m_action( "action" )
{
}

FormObject::FormObject( Type _type, int _posx, int _posy, int _sizex, int _sizey, QString _text )
  : m_type( _type )
  , m_posx( _posx )
  , m_posy( _posy )
  , m_sizex( _sizex )
  , m_sizey( _sizey )
  , m_text( _text )
  , m_action( "action" )
{
}

FormObject::FormObject( const FormObject& _to_copy ) : QObject()
{
  m_type   = _to_copy.m_type;
  m_posx   = _to_copy.m_posx;
  m_posy   = _to_copy.m_posy;
  m_sizex  = _to_copy.m_sizex;
  m_sizey  = _to_copy.m_sizey;
  m_text   = _to_copy.m_text;
  m_action = _to_copy.m_action;
}
  
FormObject::~FormObject()
{
}

void FormObject::move( int _posx, int _posy )
{
  m_posx = _posx;
  m_posy = _posy;
}

void FormObject::resize( int _sizex, int _sizey )
{
  m_sizex = _sizex;
  m_sizey = _sizey;
}

void FormObject::setGeometry( QWidget* _widget )
{
  _widget->setGeometry( m_posx, m_posy, m_sizex, m_sizey );
}

FormObject::Type FormObject::type()
{
  return m_type;
}

QWidget* FormObject::create( QWidget* _parent, const char* _name)
{
  QWidget* widget;

  switch( m_type )
  {
    case Button: 
      widget = new QPushButton( _parent, _name );
      ((QPushButton*) widget)->setText( m_text );
      break;
    case LineEdit:
      widget = new QLineEdit( _parent, _name );
      break;
    case ListBox:
      widget = new QListBox( _parent, _name );
      break;
    default:
    case Label:
      widget = new QLabel( _parent, _name );
      ((QLabel*) widget)->setText( m_text );
      break;
  }

  widget->setGeometry( m_posx, m_posy, m_sizex, m_sizey );
  return widget;
}

void FormObject::setAction( QString _action )
{
  m_action = _action;
}

QString FormObject::action()
{
  return m_action;
}

