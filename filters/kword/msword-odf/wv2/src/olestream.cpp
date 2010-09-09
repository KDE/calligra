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

#include <stdio.h> // FILE,...

#include <gsf/gsf-input.h>
#include <gsf/gsf-output.h>
#include <gsf/gsf-input-memory.h>
#include <gsf/gsf-msole-utils.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using namespace wvWare;

OLEStream::OLEStream( OLEStorage* storage ) : m_storage( storage )
{
}

OLEStream::~OLEStream()
{
    if ( m_storage )
        m_storage->streamDestroyed( this );
}

void OLEStream::push()
{
    m_positions.push( tell() );
}

bool OLEStream::pop()
{
    if ( m_positions.empty() )
        return false;
    seek( m_positions.top(), G_SEEK_SET );
    m_positions.pop();
    return true;
}


OLEStreamReader::OLEStreamReader( GsfInput* stream, OLEStorage* storage ) :
    OLEStream( storage ), m_stream( stream )
{
}

OLEStreamReader::~OLEStreamReader()
{
    if ( m_stream )
        g_object_unref( G_OBJECT( m_stream ) );
}

bool OLEStreamReader::isValid() const
{
    return m_stream;
}

bool OLEStreamReader::seek( int offset, GSeekType whence )
{
    return gsf_input_seek( m_stream, offset, whence ) == 0;
}

int OLEStreamReader::tell() const
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return -1;
#endif
    return gsf_input_tell( m_stream );
}

size_t OLEStreamReader::size() const
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return 0;
#endif
    return gsf_input_size( m_stream );
}

OLEStreamReader* OLEStreamReader::inflate( int offset ) const
{
    //call the inflate function
    wvlog << "calling gsf_msole_inflate with offset of ... " << offset;
    GByteArray* gbArray = gsf_msole_inflate( m_stream, offset );
    wvlog << " got " << gbArray->len << " decompressed bytes." << endl;
    //transform it to an unsigned char* buffer (better way to do this?)
    unsigned char* buffer = new unsigned char [gbArray->len];
    for ( uint i = 0; i < gbArray->len; i++ ) {
        wvlog << (unsigned int) gbArray->data[i];
        buffer[i] = (unsigned char) gbArray->data[i];
    }
    //create storage
    /*OLEStorage store( buffer, gbArray->len );
    if ( !store.open( OLEStorage::ReadOnly ) )
        wvlog << "Couldn't open OLEStorage." << endl;
    //now get a stream reader
    OLEStreamReader* reader = store.createStreamReader("");
    */
    //create gsfinput
    GsfInput* input = GSF_INPUT( gsf_input_memory_new( buffer, gbArray->len, false ) );
    //create an OLEStreamReader from that
    OLEStreamReader* reader = new OLEStreamReader( input, 0 );
    //free memory
    g_byte_array_free (gbArray, true);
    delete[] buffer;
    //store.close();

    //return the OLEStreamReader
    return reader;
}

U8 OLEStreamReader::readU8()
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return 0;
#endif

    U8 ret;
    gsf_input_read( m_stream, sizeof( ret ), static_cast<guint8*>( &ret ) );
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
    gsf_input_read( m_stream, sizeof( ret ), reinterpret_cast<guint8*>( &ret ) );
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
    gsf_input_read( m_stream, sizeof( ret ), reinterpret_cast<guint8*>( &ret ) );
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
    return gsf_input_read( m_stream, length, buffer ) != 0;
}

void OLEStreamReader::dumpStream( const std::string& fileName )
{
    push();
    seek( 0, G_SEEK_SET );

    FILE* myFile = fopen( fileName.c_str(), "w" );
    if ( !myFile ) {
        pop();
        return;
    }

    const size_t buflen = 1024;
    char buffer[ buflen ];
    size_t remaining = size();
    size_t length;

    while ( remaining ) {
        length = remaining > buflen ? buflen : remaining;
        remaining -= length;
        if ( gsf_input_read( m_stream, length, reinterpret_cast<guint8*>( buffer ) ) )
            fwrite( buffer, 1, length, myFile );
    }

    fclose( myFile );
    pop();
}


OLEImageReader::OLEImageReader( OLEStreamReader& reader, unsigned int start, unsigned int limit ) :
    m_reader( reader ), m_start( start ), m_limit( limit ), m_position( start )
{
    if ( limit <= start )
        wvlog << "Error: The passed region is empty." << endl;
}

OLEImageReader::OLEImageReader( const OLEImageReader& rhs ) : m_reader( rhs.m_reader ), m_start( rhs.m_start ),
                                                              m_limit( rhs.m_limit ), m_position( rhs.m_position )
{
}

OLEImageReader::~OLEImageReader()
{
    // nothing to do
}

bool OLEImageReader::isValid() const
{
    return m_reader.isValid() && m_position >= m_start && m_position < m_limit;
}

bool OLEImageReader::seek( int offset, GSeekType whence )
{
    switch( whence ) {
        case G_SEEK_CUR:
            return updatePosition( m_position + offset );
        case G_SEEK_SET:
            return updatePosition( offset );
        case G_SEEK_END:
            return updatePosition( m_limit - 1 + offset );
        default:
            wvlog << "Error: Unknown GSeekType!" << endl;
            return false;
    }
}

int OLEImageReader::tell() const
{
    return static_cast<int>( m_position );
}

size_t OLEImageReader::size() const
{
    return m_limit - m_start;
}

size_t OLEImageReader::read( U8 *buffer, size_t length )
{
    m_reader.push();
    if ( !m_reader.seek( m_position, G_SEEK_SET ) ) {
        m_reader.pop();
        return 0;
    }

    size_t bytesRead = ( m_limit - m_position ) < length ? m_limit - m_position : length;
    if ( !m_reader.read( buffer, bytesRead ) ) {
        m_reader.pop();
        return 0;
    }
    //have to update our position in the stream
    unsigned int newpos = m_position + (unsigned int) bytesRead;
    wvlog << "new position is " << newpos << endl;
    if ( !updatePosition( newpos ) )
        wvlog << "error updating position in stream" << endl;
    m_reader.pop();
    return bytesRead;
}

bool OLEImageReader::updatePosition( unsigned int position )
{
    if ( m_start <= position && position < m_limit ) {
        m_position = position;
        return true;
    }
    return false;
}


OLEStreamWriter::OLEStreamWriter( GsfOutput* stream, OLEStorage* storage ) :
    OLEStream( storage ), m_stream( stream )
{
}

OLEStreamWriter::~OLEStreamWriter()
{
    if ( m_stream ) {
        gsf_output_close( m_stream );
        g_object_unref( G_OBJECT( m_stream ) );
    }
}

bool OLEStreamWriter::isValid() const
{
    return m_stream;
}

bool OLEStreamWriter::seek( int offset, GSeekType whence )
{
    return gsf_output_seek( m_stream, offset, whence ) == 0;
}

int OLEStreamWriter::tell() const
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return -1;
#endif
    return gsf_output_tell(  m_stream );
}

size_t OLEStreamWriter::size() const
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return 0;
#endif
    return gsf_output_size( m_stream );
}

void OLEStreamWriter::write( U8 data )
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return;
#endif
    gsf_output_write( m_stream, sizeof( data ), &data );
}

void OLEStreamWriter::write( S8 data )
{
    write( static_cast<U8>( data ) );
}

void OLEStreamWriter::write( U16 data )
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return;
#endif
    U16 copy = toLittleEndian( data );
    gsf_output_write( m_stream, sizeof( copy ), reinterpret_cast<guint8 *>( &copy ) );
}

void OLEStreamWriter::write( S16 data )
{
    write( static_cast<U16>( data ) );
}

void OLEStreamWriter::write( U32 data )
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return;
#endif
    U32 copy = toLittleEndian( data );
    gsf_output_write( m_stream, sizeof( copy ), reinterpret_cast<guint8 *>( &copy ) );
}

void OLEStreamWriter::write( S32 data )
{
    write( static_cast<U32>( data ) );
}

void OLEStreamWriter::write( U8* data, size_t length )
{
#ifdef WV2_CHECKING
    if ( !m_stream )
        return;
#endif
    gsf_output_write( m_stream, length, data );
}

GsfOutput* OLEStreamWriter::getGsfStream()
{
    return m_stream;
}

