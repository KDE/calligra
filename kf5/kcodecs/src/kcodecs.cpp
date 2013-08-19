/*
   Copyright (C) 2000-2001 Dawit Alemayehu <adawit@kde.org>
   Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License (LGPL)
   version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

   RFC 1321 "MD5 Message-Digest Algorithm" Copyright (C) 1991-1992.             // krazy:exclude=copyright
   RSA Data Security, Inc. Created 1991. All rights reserved.

   The KMD5 class is based on a C++ implementation of
   "RSA Data Security, Inc. MD5 Message-Digest Algorithm" by
   Mordechai T. Abzug,	Copyright (c) 1995.  This implementation                // krazy:exclude=copyright
   passes the test-suite as defined in RFC 1321.

   The encoding and decoding utilities in KCodecs with the exception of
   quoted-printable are based on the java implementation in HTTPClient
   package by Ronald Tschalär Copyright (C) 1996-1999.                          // krazy:exclude=copyright

   The quoted-printable codec as described in RFC 2045, section 6.7. is by
   Rik Hemsley (C) 2001.
*/

#include "kcodecs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <QtCore/QDebug>
#include <QtCore/QIODevice>
#include <QtCore/QTextCodec>

#if defined(Q_OS_WIN)
#define strncasecmp _strnicmp
#endif

namespace KCodecs
{

static const char Base64EncMap[64] =
{
  0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
  0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
  0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
  0x59, 0x5A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
  0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
  0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
  0x77, 0x78, 0x79, 0x7A, 0x30, 0x31, 0x32, 0x33,
  0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2B, 0x2F
};

static const char Base64DecMap[128] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x3F,
  0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
  0x3C, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
  0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
  0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
  0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
  0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
  0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
  0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const char UUEncMap[64] =
{
  0x60, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
  0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F
};

static const char UUDecMap[128] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const char hexChars[16] =
{
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

static const unsigned int maxQPLineLength = 70;

} // namespace KCodecs


/******************************** KCodecs ********************************/
// strchr(3) for broken systems.
static int rikFindChar(register const char * _s, const char c)
{
  register const char * s = _s;

  while (true)
  {
    if ((0 == *s) || (c == *s)) break; ++s;
    if ((0 == *s) || (c == *s)) break; ++s;
    if ((0 == *s) || (c == *s)) break; ++s;
    if ((0 == *s) || (c == *s)) break; ++s;
  }

  return s - _s;
}

QByteArray KCodecs::quotedPrintableEncode(const QByteArray& in, bool useCRLF)
{
  QByteArray out;
  quotedPrintableEncode (in, out, useCRLF);
  return out;
}

void KCodecs::quotedPrintableEncode(const QByteArray& in, QByteArray& out, bool useCRLF)
{
  out.resize (0);
  if (in.isEmpty())
    return;

  char *cursor;
  const char *data;
  unsigned int lineLength;
  unsigned int pos;

  const unsigned int length = in.size();
  const unsigned int end = length - 1;


  // Reasonable guess for output size when we're encoding
  // mostly-ASCII data. It doesn't really matter, because
  // the underlying allocation routines are quite efficient,
  // but it's nice to have 0 allocations in many cases.
  out.resize ((length*12)/10);
  cursor = out.data();
  data = in.data();
  lineLength = 0;
  pos = 0;

  for (unsigned int i = 0; i < length; i++)
  {
    unsigned char c (data[i]);

    // check if we have to enlarge the output buffer, use
    // a safety margin of 16 byte
    pos = cursor-out.data();
    if (out.size()-pos < 16) {
      out.resize(out.size()+4096);
      cursor = out.data()+pos;
    }

    // Plain ASCII chars just go straight out.

    if ((c >= 33) && (c <= 126) && ('=' != c))
    {
      *cursor++ = c;
      ++lineLength;
    }

    // Spaces need some thought. We have to encode them at eol (or eof).

    else if (' ' == c)
    {
      if
        (
         (i >= length)
         ||
         ((i < end) && ((useCRLF && ('\r' == data[i + 1]) && ('\n' == data[i + 2]))
                        ||
                        (!useCRLF && ('\n' == data[i + 1]))))
        )
      {
        *cursor++ = '=';
        *cursor++ = '2';
        *cursor++ = '0';

        lineLength += 3;
      }
      else
      {
        *cursor++ = ' ';
        ++lineLength;
      }
    }
    // If we find a line break, just let it through.
    else if ((useCRLF && ('\r' == c) && (i < end) && ('\n' == data[i + 1])) ||
             (!useCRLF && ('\n' == c)))
    {
      lineLength = 0;

      if (useCRLF) {
        *cursor++ = '\r';
        *cursor++ = '\n';
        ++i;
      } else {
        *cursor++ = '\n';
      }
    }

    // Anything else is converted to =XX.

    else
    {
      *cursor++ = '=';
      *cursor++ = hexChars[c / 16];
      *cursor++ = hexChars[c % 16];

      lineLength += 3;
    }

    // If we're approaching the maximum line length, do a soft line break.

    if ((lineLength > maxQPLineLength) && (i < end))
    {
      if (useCRLF) {
        *cursor++ = '=';
        *cursor++ = '\r';
        *cursor++ = '\n';
      } else {
        *cursor++ = '=';
        *cursor++ = '\n';
      }

      lineLength = 0;
    }
  }

  out.truncate(cursor - out.data());
}

QByteArray KCodecs::quotedPrintableDecode(const QByteArray & in)
{
  QByteArray out;
  quotedPrintableDecode (in, out);
  return out;
}


void KCodecs::quotedPrintableDecode(const QByteArray& in, QByteArray& out)
{
  // clear out the output buffer
  out.resize (0);
  if (in.isEmpty())
      return;

  char *cursor;
  const char *data;
  const unsigned int length = in.size();

  data = in.data();
  out.resize (length);
  cursor = out.data();

  for (unsigned int i = 0; i < length; i++)
  {
    char c(data[i]);

    if ('=' == c)
    {
      if (i < length - 2)
      {
        char c1 = data[i + 1];
        char c2 = data[i + 2];

        if (('\n' == c1) || ('\r' == c1 && '\n' == c2))
        {
          // Soft line break. No output.
          if ('\r' == c1)
            i += 2;        // CRLF line breaks
          else
            i += 1;
        }
        else
        {
          // =XX encoded byte.

          int hexChar0 = rikFindChar(hexChars, c1);
          int hexChar1 = rikFindChar(hexChars, c2);

          if (hexChar0 < 16 && hexChar1 < 16)
          {
            *cursor++ = char((hexChar0 * 16) | hexChar1);
            i += 2;
          }
        }
      }
    }
    else
    {
      *cursor++ = c;
    }
  }

  out.truncate(cursor - out.data());
}

QByteArray KCodecs::base64Encode( const QByteArray& in, bool insertLFs )
{
    QByteArray out;
    base64Encode( in, out, insertLFs );
    return out;
}

void KCodecs::base64Encode( const QByteArray& in, QByteArray& out,
                            bool insertLFs )
{
    // clear out the output buffer
    out.resize (0);
    if ( in.isEmpty() )
        return;

    unsigned int sidx = 0;
    int didx = 0;
    const char* data = in.data();
    const unsigned int len = in.size();

    unsigned int out_len = ((len+2)/3)*4;

    // Deal with the 76 characters or less per
    // line limit specified in RFC 2045 on a
    // pre request basis.
    insertLFs = (insertLFs && out_len > 76);
    if ( insertLFs )
      out_len += ((out_len-1)/76);

    int count = 0;
    out.resize( out_len );

    // 3-byte to 4-byte conversion + 0-63 to ascii printable conversion
    if ( len > 1 )
    {
        while (sidx < len-2)
        {
            if ( insertLFs )
            {
                if ( count && (count%76) == 0 )
                    out[didx++] = '\n';
                count += 4;
            }
            out[didx++] = Base64EncMap[(data[sidx] >> 2) & 077];
            out[didx++] = Base64EncMap[((data[sidx+1] >> 4) & 017) |
                                       ((data[sidx] << 4) & 077)];
            out[didx++] = Base64EncMap[((data[sidx+2] >> 6) & 003) |
                                       ((data[sidx+1] << 2) & 077)];
            out[didx++] = Base64EncMap[data[sidx+2] & 077];
            sidx += 3;
        }
    }

    if (sidx < len)
    {
        if ( insertLFs && (count > 0) && (count%76) == 0 )
           out[didx++] = '\n';

        out[didx++] = Base64EncMap[(data[sidx] >> 2) & 077];
        if (sidx < len-1)
        {
            out[didx++] = Base64EncMap[((data[sidx+1] >> 4) & 017) |
                                       ((data[sidx] << 4) & 077)];
            out[didx++] = Base64EncMap[(data[sidx+1] << 2) & 077];
        }
        else
        {
            out[didx++] = Base64EncMap[(data[sidx] << 4) & 077];
        }
    }

    // Add padding
    while (didx < out.size())
    {
        out[didx] = '=';
        didx++;
    }
}

QByteArray KCodecs::base64Decode( const QByteArray& in )
{
    QByteArray out;
    base64Decode( in, out );
    return out;
}

void KCodecs::base64Decode( const QByteArray& in, QByteArray& out )
{
    out.resize(0);
    if ( in.isEmpty() )
        return;

    int count = 0;
    int len = in.size(), tail = len;
    const char* data = in.data();

    // Deal with possible *nix "BEGIN" marker!!
    while ( count < len && (data[count] == '\n' || data[count] == '\r' ||
            data[count] == '\t' || data[count] == ' ') )
        count++;

    if ( strncasecmp(data+count, "begin", 5) == 0 )
    {
        count += 5;
        while ( count < len && data[count] != '\n' && data[count] != '\r' )
            count++;

        while ( count < len && (data[count] == '\n' || data[count] == '\r') )
            count ++;

        data += count;
        tail = (len -= count);
    }

    // Find the tail end of the actual encoded data even if
    // there is/are trailing CR and/or LF.
    while ( data[tail-1] == '=' || data[tail-1] == '\n' ||
            data[tail-1] == '\r' )
        if ( data[--tail] != '=' ) len = tail;

    unsigned int outIdx = 0;
    out.resize( (count=len) );
    for (int idx = 0; idx < count; idx++)
    {
        // Adhere to RFC 2045 and ignore characters
        // that are not part of the encoding table.
        unsigned char ch = data[idx];
        if ((ch > 47 && ch < 58) || (ch > 64 && ch < 91) ||
            (ch > 96 && ch < 123) || ch == '+' || ch == '/' || ch == '=')
        {
            out[outIdx++] = Base64DecMap[ch];
        }
        else
        {
            len--;
            tail--;
        }
    }

    // qDebug() << "Tail size = " << tail << ", Length size = " << len;

    // 4-byte to 3-byte conversion
    len = (tail>(len/4)) ? tail-(len/4) : 0;
    int sidx = 0, didx = 0;
    if ( len > 1 )
    {
      while (didx < len-2)
      {
          out[didx] = (((out[sidx] << 2) & 255) | ((out[sidx+1] >> 4) & 003));
          out[didx+1] = (((out[sidx+1] << 4) & 255) | ((out[sidx+2] >> 2) & 017));
          out[didx+2] = (((out[sidx+2] << 6) & 255) | (out[sidx+3] & 077));
          sidx += 4;
          didx += 3;
      }
    }

    if (didx < len)
        out[didx] = (((out[sidx] << 2) & 255) | ((out[sidx+1] >> 4) & 003));

    if (++didx < len )
        out[didx] = (((out[sidx+1] << 4) & 255) | ((out[sidx+2] >> 2) & 017));

    // Resize the output buffer
    if ( len == 0 || len < out.size() )
      out.resize(len);
}

QByteArray KCodecs::uuencode( const QByteArray& in )
{
    QByteArray out;
    uuencode( in, out );
    return QByteArray( out.data(), out.size()+1 );
}

void KCodecs::uuencode( const QByteArray& in, QByteArray& out )
{
    out.resize( 0 );
    if( in.isEmpty() )
        return;

    unsigned int sidx = 0;
    int didx = 0;
    unsigned int line_len = 45;

    const char nl[] = "\n";
    const char* data = in.data();
    const unsigned int nl_len = strlen(nl);
    const unsigned int len = in.size();

    out.resize( (len+2)/3*4 + ((len+line_len-1)/line_len)*(nl_len+1) );
    // split into lines, adding line-length and line terminator
    while (sidx+line_len < len)
    {
        // line length
        out[didx++] = UUEncMap[line_len];

        // 3-byte to 4-byte conversion + 0-63 to ascii printable conversion
        for (unsigned int end = sidx+line_len; sidx < end; sidx += 3)
        {
            out[didx++] = UUEncMap[(data[sidx] >> 2) & 077];
            out[didx++] = UUEncMap[((data[sidx+1] >> 4) & 017) |
                                   ((data[sidx] << 4) & 077)];
            out[didx++] = UUEncMap[((data[sidx+2] >> 6) & 003) |
                                ((data[sidx+1] << 2) & 077)];
            out[didx++] = UUEncMap[data[sidx+2] & 077];
        }

        // line terminator
        //for (unsigned int idx=0; idx < nl_len; idx++)
        //out[didx++] = nl[idx];
        memcpy(out.data()+didx, nl, nl_len);
        didx += nl_len;
    }

    // line length
    out[didx++] = UUEncMap[len-sidx];
    // 3-byte to 4-byte conversion + 0-63 to ascii printable conversion
    while (sidx+2 < len)
    {
        out[didx++] = UUEncMap[(data[sidx] >> 2) & 077];
        out[didx++] = UUEncMap[((data[sidx+1] >> 4) & 017) |
                               ((data[sidx] << 4) & 077)];
        out[didx++] = UUEncMap[((data[sidx+2] >> 6) & 003) |
                               ((data[sidx+1] << 2) & 077)];
        out[didx++] = UUEncMap[data[sidx+2] & 077];
        sidx += 3;
    }

    if (sidx < len-1)
    {
        out[didx++] = UUEncMap[(data[sidx] >> 2) & 077];
        out[didx++] = UUEncMap[((data[sidx+1] >> 4) & 017) |
                               ((data[sidx] << 4) & 077)];
        out[didx++] = UUEncMap[(data[sidx+1] << 2) & 077];
        out[didx++] = UUEncMap[0];
    }
    else if (sidx < len)
    {
        out[didx++] = UUEncMap[(data[sidx] >> 2) & 077];
        out[didx++] = UUEncMap[(data[sidx] << 4) & 077];
        out[didx++] = UUEncMap[0];
        out[didx++] = UUEncMap[0];
    }

    // line terminator
    memcpy(out.data()+didx, nl, nl_len);
    didx += nl_len;

    // sanity check
    if ( didx != out.size() )
        out.resize( 0 );
}

QByteArray KCodecs::uudecode( const QByteArray& in )
{
    QByteArray out;
    uudecode( in, out );
    return out;
}

void KCodecs::uudecode( const QByteArray& in, QByteArray& out )
{
    out.resize( 0 );
    if( in.isEmpty() )
        return;

    int sidx = 0;
    int didx = 0;
    int len = in.size();
    int line_len, end;
    const char* data = in.data();

    // Deal with *nix "BEGIN"/"END" separators!!
    int count = 0;
    while ( count < len && (data[count] == '\n' || data[count] == '\r' ||
            data[count] == '\t' || data[count] == ' ') )
        count ++;

    bool hasLF = false;
    if ( strncasecmp( data+count, "begin", 5) == 0 )
    {
        count += 5;
        while ( count < len && data[count] != '\n' && data[count] != '\r' )
            count ++;

        while ( count < len && (data[count] == '\n' || data[count] == '\r') )
            count ++;

        data += count;
        len -= count;
        hasLF = true;
    }

    out.resize( len/4*3 );
    while ( sidx < len )
    {
        // get line length (in number of encoded octets)
        line_len = UUDecMap[ (unsigned char) data[sidx++]];
        // ascii printable to 0-63 and 4-byte to 3-byte conversion
        end = didx+line_len;
        char A, B, C, D;
        if (end > 2) {
          while (didx < end-2)
          {
             A = UUDecMap[(unsigned char) data[sidx]];
             B = UUDecMap[(unsigned char) data[sidx+1]];
             C = UUDecMap[(unsigned char) data[sidx+2]];
             D = UUDecMap[(unsigned char) data[sidx+3]];
             out[didx++] = ( ((A << 2) & 255) | ((B >> 4) & 003) );
             out[didx++] = ( ((B << 4) & 255) | ((C >> 2) & 017) );
             out[didx++] = ( ((C << 6) & 255) | (D & 077) );
             sidx += 4;
          }
        }

        if (didx < end)
        {
            A = UUDecMap[(unsigned char) data[sidx]];
            B = UUDecMap[(unsigned char) data[sidx+1]];
            out[didx++] = ( ((A << 2) & 255) | ((B >> 4) & 003) );
        }

        if (didx < end)
        {
            B = UUDecMap[(unsigned char) data[sidx+1]];
            C = UUDecMap[(unsigned char) data[sidx+2]];
            out[didx++] = ( ((B << 4) & 255) | ((C >> 2) & 017) );
        }

        // skip padding
        while (sidx < len  && data[sidx] != '\n' && data[sidx] != '\r')
            sidx++;

        // skip end of line
        while (sidx < len  && (data[sidx] == '\n' || data[sidx] == '\r'))
            sidx++;

        // skip the "END" separator when present.
        if ( hasLF && strncasecmp( data+sidx, "end", 3) == 0 )
            break;
    }

    if ( didx < out.size()  )
        out.resize( didx );
}



QString KCodecs::decodeRFC2047String(const QString &msg)
{
    QString charset;
    QChar encoding;
    QString notEncodedText;
    QString encodedText;
    QString decodedText;
    int encEnd=0;
    if(!msg.startsWith(QLatin1String("=?")) || (encEnd=msg.lastIndexOf(QLatin1String("?=")))==-1)
        return msg;

    notEncodedText=msg.mid(encEnd+2);
    encodedText=msg.left(encEnd);
    encodedText=encodedText.mid(2,encodedText.length()-2);
    int questionMark=encodedText.indexOf(QLatin1Char('?'));
    if (questionMark==-1)
        return msg;
    charset=encodedText.left(questionMark).toLower();
    encoding=encodedText.at(questionMark+1).toLower();
    if (encoding != QLatin1Char('b') && encoding != QLatin1Char('q'))
        return msg;
    encodedText=encodedText.mid(questionMark+3);
    if(charset.indexOf(QLatin1Char(' '))!=-1 && encodedText.indexOf(QLatin1Char(' '))!=-1)
        return msg;
    QByteArray tmpOut;
    QByteArray tmpIn = encodedText.toLocal8Bit();
    if(encoding == QLatin1Char('q'))
        tmpOut = KCodecs::quotedPrintableDecode(tmpIn);
    else
        tmpOut = KCodecs::base64Decode(tmpIn);
    if (charset != QLatin1String("us-ascii")) {
        QTextCodec *codec = QTextCodec::codecForName(charset.toLocal8Bit());
        if(!codec)
            return msg;
        decodedText = codec->toUnicode(tmpOut);
        decodedText = decodedText.replace(QLatin1Char('_'), QLatin1Char(' '));
    } else {
        decodedText = QString::fromLocal8Bit(tmpOut.replace('_', ' ').data());
    }

    return decodedText + notEncodedText;
}

