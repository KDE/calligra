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

#include <koStream.h>

#include <stdlib.h>

#include <qimage.h>
#include <qiodevice.h>
#include <qbuffer.h>
#include <qpicture.h>
#include <qpen.h>
#include <qfont.h>


using namespace std;

ostream& operator<< ( ostream& outs, const QRect &_rect )
{
  outs << "<RECT x=" << _rect.left() << " y=" << _rect.top() << " w=" << _rect.width() << " h=" << _rect.height() << " />";

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

/* ostream& operator<< ( ostream& outs, const QRect &_rect )
{
  char buffer[ 4 * 8 + 3 + 1 ];

  intToHexStr( buffer, _rect.left() );
  buffer[8] = ',';
  intToHexStr( buffer + 9, _rect.top() );
  buffer[17] = ',';
  intToHexStr( buffer + 18, _rect.width() );
  buffer[26] = ',';
  intToHexStr( buffer + 27, _rect.height() );

  outs << buffer;

  return outs;
}

QRect strToRect( const char *_buffer )
{
  QRect r;

  r.setRect( hexStrToInt( _buffer ), hexStrToInt( _buffer + 9 ),
	     hexStrToInt( _buffer + 18 ), hexStrToInt( _buffer + 27 ) );

  return r;
}

istream& operator>> ( istream& ins, QRect &_rect )
{
  char buffer[ 4 * 8 + 1 ];
  ins.get( buffer, 4*8 );

  _rect.setRect( hexStrToInt( buffer ), hexStrToInt( buffer + 9 ),
		 hexStrToInt( buffer + 18 ), hexStrToInt( buffer + 27 ) );

  return ins;
} */

ostream& operator<< ( ostream& outs, const QColor &_color )
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

istream& operator>> ( istream& ins, QColor &_color )
{
  char buffer[ 1 + 3 * 2 + 1 ];
  ins.get( buffer, 1 + 3*2 );

  _color.setRgb( hexStrToShort( buffer + 1 ), hexStrToShort( buffer + 3 ), hexStrToShort( buffer + 5 ) );

  return ins;
}

ostream& operator<< ( ostream& outs, const QPen &_pen )
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

/* ostream& operator<< ( ostream& outs, const QPen &_pen )
{
  char buffer[ 2 * 8 + 2 + 1 ];

  intToHexStr( buffer, _pen.width() );
  buffer[8] = ',';
  intToHexStr( buffer + 9, (int)_pen.style() );

  outs << buffer << ' ' << _pen.color();

  return outs;
}

QPen strToPen( const char *_buffer )
{
  PenStyle style = (PenStyle)(hexStrToInt( _buffer + 9 ) );

  QPen p;
  p.setWidth( hexStrToInt( _buffer ) );
  p.setStyle( style );
  p.setColor( strToColor( _buffer + 17 ) );

  return p;
}

istream& operator>> ( istream& ins, QPen &_pen )
{
  char buffer[ 2 * 8 + 1 + 3 * 2 + 1 ];
  ins.get( buffer, 2*8 + 1 + 3*2 );

  PenStyle style = (PenStyle)hexStrToInt( buffer + 9 );

  _pen.setWidth( hexStrToInt( buffer ) );
  _pen.setStyle( style );
  _pen.setColor( strToColor( buffer + 17 ) );

  return ins;
}
*/

ostream& operator<< ( ostream& outs, const QFont &_font )
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

class QIO2CPP : public QIODevice
{
public:
  QIO2CPP( ostream &_out) : m_out( _out )
  {
    setType( IO_Sequential );
    setMode( IO_WriteOnly );
    setState( IO_Open );
    setStatus( IO_Ok );
  }
  ~QIO2CPP() { };

  bool open( int ) { return true; }
  void close() { }
  void flush() { m_out.flush(); }

  uint size() const { return 0xffffffff; }
  int readBlock( char *, uint ) { return 0; }
  int writeBlock( const char *data, uint len ) { m_out.write( data, len ); return len; }
  int readLine( char *, uint  ) { return 0; };

  int getch() { return 0; };
  int putch( int _c ) { m_out.put( _c ); return _c; }
  int ungetch( int ) { return 0; }

protected:
  ostream &m_out;
};

ostream& operator<< ( ostream& outs, const QImage &_img )
{
  QIO2CPP out( outs );
  QImageIO io( &out, "BMP" );
  io.setImage( _img );
  io.write();

  return outs;
}

void writeImageToStream( ostream &outs, const QImage &_img, const QString &_format )
{
  QIO2CPP out( outs );
  QImageIO io( &out, _format.upper() );
  io.setImage( _img );
  io.write();
}

ostream& operator<< ( ostream& outs, const QPicture &_pic )
{
  QIO2CPP out( outs );
  out.writeBlock( _pic.data(), _pic.size() );

  return outs;
}

class CPP2QIO : public QIODevice
{
public:
  CPP2QIO( istream &_in) : m_in( _in )
  {
    setType( IO_Direct );
    setMode( IO_ReadOnly );
    setState( IO_Open );
    setStatus( IO_Ok );
  }
  ~CPP2QIO() { };

  bool open( int ) { return true; }
  void close() { }
  void flush() { }
  bool atEnd() { if ( m_in.eof() ) return true; return false; }
  int at() const { return m_in.tellg(); }
  bool at( int _pos ) { m_in.seekg( _pos ); return true; }

  uint size() const { return 0xffffffff; }
  int readBlock( char *data, uint len ) { m_in.read( data, len ); return m_in.gcount(); }
  int writeBlock( const char *, uint ) { return -1; }
  /* int readLine( char *data, uint maxlen )
  {
    int len = 0;
    if ( maxlen > 2 )
    {
      m_in.get( data, maxlen - 2 );
      len = m_in.gcount();
    }
    if ( maxlen > 0 )
    {
      int c = m_in.get();
      if ( c != EOF )
	data[ len++ ] = c;
      data[ len++ ] = 0;
    }
    return len;
  } */

  int getch() { int c = m_in.get(); if ( c == EOF ) return -1; return c; }
  int putch( int ) { return -1; }
  int ungetch( int _c ) { m_in.putback( _c ); return _c; }

protected:
  istream &m_in;
};

istream& operator>> ( istream& ins, QImage &_img )
{
  // Create a random access device in memory
  char buffer[ 4096 ];

  QBuffer buff;
  buff.open( IO_WriteOnly );

  while ( !ins.eof() )
  {
    ins.read( buffer, 4096 );
    buff.writeBlock( buffer, ins.gcount() );
  }

  buff.close();

  _img.loadFromData( buff.buffer() );

  return ins;
}

istream &operator>>( istream &ins, QPicture &_pic )
{
    // Create a random access device in memory
    char buffer[ 4096 ];

    QBuffer buff;
    buff.open( IO_WriteOnly );

    while ( !ins.eof() )
    {
        ins.read( buffer, 4096 );
        buff.writeBlock( buffer, ins.gcount() );
    }

    buff.close();

    _pic.setData( buff.buffer(), buff.size() );

    return ins;
}
