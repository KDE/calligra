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

#ifndef KQIODEVICEGZIP_H
#define KQIODEVICEGZIP_H

#include <qiodevice.h>
#include <qstring.h>
#include <zlib.h>


class KQIODeviceGZip : public QIODevice
{
public:
    KQIODeviceGZip(const QString& filename);
    ~KQIODeviceGZip(void);

    bool open(int mode);
    void close(void);
    void flush(void);

    uint size(void) const;
    int  at(void) const;
    bool at(int pos);
    bool atEnd(void) const;
    bool reset (void);

    int readBlock( char *data, uint maxlen );
    int writeBlock( const char *data, uint len );

    int getch(void);
    int putch(int ch);
    int ungetch(int ch);
private:
    gzFile m_gzfile;
    int m_ungetchar;
    QString m_filename;
};


#endif //KQIODEVICEGZIP_H
