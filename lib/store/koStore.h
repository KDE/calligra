/* This file is part of the KDE project
   Copyright (C) 1998, 1999 David Faure <faure@kde.org>

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

#ifndef __koStore_h_
#define __koStore_h_

#include "koStoreIf.h"

// Base class for KoTarStore and KoBinaryStore
// Needed for the write( const char*, long ) and read(char*, long) methods
class KoStore : public KOStore::Store_skel
{
protected: // interface, not real class
  KoStore::KoStore() {}
public:
  virtual ~KoStore() {}

  virtual bool write( const char* _data, unsigned long _len ) = 0L;
  virtual long read( char *_buffer, unsigned long _len ) = 0L;
  virtual bool bad() = 0L;
};

#endif
