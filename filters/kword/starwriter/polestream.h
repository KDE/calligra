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

#ifndef POLE_STREAM_H
#define POLE_STREAM_H

namespace POLE
{

class StreamIO;

class Stream
{
  public:

    Stream( StreamIO* io );

    ~Stream();

    unsigned long size();

    unsigned long tell();

    void seek( unsigned long pos ); 

    unsigned long read( unsigned char* data, unsigned long maxlen );

  private:

    StreamIO* io;

    // no copy or assign
    Stream( const Stream& );
    Stream& operator=( const Stream& );
};

}

#endif // POLE_STREAM_H
