/*
 *  Copyright (c) 2001 Graham Short.  <grahshrt@netscape.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef QPRO_STREAM_H
#define QPRO_STREAM_H

//#define USE_QT

#ifdef USE_QT

#include <QDataStream>
#include <QBuffer>

typedef quint8  QP_UINT8;
typedef qint8   QP_INT8;
typedef qint16  QP_INT16;
typedef qint32  QP_INT32;

class QpStream : public QDataStream
{
public:
    QpStream(unsigned char* pBuffer, unsigned int pLen);
    ~QpStream();

protected:
    QBuffer        cBuf;
    QByteArray     cByteArray;

    unsigned char* cBuffer;
    unsigned int   cLen;
};

#else

#include <iostream>
using namespace std;

// ??? sort out how to do sizes

typedef char            QP_INT8  ;
typedef unsigned char   QP_UINT8  ;
typedef short           QP_INT16 ;
typedef int             QP_INT32 ;
typedef double          QP_INT64 ;

class QpIStream
{
public:
    QpIStream(const char* pFileName);
    QpIStream(unsigned char* pBuffer, unsigned int pLen);
    ~QpIStream();

    int          get();

    QpIStream&   read(char* pBuf, QP_INT16 pLen);

    operator void*();
    int operator !();

    QpIStream& operator >> (QP_INT8  &pI8);
    QpIStream& operator >> (QP_UINT8 &pI8);
    QpIStream& operator >> (QP_INT16 &pI16);
    QpIStream& operator >> (QP_INT32 &pI32);
    QpIStream& operator >> (QP_INT64 &pI64);
    QpIStream& operator >> (char*& pStr);

protected:
    istream*      cIn;
    long          cOffset;
    streambuf*    cStreamBuf;
};

#endif

#endif // QPRO_STREAM_H
