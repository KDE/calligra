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

#ifndef KOLE_STORAGE_H
#define KOLE_STORAGE_H

#include <qstring.h>
#include <qstringlist.h>

class QDataStream;

namespace KOLE
{

class StorageInfo;

class Storage
{

public:

  enum { Ok, OpenFailed, NotOLE, BadOLE, UnknownError };

  Storage();

  ~Storage();

  /**
   * Opens the specified file, using the mode m.
   *
   * @return true if succesful, otherwise false.
   */
  bool open( const QString& filename, int m = IO_ReadOnly );

  /**
   * Flushes the buffer to the disk.
   */
  void flush();

  /**
   * Closes the storage.
   *
   * If it was opened using IO_WriteOnly or IO_ReadWrite, this function
   * also flushes the buffer.
   */
  void close();

  QStringList listDirectory();

  bool enterDirectory( const QString& directory );

  void leaveDirectory();

  QString path();

  /**
   * Creates an input/output stream for specified name. You should not
   * delete the stream because it is owned by the storage.
   *
   * If name does not exist or is a directory, this function
   * will return null.
   */
  QDataStream* stream( const QString& name );

  int result;

protected:

  StorageInfo* info;

private:

  // no copy or assign
  Storage( const Storage& );
  Storage& operator=( const Storage& );

};


}

#endif // KOLE_STORAGE_H
