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

// (Werner) I've translated the comments - there might be
// some errors, though :(
// (David) Thanks !!!

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
    int anzPutback;
    anzPutback = gptr() - eback();
    if (anzPutback > 4)
    {
        anzPutback = 4;
    }

    /* prepend the chars already read (put them
     * back in the buffer (first 4 chars)
     */
    memcpy( puffer + ( 4 - anzPutback ), gptr()-anzPutback, anzPutback );

    /* read new chars
     */
    long anz = 8192;
    anz = m_pStore->read( puffer + 4, pufferSize - 4 );
    if ( anz <= 0 )
    {
        // either there was an error or we are at the EOF
        return EOF;
    }
    //else
    //  kdDebug(30002) << "Read " << anz << " bytes" << endl;

    /* set the pointer to our buffer
     */
    setg (puffer+(4-anzPutback),   // start of the Putback-zone
	  puffer+4,                    // read position
	  puffer+4+anz);               // end of the buffer

    // return the next char
    unsigned char c = *((unsigned char*)gptr());
    return c;
    // return *gptr();
}

int ostorestreambuffer::emptybuffer()
{
    int anz = pptr()-pbase();
    m_pStore->write( m_buffer, anz );

    pbump (-anz);    // restore "write-pointer" position
    return anz;
}
