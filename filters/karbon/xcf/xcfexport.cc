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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <q3cstring.h>
#include <qdatastream.h>
#include <qdom.h>
#include <qfile.h>
#include <qstring.h>
#include <q3valuelist.h>

#include <kgenericfactory.h>
#include <KoFilter.h>
#include <KoFilterChain.h>
#include <KoStore.h>

#include "vdocument.h"
#include "vlayer.h"
#include "xcfexport.h"

#include <kdebug.h>


// Tile size constants.
const unsigned XcfExport::m_tileWidth  = 64;
const unsigned XcfExport::m_tileHeight = 64;


typedef KGenericFactory<XcfExport, KoFilter> XcfExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonxcfexport, XcfExportFactory( "kofficefilters" ) )


XcfExport::XcfExport( KoFilter*, const char*, const QStringList& )
	: KoFilter()
{
	m_zoomX = 1.0;
	m_zoomY = 1.0;
}

KoFilter::ConversionStatus
XcfExport::convert( const QByteArray& from, const QByteArray& to )
{
	if( to != "image/x-xcf-gimp" || from != "application/x-karbon" )
	{
		return KoFilter::NotImplemented;
	}


	KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );

	if( !storeIn )
		return KoFilter::StupidError;


	QFile fileOut( m_chain->outputFile() );

	if( !fileOut.open( QIODevice::WriteOnly ) )
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
	// Offsets.
	qlonglong current = 0;
	qlonglong start = 0;
	qlonglong end = 0;

	// Save width and height for layer saving.
	m_width  = static_cast<unsigned>( document.width()  * m_zoomX );
	m_height = static_cast<unsigned>( document.height() * m_zoomY );


	// Header tag (size 14 bytes).
	m_stream->writeRawBytes( "gimp xcf file", 14 );

	// Image width.
	*m_stream << static_cast<quint32>( m_width );

	// Image height.
	*m_stream << static_cast<quint32>( m_height );

	// Image type = RGB.
	*m_stream << static_cast<quint32>( 0 );

	// Do not save any properties currently.
	*m_stream
		// "END".
		<< static_cast<quint32>( 0 )
		// Property size in bytes.
		<< static_cast<quint32>( 0 );


	// Save current offset.
	current = m_stream->device()->at();

	// Leave space for layer and channel offsets.
	m_stream->device()->at(
		// current position + (number layers + number channels + 2) * 4.
		current + ( document.layers().count() + 3 + 2 ) * 4 );


	// Iterate over layers.
	VLayerListIterator itr( document.layers() );

	for( ; itr.current(); ++itr )
	{
		// Save start offset.
		start = m_stream->device()->at();


		// Write layer.
		itr.current()->accept( *this );


		// Save end offset.
		end = m_stream->device()->at();

		// Return to current offset.
		m_stream->device()->at( current );

		// Save layer offset.
		*m_stream << start;

		// Increment offset.
		current = m_stream->device()->at();

		// Return to end offset.
		m_stream->device()->at( end );
	}


	// Return to current offset.
	m_stream->device()->at( current );

	// Append a zero offset to indicate end of layer offsets.
	*m_stream << static_cast<quint32>( 0 );


	// Return to end offset.
	m_stream->device()->at( end );

	// Append a zero offset to indicate end of channel offsets.
	*m_stream << static_cast<quint32>( 0 );
}

void
XcfExport::visitVLayer( VLayer& layer )
{
	// Layer width = image width.
	*m_stream << static_cast<quint32>( m_width );

	// Layer height = image height.
	*m_stream << static_cast<quint32>( m_height );

	// Layer type = RGBA.
	*m_stream << static_cast<quint32>( 1 );

	// Layer name.
	*m_stream << layer.name().latin1();

	// Layer opacity.
	*m_stream << static_cast<quint32>( 6 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 4 );
	// Fully opaque = 255.
	*m_stream << static_cast<quint32>( 255 );

	// Layer visible?
	*m_stream << static_cast<quint32>( 8 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 4 );
	// True.
	*m_stream << static_cast<quint32>( 1 );

	// Layer linked?
	*m_stream << static_cast<quint32>( 9 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 4 );
	// False.
	*m_stream << static_cast<quint32>( 0 );

	// Preserve transparency?
	*m_stream << static_cast<quint32>( 10 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 4 );
	// False.
	*m_stream << static_cast<quint32>( 0 );

	// Apply mask?
	*m_stream << static_cast<quint32>( 11 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 4 );
	// False.
	*m_stream << static_cast<quint32>( 0 );

	// Edit mask?
	*m_stream << static_cast<quint32>( 12 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 4 );
	// False.
	*m_stream << static_cast<quint32>( 0 );

	// Show mask?
	*m_stream << static_cast<quint32>( 13 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 4 );
	// False.
	*m_stream << static_cast<quint32>( 0 );

	// Layer offsets.
	*m_stream << static_cast<quint32>( 15 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 8 );
	// X-Offset.
	*m_stream << static_cast<quint32>( 0 );
	// Y-Offset.
	*m_stream << static_cast<quint32>( 0 );

	// Layer mode.
	*m_stream << static_cast<quint32>( 7 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 4 );
	// Normal mode.
	*m_stream << static_cast<quint32>( 0 );

	// TODO: Tattoo.
	*m_stream << static_cast<quint32>( 20 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 4 );
	// False.
	*m_stream << static_cast<quint32>( 0 );

	// Layer properties end.
	*m_stream << static_cast<quint32>( 0 );
	// Property size in bytes.
	*m_stream << static_cast<quint32>( 0 );


	// Offsets.
	qlonglong current = 0;
	qlonglong start = 0;
	qlonglong end = 0;

	// Save current offset.
	current = m_stream->device()->at();

	// Leave space for hierarchy offsets.
	m_stream->device()->at( current + 8 );

	// Save start offset.
	start = m_stream->device()->at();


	// Write hierarchy.
	writeHierarchy();


	// Save end offset.
	end = m_stream->device()->at();

	// Return to current offset.
	m_stream->device()->at( current );

	// Save hierarchy offset.
	*m_stream << start;


	// Append a zero offset to indicate end of layer mask offsets.
	*m_stream << static_cast<quint32>( 0 );
}

void
XcfExport::writeHierarchy()
{
	// Offsets.
	qlonglong current = 0;
	qlonglong start = 0;
	qlonglong end = 0;

	// Width (again?).
	*m_stream << m_width;

	// Height (again?).
	*m_stream << m_height;

	// Color depth.
	*m_stream << static_cast<quint32>( 3 );


	// Calculate level number.
	int levX = levels( m_width, m_tileWidth );
	int levY = levels( m_height, m_tileHeight );
	int levels = qMax( levX, levY );

	int width = m_width;
	int height = m_height;

	// Save current offset.
	current = m_stream->device()->at();

	// Leave space for level offsets.
	m_stream->device()->at( current + ( levels + 1 ) * 4 );

	for( int i = 0; i < levels; ++i )
	{
		// Save start offset.
		start = m_stream->device()->at();

		if( i == 0 )
		{
			// Write level.
			writeLevel();
		}
		else
		{
			// Fake an empty level.
			width  /= 2;
			height /= 2;

			*m_stream << static_cast<quint32>( width );
			*m_stream << static_cast<quint32>( height );
			*m_stream << static_cast<quint32>( 0 );
		}

		// Save end offset.
		end = m_stream->device()->at();

		// Return to current offset.
		m_stream->device()->at( current );

		// Save level offset.
		*m_stream << start;

		// Increment offset.
		current = m_stream->device()->at();

		// Return to end offset.
		m_stream->device()->at( end );
	}

	// Return to current offset.
	m_stream->device()->at( current );

	// Append a zero offset to indicate end of level offsets.
	*m_stream << static_cast<quint32>( 0 );
}

void
XcfExport::writeLevel()
{
	// Offsets.
	qlonglong current = 0;
	qlonglong start = 0;
	qlonglong end = 0;

	*m_stream << static_cast<quint32>( m_width );
	*m_stream << static_cast<quint32>( m_height );

	int rows = ( m_height + m_tileHeight - 1 ) / m_tileHeight;
	int cols = ( m_width  + m_tileWidth  - 1 ) / m_tileWidth;
	int tiles = rows * cols;

	// Save current offset.
	current = m_stream->device()->at();

	// Leave space for tile offsets.
	m_stream->device()->at( current + ( tiles + 1 ) * 4 );

	for( int i = 0; i < tiles; ++i )
	{
		// Save start offset.
		start = m_stream->device()->at();


		// TODO: Save tile.
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );
		*m_stream << static_cast<quint8>( 1 );


		// Save end offset.
		end = m_stream->device()->at();

		// Return to current offset.
		m_stream->device()->at( current );

		// Save tile offset.
		*m_stream << start;

		// Increment offset.
		current = m_stream->device()->at();

		// Return to end offset.
		m_stream->device()->at( end );
	}
}

int
XcfExport::levels( int layerSize, int tileSize )
{
	int l = 1;

	while( layerSize > tileSize )
	{
		layerSize /= 2;
		l += 1;
	}

	return l;
}

#include "xcfexport.moc"
