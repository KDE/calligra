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

#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <kdebug.h>

#include "komlMime.h"

void Base64::encode( char *_dest, unsigned char c1, unsigned char c2, unsigned char c3, int _len )
{
  /* Conversion table. */
  static char tbl[64] = {
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/'
  };

  /* Transform the 3x8 bits to 4x6 bits, as required by
     base64.  */
  *_dest++ = tbl[ c1 >> 2 ];
  *_dest++ = tbl[ ( ( c1 & 3) << 4 ) | ( c2 >> 4 ) ];
  *_dest++ = tbl[ ( ( c2 & 0xf) << 2 ) | ( c3 >> 6 ) ];
  *_dest++ = tbl[ c3 & 0x3f ];
  *_dest = 0;

  if ( _len == 2 )
    *(_dest - 1) = '=';
  else if ( _len == 1 )
    *(_dest - 1) = *(_dest - 2) = '=';
}

int Base64::decode( char *_dest, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4 )
{
  static unsigned char tbl[128] = {
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377','\377','\377','\377','\377','\377',
    '\377','\377','\377',    62,'\377','\377','\377',    63,
        52,    53,    54,    55,    56,    57,    58,    59,
        60,    61,'\377','\377','\377','\377','\377','\377',
    '\377',     0,     1,     2,     3,     4,     5,     6,
         7,     8,     9,    10,    11,    12,    13,    14,
        15,    16,    17,    18,    19,    20,    21,    22,
        23,    24,    25,'\377','\377','\377','\377','\377',
    '\377',    26,    27,    28,    29,    30,    31,    32,
        33,    34,    35,    36,    37,    38,    39,    40,
        41,    42,    43,    44,    45,    46,    47,    48,
        49,    50,    51,'\377','\377','\377','\377','\377'
  };

  int conv = 3;
  if ( c3 == '=' )
    conv = 1;
  else if ( c4 == '=' )
    conv = 2;

  c1 = tbl[ c1 ];
  c2 = tbl[ c2 ];
  c3 = tbl[ c3 ];
  c4 = tbl[ c4 ];

  if ( conv >= 1 )
    *_dest++ = ( c1 << 2 ) | ( ( c2 >> 4 ) & 3 );
  if ( conv >= 2 )
    *_dest++ = ( c2 << 4 ) | ( c3 >> 2 );
  if ( conv == 3 )
    *_dest++ = ( ( c3 & 3 ) << 6 ) | c4;

  return conv;
}

int Base64DecodeBuffer::underflow()
{
    // Is the read position within the buffer?
    if (gptr() < egptr() )
    {
      return *gptr();
    }

    if ( m_in.eof() )
    {
      kdError(30001) << "Unexpected end of input" << endl;
      m_bEnd = true;
    }

    if ( m_bEnd )
      return EOF;

    /* calculate the size of the putback-zone
     *  - amount of the characters read
     *  - max. 4
     */
    int anzPutback;
    anzPutback = gptr() - eback();
    if ( anzPutback > 4 )
      anzPutback = 4;

    /* There can be 4 characters max. right now - put them
     * in the putback-zone (first 4 characters)
     */
    memcpy( m_buffer + ( 4 - anzPutback ), gptr()-anzPutback, anzPutback );

    int want = ( m_iBufferSize - 4 ) * 4 / 3;
    char *buf = new char[ want ];

    int got = 0;
    while( !m_in.eof() && !m_bEnd && got < want )
    {
      char c = m_in.get();
      buf[ got ] = c;
      if ( c == '=' )
      {
        kdDebug(30001) << "END OF BASE64" << endl;

        if ( got % 4 == 2 )
        {
          if ( m_in.eof() )
          {
            kdError(30001) << "Unexpected EOF" << endl;
            delete [] buf;
            return EOF;
          }
          got++;
          c = m_in.get();
          if ( c != '=' )
          {
            kdError(30001) << "Not correct base64" << endl;
            delete [] buf;
            return EOF;
          }
          buf[ got++ ] = c;
          m_bEnd = true;
        }
        else if ( got % 4 == 3 )
        {
          got++;
          m_bEnd = true;
        }
        else
        {
          kdError(30001) << "Unexpected =" << endl;
          delete [] buf;
          return EOF;
        }
      }
      else if ( !isspace( c ) )
        got++;
    }

    if( got % 4 != 0 )
    {
      kdError(30001) << "Unexpected EOF 2" << endl;
      delete [] buf;
      return EOF;
    }

    int anz = 0;
    int cnt = got / 4;
    for( int j = 0; j < cnt; j++ )
    {
      int conv = decode( m_buffer + j * 3 + 4, buf[ j * 4 ], buf[ j * 4 + 1 ], buf[ j * 4 + 2 ], buf[ j * 4 + 3 ] );
      assert( conv == 3 || j == cnt - 1 );
      anz += conv;
    }

    /* Set the buffer-pointer
     */
    setg ( m_buffer + ( 4 - anzPutback ),   // start of the putback-zone
           m_buffer + 4,                // read position
           m_buffer + 4 + anz );           // end of the buffer

    delete [] buf;

    // return the next character
    return *gptr();
}
