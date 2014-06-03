/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#include "olestream.h"
#include "wvlog.h"
#include "pole.h"

#include <stdio.h> // FILE,...



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using namespace wvWare;

OLEStream::OLEStream( OLEStorage* storage ) : m_storage( storage )
{
}

OLEStream::~OLEStream()
{
}

void OLEStream::push()
{
    m_positions.push( tell() );
}

bool OLEStream::pop()
{
    if ( m_positions.empty() )
        return false;
    seek( m_positions.top(), WV2_SEEK_SET );
    m_positions.pop();
    return true;
}


OLEStreamReader::OLEStreamReader( POLE::Stream *stream, OLEStorage *storage ) :
    OLEStream( storage ), m_stream( stream ), m_pos(0)
{
}

OLEStreamReader::~OLEStreamReader()
{
    delete m_stream;
}

bool OLEStreamReader::isValid() const
{
    return m_stream;
}

bool OLEStreamReader::seek( int offset, WV2SeekType whence )
{
    unsigned long tempPos = m_pos;

    switch(whence) {
    case WV2_SEEK_CUR:
        tempPos += offset;
        break;
    case WV2_SEEK_SET:
        tempPos = offset;
        break;
    }

    if (tempPos > m_stream->size())
    {
        return false;
    }

    m_pos = tempPos;
    m_stream->seek(m_pos);

    return true;
}

int OLEStreamReader::tell() const
{
    if (!m_stream) {
        return -1;
    }

    return m_pos;
}

size_t OLEStreamReader::size() const
{
    if (!m_stream) {
        return 0;
    }

    return m_stream->size();
}

U8 OLEStreamReader::readU8()
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return 0;
#endif

    U8 ret;
    m_pos += m_stream->read( static_cast<unsigned char*>(&ret), sizeof(U8) );

    return ret;
}

S8 OLEStreamReader::readS8()
{
    return static_cast<S8>( readU8() );
}

U16 OLEStreamReader::readU16()
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return 0;
#endif

#if defined(WORDS_BIGENDIAN)
    // Please take care when "optimizing" that and read
    // http://gcc.gnu.org/ml/gcc-bugs/2000-12/msg00429.html
    // and http://www.eskimo.com/~scs/C-faq/q3.8.html
    U16 tmp1 = readU8();
    U16 tmp2 = readU8();
    return ( tmp2 << 8 ) | tmp1;
#else
    U16 ret;
    m_pos += m_stream->read( reinterpret_cast<unsigned char*>(&ret), sizeof(U16) );
    return ret;
#endif
}

S16 OLEStreamReader::readS16()
{
    return static_cast<S16>( readU16() );
}

U32 OLEStreamReader::readU32()
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return 0;
#endif

#if defined(WORDS_BIGENDIAN)
    // Please take care when "optimizing" that and read
    // http://gcc.gnu.org/ml/gcc-bugs/2000-12/msg00429.html
    // and http://www.eskimo.com/~scs/C-faq/q3.8.html
    U32 tmp1 = readU16();
    U32 tmp2 = readU16();
    return ( tmp2 << 16 ) | tmp1;
#else
    U32 ret;
    m_pos += m_stream->read( reinterpret_cast<unsigned char*>(&ret), sizeof(U32) );
    return ret;
#endif
}

S32 OLEStreamReader::readS32()
{
    return static_cast<S32>( readU32() );
}

bool OLEStreamReader::read( U8 *buffer, size_t length )
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return false;
#endif
    m_pos += m_stream->read( static_cast<unsigned char*>(buffer), length );
    return true;
}


OLEStreamWriter::OLEStreamWriter( OLEStorage* storage ) :
    OLEStream( storage )
{
}

OLEStreamWriter::~OLEStreamWriter()
{
}

bool OLEStreamWriter::isValid() const
{
    return false;
}

bool OLEStreamWriter::seek( int /*offset*/, WV2SeekType /*whence*/ )
{
    return false;
}

int OLEStreamWriter::tell() const
{
    return 0;
}

size_t OLEStreamWriter::size() const
{
    return 0;
}

void OLEStreamWriter::write( U8 /*data*/ )
{
}

void OLEStreamWriter::write( S8 /*data*/ )
{
}

void OLEStreamWriter::write( U16 /*data*/ )
{
}

void OLEStreamWriter::write( S16 /*data*/ )
{
}

void OLEStreamWriter::write( U32 /*data*/ )
{
}

void OLEStreamWriter::write( S32 /*data*/ )
{
}

void OLEStreamWriter::write( U8* /*data*/, size_t /*length*/ )
{
}
