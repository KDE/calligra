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


#include <kdebug.h>
#include <qdom.h>
#include <qfile.h>
#include <koFilterChain.h>
#include <kgenericfactory.h>

#include "wpexport.h"
#include "wpexport.moc"

typedef KGenericFactory<WPExport, KoFilter> WPExportFactory;
K_EXPORT_COMPONENT_FACTORY( libwpexport, WPExportFactory( "wpexport" ) );


WPExport::WPExport(KoFilter *, const char *, const QStringList&) :
                     KoFilter()
{
}

KoFilter::ConversionStatus WPExport::convert( const QCString& from, const QCString& to )
{
  // check for proper conversion
  if ( to != "application/wordperfect" || from != "application/x-kword" )
      return KoFilter::NotImplemented;

  kdDebug() << "KWord WordPerfect filter: sorry, export is not supported now " << endl;

  return KoFilter::NotImplemented;
}
