/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <stdlib.h>

#include <qiodevice.h>
#include <qbuffer.h>
#include <qpen.h>
#include <qfont.h>

#include "koStream.h"

QTextStream& operator<< ( QTextStream& outs, const QRect &_rect )
{
  outs << "<RECT x=\"" << _rect.left() << "\" y=\"" << _rect.top() << "\" w=\"" << _rect.width() << "\" h=\"" << _rect.height() << "\" />";
  return outs;
}

QRect tagToRect( vector<KOMLAttrib>& _attribs )
{
  QRect r;

  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end() ; it++ )
  {
    if ( (*it).m_strName == "x" )
      r.setLeft( atoi( (*it).m_strValue.c_str() ) );
    else if ( (*it).m_strName == "y" )
      r.setTop( atoi( (*it).m_strValue.c_str() ) );
    else if ( (*it).m_strName == "w" )
      r.setWidth( atoi( (*it).m_strValue.c_str() ) );
    else if ( (*it).m_strName == "h" )
      r.setHeight( atoi( (*it).m_strValue.c_str() ) );
  }

  return r;
}

QTextStream& operator<< ( QTextStream& outs, const QColor &_color )
{
  char buffer[ 3 * 2 + 1 ];

  shortToHexStr( buffer, _color.red() );
  shortToHexStr( buffer + 2, _color.green() );
  shortToHexStr( buffer + 4, _color.blue() );
  buffer[ 3*2 ] = 0;

  outs << '#' << buffer;

  return outs;
}

QColor strToColor( const char *_buffer )
{
  QColor c;
  c.setRgb( hexStrToShort( _buffer + 1 ), hexStrToShort( _buffer + 3 ), hexStrToShort( _buffer + 5 ) );

  return c;
}

QTextStream& operator>> ( QTextStream& ins, QColor &_color )
{
  char buffer[ 1 + 3 * 2 + 1 ];
  ins.readRawBytes( buffer, 1 + 3*2 );

  _color.setRgb( hexStrToShort( buffer + 1 ), hexStrToShort( buffer + 3 ), hexStrToShort( buffer + 5 ) );

  return ins;
}

QTextStream& operator<< ( QTextStream& outs, const QPen &_pen )
{
  outs << "<PEN style=" << (int)_pen.style() << " width=" << _pen.width() << " color=" << _pen.color() << "/>";

  return outs;
}

QPen tagToPen( vector<KOMLAttrib>& _attribs )
{
  QPen p;

  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end() ; it++ )
  {
    if ( (*it).m_strName == "style" )
    {
      Qt::PenStyle style = (Qt::PenStyle)atoi( (*it).m_strValue.c_str() );
      p.setStyle( style );
    }
    else if ( (*it).m_strName == "width" )
      p.setWidth( atoi( (*it).m_strValue.c_str() ) );
    else if ( (*it).m_strName == "color" )
      p.setColor( strToColor( (*it).m_strValue.c_str() ) );
  }

  return p;
}

QTextStream& operator<< ( QTextStream& outs, const QFont &_font )
{
  outs << "<FONT family=\"" << _font.family().ascii() << "\" pt=" << _font.pointSize() << " weight="
       << _font.weight();
  if ( _font.italic() )
    outs << " italic/>";
  else
    outs << "/>";

  return outs;
}

QFont tagToFont( vector<KOMLAttrib>& _attribs )
{
  // QFont f;
  QFont f( "Times", 12 );

  vector<KOMLAttrib>::const_iterator it = _attribs.begin();
  for( ; it != _attribs.end() ; it++ )
  {
    if ( (*it).m_strName == "family" )
      f.setFamily( (*it).m_strValue.c_str() );
    else if ( (*it).m_strName == "pt" )
      f.setPointSize( atoi( (*it).m_strValue.c_str() ) );
    else if ( (*it).m_strName == "weight" )
      f.setWeight( atoi( (*it).m_strValue.c_str() ) );
    else if ( (*it).m_strName == "italic" )
      f.setItalic( true );
    else if ( (*it).m_strName == "bold" )
      f.setBold( true );
  }

  return f;
}
