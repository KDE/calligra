/* This file is part of the KOffice project
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qiodevice.h>
#include <qstring.h>

#include <koledevice.h>
#include <kolestorageinfo.h>
#include <kolestorage.h>

using namespace KOLE;

Device::Device( StorageInfo* si, Entry* e ):
  QIODevice(),
  storageinfo( si ),
  entry( e ),
  pos( 0 ),
  cache_pos( 0 ),
  dirty( false )
{
  blocks = ( entry->size > storageinfo->threshold ) ?
    storageinfo->bb.follow( entry->start ) :
    storageinfo->sb.follow( entry->start ); 
  //updateCache();
}

Device::~Device()
{
  close();
}

bool Device::open( int mode )
{
  return true;
}

void Device::close()
{
  flush();

  // notify StorageInfo that we're closing
}

void Device::flush()
{
  // notify flushing

  // save write buffer
  if( dirty )
  {
    dirty = false;
  }
}

Q_ULONG Device::size() const
{
  return entry->size;
}

Q_LONG Device::readBlock( char* data, Q_ULONG maxlen )
{
  Q_LONG bytes = readBlock( pos, data, maxlen );
  pos += bytes;
  return bytes;
}

Q_LONG Device::writeBlock( const char* data, Q_ULONG maxlen )
{
  Q_LONG bytes = writeBlock( pos, data, maxlen );
  pos += bytes;
  return bytes;
}

Q_LONG Device::readBlock( Q_ULONG p, char* data, Q_ULONG maxlen )
{
  char *ptr = data;
  Q_ULONG bytes = maxlen;

  if( entry->size < storageinfo->threshold )
  {
    // small file
    Q_ULONG index = p / storageinfo->sb_size;
    Q_ULONG offset = p % storageinfo->sb_size;
    Q_ULONG slack = storageinfo->sb_size - offset;
    if( index >= blocks.size() ) return 0;

    unsigned first = blocks[ index ];
    QByteArray buf( storageinfo->sb_size );
    storageinfo->loadSmallBlock( first, buf.data(), buf.size() );

    memcpy( ptr, buf.data() + offset, QMIN(maxlen,slack) );
    if( maxlen > slack )
    {
      memcpy( ptr, buf.data() + offset, slack );

      QValueVector<int> chain( blocks.size()-index );
      for( unsigned k=0; k < blocks.size(); k++ )
        chain[k] = blocks[index+k+1];

      bytes = slack + storageinfo->loadSmallBlocks( chain, ptr + slack, maxlen-slack );
    }
  }
  else
  {
    // big fle
    Q_ULONG index = p / storageinfo->bb_size;
    Q_ULONG offset = p % storageinfo->bb_size;
    Q_ULONG slack = storageinfo->bb_size - offset;

    if( index >= blocks.size() ) return 0;

    unsigned first = blocks[ index ];
    QByteArray buf( storageinfo->bb_size );
    storageinfo->loadBigBlock( first, buf.data(), buf.size() );

    if( maxlen <= slack )
      memcpy( ptr, buf.data() + offset, maxlen );
    else
    {
      memcpy( ptr, buf.data() + offset, slack );

      QValueVector<int> chain( blocks.size()-index );
      for( unsigned k=0; k < blocks.size(); k++ )
        chain[k] = blocks[index+k+1];

      bytes = slack + storageinfo->loadBigBlocks( chain, ptr + slack, maxlen-slack );
    }
  }

  return bytes;
}


// not supported yet !
Q_LONG Device::writeBlock( Q_ULONG p, const char* data, Q_ULONG len )
{
  return 0;
}

Q_ULONG Device::at() const
{
  return pos;
}

bool Device::atEnd() const
{
  return pos >= entry->size;
}

bool Device::at( int p )
{
  pos = p;
  return true;
}

int Device::getch()
{

  // past end-of-file ?
  if( pos > entry->size ) return -1;

  // need to update cache ?
  if( !cache_data.size() || ( pos < cache_pos ) || 
    ( pos >= cache_pos + cache_data.size() ) )
    updateCache();  

  // something bad if we don't have good cache
  if( !cache_data.size() ) return -1;

  Q_UINT8 ch = cache_data[ pos - cache_pos ];
  pos++;

  return ch;
}

// not supported yet !
int Device::putch( int ch )
{
  return -1;
}

int Device::ungetch( int ch )
{
  pos--;
  if( pos < 0 ) pos = 0;
  return 0;
}

void Device::updateCache()
{
  // write out buffer to disk
  flush();

  // within block boundary when possible
  unsigned size = ( entry->size < storageinfo->threshold ) ?
    storageinfo->sb_size : storageinfo->bb_size;

  size = 4096;

  cache_pos = pos - (pos % size );
  cache_data.resize( size );

  size = readBlock( cache_pos, cache_data.data(), cache_data.size() );
  cache_data.resize( size );
}
