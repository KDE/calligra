// $Header$

/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

// TODO: more error report and control

#include <qstring.h>
#include "kqiodevicegzip.h"

KQIODeviceGZip::KQIODeviceGZip(const QString& filename)
{
    m_gzfile=0;
    m_ungetchar=-1;
    m_filename=filename;
    setFlags(IO_Sequential); // We have no direct access, so it is sequential!
    // NOTE: "sequential" also means that you cannot use size()
}

KQIODeviceGZip::~KQIODeviceGZip(void)
{
    if (m_gzfile)
        close();
}

bool KQIODeviceGZip::open(int mode)
{
    if (m_gzfile)
        close(); // One file is already open, so close it first.
    if (m_filename.isEmpty())
        return false; // No file name, cannot open!

    if (IO_ReadOnly==mode)
    {
        m_gzfile=gzopen(m_filename.local8Bit(),"rb");
    }
    else if (IO_WriteOnly==mode)
    {
        m_gzfile=gzopen(m_filename.local8Bit(),"wb9"); // Always set best compression
    }
    else
    {
        // We only support read only or write only, nothing else!
        return false;
    }
    return (m_gzfile!=0);
}

void KQIODeviceGZip::close(void)
{
    if (m_gzfile)
    {
        gzclose(m_gzfile);
        m_gzfile=0;
    }
}

void KQIODeviceGZip::flush(void)
{
    // Always try to flush, do not return any error!
    if (m_gzfile)
    {
        gzflush(m_gzfile,Z_SYNC_FLUSH);
    }
}

uint KQIODeviceGZip::size(void) const
{
    return 0; // You cannot determine size!
}

int  KQIODeviceGZip::at() const
{
    if (!m_gzfile)
        return 0;
    return gztell(m_gzfile);
}

bool KQIODeviceGZip::at(int pos)
{
    if (!m_gzfile)
        return false;
    return (gzseek(m_gzfile,pos,SEEK_SET)>=0);
}

bool KQIODeviceGZip::atEnd() const
{
    if (!m_gzfile)
        return true;
    return gzeof(m_gzfile);
}

bool KQIODeviceGZip::reset(void)
{
    if (!m_gzfile)
        return false; //Say we arew at end of file
    return (gzrewind(m_gzfile)>=0);
}

int KQIODeviceGZip::readBlock( char *data, uint maxlen )
{
    int result=0;
    if (m_gzfile)
    {
        result=gzread(m_gzfile,data,maxlen);
        if (result<0) result=0;
    }
    return result;
}

int KQIODeviceGZip::writeBlock( const char *data, uint len )
{
    int result=0;
    if (m_gzfile)
    {
        result=gzwrite(m_gzfile,(char*)data,len);
    }
    return result;
}

int KQIODeviceGZip::getch()
{
    if (m_ungetchar>0)
    {
        const int ch=m_ungetchar;
        m_ungetchar=-1;
        return ch;
    }
    if (!m_gzfile)
        return -1;
    return gzgetc(m_gzfile);
}

int KQIODeviceGZip::putch( int ch)
{
    if (!m_gzfile)
        return -1;
    return gzputc(m_gzfile,ch);
}

int KQIODeviceGZip::ungetch(int ch)
{
    m_ungetchar=ch;
    return ch;
}
