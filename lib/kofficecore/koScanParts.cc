/* This file is part of the KDE libraries
    Copyright (C) 1998 Torben Weis <weis@kde.org>

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

#include "koScanParts.h"
#include "koIMR.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>    
#include <sys/stat.h>

#include <ksimpleconfig.h>
#include <klocale.h>
#include <kapp.h>

void koScanPartsError( const char *_file, const char *_entry )
{
  warning( i18n( "The OpenParts config file\n%s\ndoes not conatain a %s=... entry" ), _file, _entry );
}

void koScanParts()
{
  CORBA::Object_var obj = opapp_orb->resolve_initial_references ("ImplementationRepository");
  CORBA::ImplRepository_var imr = CORBA::ImplRepository::_narrow( obj );
  assert( !CORBA::is_nil( imr ) );
  
  koScanParts( imr );
}

void koScanParts( CORBA::ImplRepository_ptr _imr )
{
  QString p = kapp->kde_datadir().copy();
  p += "/koffice/partlnk";
  koScanParts( p, _imr );
}

void koScanParts( const char* _path, CORBA::ImplRepository_ptr _imr )
{   
  if ( g_plstPartEntries == 0L )
    g_plstPartEntries = new QList<KoPartEntry>;

  DIR *dp;
  struct dirent *ep;
  dp = opendir( _path );
  if ( dp == 0L )
    return;
  
  // Loop thru all directory entries
  while ( ( ep = readdir( dp ) ) != 0L )
  {
    if ( strcmp( ep->d_name, "." ) != 0 && strcmp( ep->d_name, ".." ) != 0 )
    {
      QString tmp = ep->d_name;
      QString file = _path;
      file += "/";
      file += ep->d_name;
      struct stat buff;
      stat( file.data(), &buff );
      if ( S_ISDIR( buff.st_mode ) )
      {
	koScanParts( file, _imr );
      }
      else if ( tmp.length() > 7 && tmp.right( 7 ) == ".kdelnk" )
      {
	FILE *f = fopen( file, "r" );
	if ( f == 0L )
	{
	  fclose( f );
	  continue;
	}
	
	koScanPartFile( file, _imr );
      }
    }
  }
  
  closedir( dp );
}

void koScanPartFile( const char* _file, CORBA::ImplRepository_ptr _imr )
{
  KSimpleConfig config( _file, true );
  config.setGroup( "KDE Desktop Entry" );
  
  QString type = config.readEntry( "Type" );
  if ( type.isEmpty() )
  {
    koScanPartsError( _file, "Type" );
    return;
  }
  if ( type != "OpenPart" )
  {
    warning("Not an open part\n");
    return;
  }
  
  QString cmd = config.readEntry( "Exec" );
  if ( cmd.isEmpty() )
  {
    koScanPartsError( _file, "Exec" );
    return;
  }
  QStrList repoids;
  if ( config.readListEntry( "RepoID", repoids ) == 0 )
  {
    koScanPartsError( _file, "RepoID" );
    return;
  }
  QStrList mimes;
  if ( config.readListEntry( "MimeTypes", mimes ) == 0 )
  {
    koScanPartsError( _file, "MimeTypes" );
    return;
  }
  QString name = config.readEntry( "Name" );
  if ( name.isEmpty() )
  {
    QString tmp( _file );
    int i = tmp.findRev( "/" );
    if ( i == -1 )
      return;
    name = tmp.right( tmp.length() - i - 1 );
    if ( name.isEmpty() )
      return;
  }
  
  QString comment = config.readEntry( "Comment" );
  if ( comment.isEmpty() )
    comment = name.data();
  QString mode = config.readEntry( "ActivationMode" );
  if ( mode.isEmpty() )
  {
    koScanPartsError( _file, "ActivationMode" );
    return;
  }
  
  imr_create( name, mode, cmd, repoids, _imr );

  g_plstPartEntries->append( new KoPartEntry( name, cmd, mode, comment, mimes, repoids ) );
}


KoPartEntry::KoPartEntry( const char *_name, const char *_exec, const char *_mode, const char *_comment,
		      QStrList& _mimes, QStrList& _repos )
{
  m_strName = _name;
  m_strExec = _exec;
  m_strActivationMode = _mode;
  m_strComment = _comment;
  m_strlstMimeTypes = _mimes;
  m_strlstRepoID = _repos;
}

QList<KoPartEntry> *g_plstPartEntries = 0L;
