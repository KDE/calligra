/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcstring.h>
#include <qdom.h>
#include <qfile.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qimage.h>

#include <kgenericfactory.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <koStore.h>

#include "pngexport.h"
#include "vdocument.h"
#include "vselection.h"
#include "vkopainter.h"

#include <kdebug.h>


typedef KGenericFactory<PngExport, KoFilter> PngExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonpngexport, PngExportFactory( "karbonpngexport" ) );


PngExport::PngExport( KoFilter*, const char*, const QStringList& )
	: KoFilter()
{
}

KoFilter::ConversionStatus
PngExport::convert( const QCString& from, const QCString& to )
{
	if ( to != "image/png" || from != "application/x-karbon" )
	{
		return KoFilter::NotImplemented;
	}

	KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );
	if( !storeIn )
		return KoFilter::StupidError;

	QDomDocument domIn;
	domIn.setContent( storeIn );
	QDomElement docNode = domIn.documentElement();

	// load the document and export it:
	VDocument doc;
	doc.load( docNode );

	// select all objects:
	doc.selection()->append();

	// get the bounding box of all selected objects:
	const KoRect& rect = doc.selection()->boundingBox();

	// create image with correct width and height
	QImage img( int( rect.width() ), int( rect.height() ), 32 );

	// Create painter and set up objects to draw
	VKoPainter p( img.bits(), rect.width(), rect.height() );
	p.setWorldMatrix( QWMatrix().translate( -rect.x(), -rect.y() ) );
	VObjectList objects = doc.selection()->objects();
	VObjectListIterator itr = objects;

	// we dont need the selection anymore:
	doc.selection()->clear();

	// paint shapes over image
    for ( ; itr.current(); ++itr )
		itr.current()->draw( &p, rect );

	QImage image = img.swapRGB();
	// save png
	image.save( m_chain->outputFile(), "PNG" );

	delete storeIn;

	return KoFilter::OK;
}

#include "pngexport.moc"

