/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

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

#include <qcstring.h>
#include <qdatastream.h>
#include <qdom.h>
#include <qfile.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <kgenericfactory.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <koStore.h>

#include "vdocument.h"
#include "vlayer.h"
#include "xcfexport.h"

#include <kdebug.h>


typedef KGenericFactory<XcfExport, KoFilter> XcfExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonxcfexport, XcfExportFactory( "karbonxcfexport" ) );


XcfExport::XcfExport( KoFilter*, const char*, const QStringList& )
	: KoFilter()
{
}

KoFilter::ConversionStatus
XcfExport::convert( const QCString& from, const QCString& to )
{
	if( to != "image/x-xcf-gimp" || from != "application/x-karbon" )
	{
		return KoFilter::NotImplemented;
	}


	KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );

	if( !storeIn )
		return KoFilter::StupidError;


	QFile fileOut( m_chain->outputFile() );

	if( !fileOut.open( IO_WriteOnly ) )
		return KoFilter::StupidError;


	QDomDocument domIn;
	domIn.setContent( storeIn );
	QDomElement docNode = domIn.documentElement();

	m_stream = new QDataStream( &fileOut );


	// Load the document.
	VDocument doc;
	doc.load( docNode );

	// Process the document.
	doc.accept( *this );


	delete m_stream;
	fileOut.close();

	return KoFilter::OK;
}

void
XcfExport::visitVDocument( VDocument& document )
{
	// Write header.

	// Tag.
	m_stream->writeRawBytes( "gimp xcf file", 14 );


	// Export layers.
	VVisitor::visitVDocument( document );
}

void
XcfExport::visitVLayer( VLayer& /*layer*/ )
{

}

#include "xcfexport.moc"

