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

#ifndef KOLE_DEVICE_H
#define KOLE_DEVICE_H

#include <qcstring.h>
#include <qiodevice.h>
#include <qvaluevector.h>

namespace KOLE 
{

class Entry;
class StorageInfo;

class Device: public QIODevice
{
  public:

    Device( StorageInfo*, Entry* );

    ~Device();

    virtual bool open( int mode );

    virtual void close();

    virtual void flush();

    virtual Q_ULONG size() const;

    virtual Q_ULONG at() const;

    virtual bool at( int );

    virtual bool atEnd() const;

    virtual Q_LONG readBlock( char* data, Q_ULONG maxlen );

    virtual Q_LONG writeBlock( const char* data, Q_ULONG len );

    int readLine( char* data, uint maxlen );

    virtual int getch();

    virtual int putch( int );

    virtual int ungetch( int );

  protected:

    virtual Q_LONG readBlock( Q_ULONG p, char* data, Q_ULONG maxlen );

    virtual Q_LONG writeBlock( Q_ULONG p, const char* data, Q_ULONG len );

  private:

    StorageInfo* storageinfo;

    Entry* entry;

    // current stream position
    Q_ULONG pos;

    // chain of occupied blocks (local copy)
    QValueVector<int> blocks;

    // simple cache buffer to speed-up getch()
    QByteArray cache_data;
    Q_ULONG cache_pos;
    void updateCache();

    // when true, flush() before cache update
    bool dirty;

};

}

#endif // KOLE_DEVICE_H
