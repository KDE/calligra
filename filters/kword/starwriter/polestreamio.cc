/* POLE - Portable library to access OLE Storage
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>

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
   Boston, MA 02111-1307, US
*/

#include <vector>

#include <polestorage.h>
#include <polestorageio.h>
#include <polestream.h>
#include <polestreamio.h>

using namespace POLE;

StreamIO::StreamIO( StorageIO* _io, Entry* _entry ):
  io( _io ), entry( _entry ), m_pos( 0 ),
  cache_data( 0 ), cache_size( 0 ), cache_pos( 0 )
{
  blocks = ( entry->size  > io->threshold ) ? io->bb.follow( entry->start ) :
     io->sb.follow( entry->start );

  // prepare cache
  cache_size = 4096; // optimal ?
  cache_data = new unsigned char[cache_size];
  updateCache();
}

// FIXME tell parent we're gone
StreamIO::~StreamIO()
{
  delete[] cache_data;  
}

void StreamIO::seek( unsigned long pos )
{
  m_pos = pos;
}

unsigned long StreamIO::tell()
{
  return m_pos;
}

int StreamIO::getch()
{
  // past end-of-file ?
  if( m_pos > entry->size ) return -1;

  // need to update cache ?
  if( !cache_size || ( m_pos < cache_pos ) ||
    ( m_pos >= cache_pos + cache_size ) )
      updateCache();

  // something bad if we don't get good cache
  if( !cache_size ) return -1;

  int data = cache_data[m_pos - cache_pos];
  m_pos++;

  return data;
}

unsigned long StreamIO::read( unsigned long pos, unsigned char* data, unsigned long maxlen )
{
  // sanity checks
  if( !data ) return 0;
  if( maxlen == 0 ) return 0;

  unsigned long totalbytes = 0;

  if ( entry->size < io->threshold )
  {
    // small file
    unsigned long index = pos / io->sb_size;

    if( index >= blocks.size() ) return 0;

    unsigned char buf[ io->sb_size ];
    while( totalbytes < maxlen )
    {
      if( index >= blocks.size() ) break;
      io->loadSmallBlock( blocks[index], buf, io->bb_size );
      unsigned long count = io->sb_size;
      if( count > maxlen-totalbytes ) count = maxlen-totalbytes;
      memcpy( data+totalbytes, buf, count );
      totalbytes += count;
      index++;
    }

  }
  else
  {
    // big file
    unsigned long index = pos / io->bb_size;

    if( index >= blocks.size() ) return 0;

    unsigned char buf[ io->bb_size ];
    while( totalbytes < maxlen )
    {
      if( index >= blocks.size() ) break;
      io->loadBigBlock( blocks[index], buf, io->bb_size );
      unsigned long count = io->bb_size;
      if( count > maxlen-totalbytes ) count = maxlen-totalbytes;
      memcpy( data+totalbytes, buf, count );
      totalbytes += count;
      index++;
    }

  }

  return totalbytes;
}

unsigned long StreamIO::read( unsigned char* data, unsigned long maxlen )
{
  unsigned long bytes = read( tell(), data, maxlen );
  m_pos += bytes;
  return bytes;
}

void StreamIO::updateCache()
{
  // sanity check
  if( !cache_data ) return;

  cache_pos = m_pos - ( m_pos % cache_size );
  unsigned long bytes = cache_size;
  if( cache_pos + bytes > entry->size ) bytes = entry->size - cache_pos;
  cache_size = read( cache_pos, cache_data, bytes );
}
