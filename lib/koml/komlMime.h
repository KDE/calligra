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

#ifndef __komlMime_h__
#define __komlMime_h__

#include <iostream>
#include <torben.h>
#include <map>
#include <string>

using std::ostream;
using std::istream;

class Base64
{
public:
  Base64() { };
  virtual ~Base64() { };

  void encode( char *p, unsigned char c1, unsigned char c2, unsigned char c3, int len = 3 );
  int decode( char *_dest, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4 );
};

#include <streambuf>

class Base64EncodeBuffer : public std::streambuf, protected Base64
{
protected:
  static const int m_bufferSize = 48;   // Groesse des Datenm_buffers
  char m_buffer[m_bufferSize];            // Datenm_buffer
  char m_buffer2[m_bufferSize * 4 / 3 + 3 + 1 ];            // Datenm_buffer 2

public:
  /* Konstruktor
   *  - Datenm_buffer initialisieren
   *  - ein Zeichen kleiner,
   *    damit das m_bufferSize-te Zeichen overflow() ausloest
   */
  Base64EncodeBuffer( ostream &_str ) : m_out( _str )
  {
    setp (m_buffer, m_buffer+(m_bufferSize-1));
  }

  /* Destruktor
   *  - Datenm_buffer leeren
   */
  virtual ~Base64EncodeBuffer()
  {
    sync();
  }

protected:
  /* Zeichen im Buffer abspeichern
   */
  int emptyBuffer()
  {
    int anz = pptr()-pbase();

    int cnt = anz / 3;
    int rest = anz % 3;
    
    int pos = 0;
    int i;
    for ( i = 0; i < cnt; i++ )
    {
      encode( m_buffer2 + i*4, m_buffer[ pos ], m_buffer[ pos + 1 ], m_buffer[ pos + 2 ], 3 );
      pos += 3;
    }
    
    if ( rest == 1 )
      encode( m_buffer2 + (i++)*4, m_buffer[ pos ], 0, 0, 1 );
    else if ( rest == 2 )
      encode( m_buffer2 + (i++)*4, m_buffer[ pos ], m_buffer[ pos + 1 ], 0, 2 );

    m_buffer2[ i*4 ] = 0;
    
    m_out.write( m_buffer2, i*4 );
    
     if ( m_out.eof() )
       return EOF;

    pbump (-anz);    // Schreibzeiger entspr. zuruecksetzen
    return anz;
  }

  /* M_Buffer voll
   *  - c und alle vorherigen Zeichen ausgeben
   */
  virtual int overflow (int c)
  {
    if (c != EOF)
    {
      // Zeichen noch in den M_Buffer einfuegen
      *pptr() = c;
      pbump(1);
    }
    if ( emptyBuffer() == EOF )
    {
      return EOF;
    }
    return c;
  }

  /* Daten mit Datentraeger abgleichen
   *  - Buffer explizit leeren
   */
  virtual int sync ()
  {
    if (emptyBuffer() == EOF)
    {
      // FEHLER
      return -1;
    }
    return 0;
  }

  ostream &m_out;
};

class Base64OStream : public ostream
{
public:
  Base64OStream( ostream& _str ) : ostream( &m_buf ), m_buf( _str ) { }

protected:
  Base64EncodeBuffer m_buf;
};


class Base64DecodeBuffer : public std::streambuf, protected Base64
{
protected:
  /* Datenpuffer:
   *  maximal 4 Zeichen Putback-Bereich plus
   *  maximal 6 Zeichen normaler Lesepuffer
   */
    const int m_iBufferSize;// = 48 + 4;     // Groesse des Datenpuffers
    char *m_buffer; // m_iBufferSize ];

public:
  /* Konstruktor
   *  - Datenpuffer leer initialisieren
   *  - keine Putback-Reserve
   *  => underflow() forcieren
   */
  Base64DecodeBuffer( istream& _str ) : m_iBufferSize(48 + 4), m_in( _str )
  {
      m_buffer = new char[m_iBufferSize];

    setg ( m_buffer + 4,     // Putback-Anfang
	   m_buffer + 4,     // Leseposition
	   m_buffer + 4 );    // Puffer-Ende

    m_bEnd = false;
  }

  ~Base64DecodeBuffer() { delete [] m_buffer; }

protected:
  /* neue Zeichen in den Puffer einlesen
   */
  virtual int underflow();
  
  istream &m_in;

  bool m_bEnd;
};

class Base64IStream : public istream
{
public:
  Base64IStream( istream& _str ) : istream( &m_buf ), m_buf( _str ) { }

protected:
  Base64DecodeBuffer m_buf;
};


class pump
{
public:
  pump( istream& _in, ostream& _out ) : m_in( _in ), m_out( _out ) { }
  
  void run()
  {
    char buffer[ 4096 ];
    while ( !m_in.eof() && m_out.good() )
    {
      m_in.read( buffer, 4096 );
      m_out.write( buffer, m_in.gcount() );
    }
  }
  
protected:
  istream &m_in;
  ostream &m_out;
};

#endif


