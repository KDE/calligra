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

#include <string>
#include <list>

#include <polestorage.h>
#include <polestorageio.h>
#include <polestream.h>
#include <polestreamio.h>

using namespace POLE;

Storage::Storage()
{
  io = (StorageIO*) 0L;
  result = Storage::Ok;
}

Storage::~Storage()
{
  close();
  delete io;
}

bool Storage::open( const char* fileName, int m )
{
  // only a few modes accepted
  if( ( m != ReadOnly ) && ( m != WriteOnly ) && ( m != ReadWrite ) )
  {
    result = UnknownError;
    return false;
  }

  io = new StorageIO( this, fileName, m );

  result = io->result;

  return result == Storage::Ok;
}

void Storage::flush()
{
  if( io ) io->flush();
}

void Storage::close()
{
  flush();
}

// list all files and subdirs in current path
std::list<std::string> Storage::listDirectory()
{
  std::list<std::string> entries;

  // sanity check
  if( !io ) return entries;
  if( !io->current_dir ) return entries;

  // sentinel: do nothing if not a directory
  if( !io->current_dir->dir ) return entries;

  // find all children belongs to this directory
  for( unsigned i = 0; i<io->current_dir->children.size(); i++ )
  {
    Entry* e = io->current_dir->children[i];
    if( e ) entries.push_back( e->name );
  }

  return entries;
}

// enters a sub-directory, returns false if not a directory or not found
bool Storage::enterDirectory( const std::string& directory )
{
  // sanity check
  if( !io ) return false;
  if( !io->current_dir ) return false;

  // look for the specified sub-dir
  for( unsigned i = 0; i<io->current_dir->children.size(); i++ )
  {
    Entry* e = io->current_dir->children[i];
    if( e ) if( e->name == directory ) 
      if ( e->dir )
      {
        io->current_dir = e;
        return true;
      }
  }

  return false;
}

// goes up one level (like cd ..)
void Storage::leaveDirectory()
{
  // sanity check
  if( !io ) return;
  if( !io->current_dir ) return;

  Entry* parent = io->current_dir->parent;
  if( parent ) if( parent->dir ) 
    io->current_dir = parent;
}

// note: without trailing "/"
std::string Storage::path()
{
  // sanity check
  if( !io ) return string();
  if( !io->current_dir ) return string();

  return io->fullName( io->current_dir );
}

Stream* Storage::stream( const std::string& name )
{
  // sanity check
  if( !name.length() ) return (Stream*) 0L;
  if( !io ) return (Stream*) 0L;

  // make absolute if necesary
  std::string fullName = name;
  if( name[0] != '/' ) fullName.insert( 0, path() + "/" );

  // find to which entry this stream associates
  Entry* entry =  io->entry( name );
  if( !entry ) return (Stream*) 0L;

  StreamIO* sio = new StreamIO( io, entry );
  Stream* s = new Stream( sio );

  return s;
}
