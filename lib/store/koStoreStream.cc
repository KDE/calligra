/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   Boston, MA 02111-1307, USA.
*/

#include "koStoreStream.h"
#include "koStore.h"

int istorestreambuffer::underflow ()
{
    // read position before end of buffer ?
    if (gptr() < egptr() )
    {
        return *gptr();
    }

    /* calculate the Putback-Zone
     *  - how many did we read already?
     *  - maximum 4 chars
     */
    int sizePutback;
    sizePutback = gptr() - eback();
    if (sizePutback > 4)
    {
        sizePutback = 4;
    }

    /* prepend the chars already read (put them
     * back in the buffer (first 4 chars)
     */
    memcpy( buffer + ( 4 - sizePutback ), gptr()-sizePutback, sizePutback );

    /* read new chars
     */
    long count = m_pStore->read( buffer + 4, bufferSize - 4 );
    if ( count <= 0 )
    {
        // either there was an error or we are at the EOF
        return EOF;
    }
    //else
    //  kdDebug(30002) << "Read " << count << " bytes" << endl;

    /* set the pointer to our buffer
     */
    setg (buffer+(4-sizePutback),       // start of the Putback-zone
	  buffer+4,                     // read position
	  buffer+4+count);              // end of the buffer

    // return the next char
    return *gptr();
}

void ostorestreambuffer::emptybuffer()
{
    int count = pptr()-pbase();
    m_pStore->write( m_buffer, count );
    pbump (-count);    // restore "write-pointer" position
}
