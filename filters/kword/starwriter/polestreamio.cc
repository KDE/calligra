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
  io( _io ), entry( _entry ), pos( 0 )
{
  blocks = ( entry->size  > io->threshold ) ? io->bb.follow( entry->start ) :
     io->sb.follow( entry->start );
}

StreamIO::~StreamIO()
{
  // tell parent we're gone
}

unsigned long StreamIO::read( unsigned char* data, unsigned long maxlen )
{
  unsigned char* ptr = data;
  unsigned long bytes = maxlen;

  if ( entry->size < io->threshold )
  {
    // small file
    unsigned long index = pos / io->sb_size;
    unsigned long offset = pos % io->sb_size;
    unsigned long slack = io->sb_size - offset;

    if( index >= blocks.size() ) return 0;

    unsigned long first = blocks[ index ];
    unsigned char buf[ io->sb_size ];

    io->loadSmallBlock( first, buf, io->sb_size );
    memcpy( ptr, buf + offset, (maxlen < slack) ? maxlen : slack );

    if( maxlen > slack )
    {
      std::vector<unsigned long> chain;
      chain.resize( blocks.size()-index );
      for( unsigned long k=0; k < blocks.size(); k++ )
        chain[k] = blocks[index+k+1];
      bytes = slack + io->loadSmallBlocks( chain, ptr + slack, maxlen-slack );
    }
  }
  else
  {
    // big file
    unsigned long index = pos / io->bb_size;
    unsigned long offset = pos % io->bb_size;
    unsigned long slack = io->bb_size - offset;

    if( index >= blocks.size() ) return 0;

    unsigned long first = blocks[ index ];
    unsigned char buf[ io->bb_size ];

    io->loadBigBlock( first, buf, io->bb_size );
    memcpy( ptr, buf + offset, (maxlen < slack) ? maxlen : slack );

    if( maxlen > slack )
    {
       std::vector<unsigned long> chain( blocks.size()-index );
       for( unsigned long k=0; k < blocks.size(); k++ )
         chain[k] = blocks[index+k+1];
       bytes = slack + io->loadBigBlocks( chain, ptr + slack, maxlen-slack );
     }
  }

  pos += bytes;

  return bytes;
}
