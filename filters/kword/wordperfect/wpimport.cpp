/* This file is part of the KDE project
   Copyright (C) 2001 Ariya Hidayat <ariyahidayat@yahoo.de>

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

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <kdebug.h>

#include "wpimport.h"
#include "wpimport.moc"

#include "kwordfilter.h"

#include <stdio.h> // debug

WPImport::WPImport( KoFilter *parent, const char *name ):
                     KoFilter(parent, name)
{
}

// this is the main beast
bool WPImport::filter(const QString &fileIn, const QString &fileOut,
                         const QString& from, const QString& to,
                         const QString &)
{
  // check for proper conversion
  if( to!= "application/x-kword" || from != "application/wordperfect" )
     return false;

  // parse and filter the doc
  WP::KWordFilter filter;
  if ( !filter.parse( fileIn ) ) return false;

  QString root = filter.root;
  QString documentInfo = filter.documentInfo;

  // sentinel check
  if( root.isEmpty() ) return false;

  // prepare storage
  KoStore out=KoStore( QString(fileOut), KoStore::Write );

  // store output document
  if( out.open( "root" ) )
    {
      QCString cstring = root.utf8();
      cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );

      out.write( (const char*) cstring, cstring.length() );
      out.close();

    }

  // store document info
  if( !documentInfo.isEmpty() )
      if( out.open( "documentinfo.xml" ) )
      {
	  QCString cstring = documentInfo.utf8();
	  cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );

	  out.write( (const char*) cstring, cstring.length() );
	  out.close();
      }

  return true;
}





