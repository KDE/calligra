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

#include <qdatastream.h>
#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>

#include <kolestorage.h>
#include <kolestorageinfo.h>
#include <koledevice.h>

using namespace KOLE;

Storage::Storage()
{
  info = (StorageInfo*) 0L;
  result = Storage::Ok;
}

Storage::~Storage()
{
  close();
  delete info;
}

bool Storage::open( const QString& fileName, int m )
{
  // only a few modes accepted
  if( ( m != IO_ReadOnly ) && ( m != IO_WriteOnly ) && ( m != IO_ReadWrite ) )
  {
    result = UnknownError;
    return false;
  }

  info = new StorageInfo( this, fileName, m );

  result = info->result;

  return result == Storage::Ok;
}

void Storage::flush()
{
  if( info ) info->flush();
}

void Storage::close()
{
  if( info ) info->flush();
}

// list all files and subdirs in current path
QStringList Storage::listDirectory()
{
  QStringList entries;

  // sanity check
  if( !info ) return entries;
  if( !info->current_dir ) return entries;

  // sentinel: do nothing if not a directory
  if( !info->current_dir->dir ) return entries;

  // find all children belongs to this directory
  for( unsigned i = 0; i<info->current_dir->children.count(); i++ )
  {
    Entry* e = info->current_dir->children.at( i );
    if( e ) entries.append( e->name );
  }

  return entries;
}

// enters a sub-directory, returns false if not a directory or not found
bool Storage::enterDirectory( const QString& directory )
{
  // sanity check
  if( !info ) return false;
  if( !info->current_dir ) return false;

  // look for the specified sub-dir
  for( unsigned i = 0; i<info->current_dir->children.count(); i++ )
  {
    Entry* e = info->current_dir->children.at( i );
    if( e ) if( e->name == directory ) 
      if ( e->dir )
      {
        info->current_dir = e;
        return true;
      }
  }

  return false;
}

// goes up one level (like cd ..)
void Storage::leaveDirectory()
{
  // sanity check
  if( !info ) return;
  if( !info->current_dir ) return;

  Entry* parent = info->current_dir->parent;
  if( parent ) if( parent->dir ) 
    info->current_dir = parent;
}

// note: without trailing "/"
QString Storage::path()
{
  // sanity check
  if( !info ) return QString::null;
  if( !info->current_dir ) return QString::null;

  return info->fullName( info->current_dir );
}

QDataStream* Storage::stream( const QString& name )
{
  // sanity check
  if( name.isEmpty() ) return (QDataStream*) 0L;
  if( !info ) return (QDataStream*) 0L;

  // make absolute if necesary
  QString fullName = name;
  if( name[0] != '/' ) fullName.prepend( path() + "/" );

  // find to which entry this stream associates
  Entry* entry =  info->entry( name );
  if( !entry ) return (QDataStream*) 0L;

  QDataStream* ds = new QDataStream( new KOLE::Device( info, entry ) );

  // keep a list of all streams
  info->streams.append( ds );

  // anything OLE is alyways little endian
  ds->setByteOrder( QDataStream::LittleEndian );

  return ds;
}
