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

#ifndef __koStore_impl_h__
#define __koStore_impl_h__

#include <fstream>
#include <qstring.h>
#include <qmap.h>

#include "koStore.h"

class KoBinaryStore : public KoStore
{
public:
  KoBinaryStore( const QString & _filename, Mode _mode );
  virtual ~KoBinaryStore();

  virtual bool open( const QString & name, const QCString &_mime_type = "" );
  virtual void close();
  virtual QByteArray read( unsigned long int max );
  virtual bool write( const QByteArray& _data );

  bool write( const char* _data, unsigned long _len );
  long read( char *_buffer, unsigned long _len );

  // void list();

  bool bad() { return m_out.bad(); }

protected:
  Mode m_mode;

  struct Entry
  {
    QCString mimetype;
    QCString name;
    unsigned int size;
    unsigned int pos;
    unsigned int data;
  };

  void writeHeader( const KoBinaryStore::Entry& _entry );
  unsigned long readHeader( KoBinaryStore::Entry& _entry );
  void putULong( unsigned long x );
  unsigned long getULong();

  QMap<QString, Entry> m_map;

  std::ofstream m_out;
  std::ifstream m_in;
  bool m_bIsOpen;

  Entry m_current;
  int m_readBytes;

  int m_id;
};

#endif
