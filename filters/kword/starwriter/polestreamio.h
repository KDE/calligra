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

#ifndef POLE_STREAMIO_H
#define POLE_STREAMIO_H

#include <vector>

namespace POLE
{

class StorageIO;
class Entry;

class StreamIO
{
  public:

    StreamIO( StorageIO* io, Entry* entry );

    ~StreamIO();

    unsigned long size();

    unsigned long read( unsigned char* data, unsigned long maxlen );

    StorageIO* io;

    Entry* entry;

    unsigned long pos;

  private:

    std::vector<unsigned long> blocks;

    // no copy or assign
    StreamIO( const StreamIO& );
    StreamIO& operator=( const StreamIO& );

};

}

#endif // POLE_STREAMIO_H
