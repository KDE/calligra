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

#ifndef POLE_STORAGE_H
#define POLE_STORAGE_H

#include <string>
#include <list>

namespace POLE
{

class StorageIO;
class Stream;

class Storage
{

public:

  enum { Ok, OpenFailed, NotOLE, BadOLE, UnknownError, StupidWorkaroundForBrokenCompiler=255 };

  enum { ReadOnly, WriteOnly, ReadWrite };

  Storage();

  ~Storage();

  /**
   * Opens the specified file, using the mode m.
   *
   * @return true if succesful, otherwise false.
   */
  bool open( const char* filename, int m = ReadOnly );

  /**
   * Flushes the buffer to the disk.
   */
  void flush();

  /**
   * Closes the storage.
   *
   * If it was opened using WriteOnly or ReadWrite, this function
   * also flushes the buffer.
   */
  void close();

  std::list<std::string> listDirectory();

  bool enterDirectory( const std::string& directory );

  void leaveDirectory();

  std::string path();

  /**
   * Creates an input/output stream for specified name. You should 
   * delete the stream because it is not owned by the storage.
   *
   * If name does not exist or is a directory, this function
   * will return null.
   */
  Stream* stream( const std::string& name );

  int result;

protected:

  StorageIO* io;

private:

  // no copy or assign
  Storage( const Storage& );
  Storage& operator=( const Storage& );

};


}

#endif // POLE_STORAGE_H
