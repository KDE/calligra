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

Stream::Stream( StreamIO* _io ):
  io( _io )
{
}

Stream::~Stream()
{
  // tell parent we're gone
}

unsigned long Stream::tell()
{
  return io ? io->pos : 0;
}

void Stream::seek( unsigned long newpos )
{
  if( io ) io->pos = newpos;
}

unsigned long Stream::size()
{
  return io ? io->entry->size : 0;
}

unsigned long Stream::read( unsigned char* data, unsigned long maxlen )
{
  return io ? io->read( data, maxlen ) : 0;
}
