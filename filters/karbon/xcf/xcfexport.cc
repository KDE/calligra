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


// Tile size constants.
const unsigned XcfExport::m_tileWidth = 64;
const unsigned XcfExport::m_tileHeight = 64;


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
	// Remember width and height for layer saving.
	m_width  = static_cast<unsigned>( document.width() );
	m_height = static_cast<unsigned>( document.height() );


	// Header tag (length 14 bytes).
	m_stream->writeRawBytes( "gimp xcf file", 14 );

	// Image width.
	*m_stream << static_cast<Q_UINT32>( m_width );

	// Image height.
	*m_stream << static_cast<Q_UINT32>( m_height );

	// Image type 0 = RGB.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// Do not save any properties.
	*m_stream
		// "END".
		<< static_cast<Q_UINT32>( 0 )
		// size 0.
		<< static_cast<Q_UINT32>( 0 );


	// Write layer offsets.
	QIODevice::Offset offset = m_stream->device()->at();

	for( unsigned i = 0; i < document.layers().count(); ++i )
	{
		*m_stream << static_cast<Q_UINT32>( offset + ( i + 3 + 2 ) * 4 );
	}
	
	// Append a zero offset to indicate end of layer offset list.
	*m_stream << static_cast<Q_UINT32>( 0 );


	// Append a zero offset to indicate end of channel offset list.
	*m_stream << static_cast<Q_UINT32>( 0 );


	// Export layers.
	VVisitor::visitVDocument( document );
}

void
XcfExport::visitVLayer( VLayer& layer )
{
	// Layer width = image width.
	*m_stream << static_cast<Q_UINT32>( m_width );

	// Layer height = image height.
	*m_stream << static_cast<Q_UINT32>( m_height );

	// Layer type = RGBA.
	*m_stream << static_cast<Q_UINT32>( 1 );

	// Layer name.
	// Size in bytes.
	*m_stream << static_cast<Q_UINT32>( layer.name().length() + 1 );
	// String.
	*m_stream << layer.name().latin1();
	
	// Layer opacity.
	*m_stream << static_cast<Q_UINT32>( 6 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 4 );
	// Fully opaque = 255.
	*m_stream << static_cast<Q_UINT32>( 255 );

	// Layer visible?
	*m_stream << static_cast<Q_UINT32>( 8 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 4 );
	// True.
	*m_stream << static_cast<Q_UINT32>( 1 );

	// Layer linked?
	*m_stream << static_cast<Q_UINT32>( 9 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 4 );
	// False.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// Preserve transparency?
	*m_stream << static_cast<Q_UINT32>( 10 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 4 );
	// False.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// Apply mask?
	*m_stream << static_cast<Q_UINT32>( 11 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 4 );
	// False.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// Edit mask?
	*m_stream << static_cast<Q_UINT32>( 12 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 4 );
	// False.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// Show mask?
	*m_stream << static_cast<Q_UINT32>( 13 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 4 );
	// False.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// Offsets.
	*m_stream << static_cast<Q_UINT32>( 15 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 8 );
	// X-Offset.
	*m_stream << static_cast<Q_UINT32>( 0 );
	// Y-Offset.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// Layer mode.
	*m_stream << static_cast<Q_UINT32>( 7 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 4 );
	// Normal mode.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// TODO: Tattoo.
	*m_stream << static_cast<Q_UINT32>( 20 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 4 );
	// False.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// Layer properties end.
	*m_stream << static_cast<Q_UINT32>( 0 );
	// Property size in bytes.
	*m_stream << static_cast<Q_UINT32>( 0 );


	// Gimp splits the image into tiles ("hierarchy").

	// TODO: Hierarchy offset.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// TODO: Layer mask offset.
	*m_stream << static_cast<Q_UINT32>( 0 );

	// Calculate tile-rows and -columns.
	Q_UINT32 rows    = ( m_height + m_tileHeight - 1 ) / m_tileHeight;
	Q_UINT32 columns = ( m_width  + m_tileWidth - 1 )  / m_tileWidth;

	// Layer depth = RGBA.
	*m_stream << static_cast<Q_UINT32>( 1 );


// TODO: to be continued.
}

#include "xcfexport.moc"
