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
#include <koFilterChain.h>
#include <kgenericfactory.h>

#include <wpimport.h>
#include <kwordfilter.h>

#include <stdio.h> // debug

typedef KGenericFactory<WPImport, KoFilter> WPImportFactory;
K_EXPORT_COMPONENT_FACTORY( libwpimport, WPImportFactory( "wpimport" ) );

WPImport::WPImport( KoFilter *, const char *, const QStringList& ):
                     KoFilter()
{
}

KoFilter::ConversionStatus WPImport::convert( const QCString& from, const QCString& to )
{
  // check for proper conversion
  if( to!= "application/x-kword" || from != "application/wordperfect" )
     return KoFilter::NotImplemented;

  // parse and filter the doc
  WP::KWordFilter filter;
  if ( !filter.parse( m_chain->inputFile() ) ) return KoFilter::FileNotFound;

  QString root = filter.root;
  QString documentInfo = filter.documentInfo;

  // sentinel check
  if( root.isEmpty() ) return KoFilter::StupidError;


  // prepare storage
  KoStore out=KoStore( QString(m_chain->outputFile()), KoStore::Write );

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

  return KoFilter::OK;
}

#include "wpimport.moc"
