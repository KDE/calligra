/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcstring.h>
#include <qdom.h>
#include <qfile.h>
#include <qstring.h>

#include <kgenericfactory.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <koStore.h>

#include <kdebug.h>

#include "epsexport.h"
#include "vdocument.h"


class EpsExportFactory : KGenericFactory<EpsExport, KoFilter>
{
public:
	EpsExportFactory( void )
		: KGenericFactory<EpsExport, KoFilter>( "karbonepsexport" )
	{}

protected:
	virtual void setupTranslations( void )
	{
		KGlobal::locale()->insertCatalogue( "karbonepsfilter" );
	}
};

K_EXPORT_COMPONENT_FACTORY( libkarbonepsexport, EpsExportFactory() );

EpsExport::EpsExport( KoFilter*, const char*, const QStringList& )
	: KoFilter()
{
}

KoFilter::ConversionStatus
EpsExport::convert( const QCString& from, const QCString& to )
{
	if ( to != "image/x-eps" || from != "application/x-karbon" )
	{
		return KoFilter::NotImplemented;
	}

	KoStore* storeIn = KoStore::createStore( m_chain->inputFile(), KoStore::Read );
	if( !storeIn->open( "root" ) )
	{
		delete storeIn;
		return KoFilter::StupidError;
	}

	QFile fileOut( m_chain->outputFile() );
	if( !fileOut.open( IO_WriteOnly ) ) {
		delete storeIn;
		return KoFilter::StupidError;
	}

	QByteArray byteArrayIn = storeIn->read( storeIn->size() );
	storeIn->close();

	QDomDocument domIn;
	domIn.setContent( byteArrayIn );
	QDomElement docNode = domIn.documentElement();

	QTextStream s( &fileOut );

	VDocument doc;
	doc.load( docNode );

	// find the bounding box of all objects:
	doc.select();
	const KoRect& rect = doc.selection().boundingBox();

	// header:
	s <<
		"%!PS-Adobe-2.0\n"
		"%%Creator: Karbon14 Exportfilter\n"
		"%%BoundingBox: "
		<< rect.left() << " "
		<< rect.top()  << " "
		<< rect.right() << " "
		<< rect.bottom()
	<< endl;

	// defs:
	s <<
		"/d /def load def\n"
		"/D {bind d} bind d\n"
		"/m {moveto} D\n"
		"/c {curveto} D\n"
		"/l {lineto} D\n"
		"/s {stroke} D\n"
	<< endl;


	// parse dom-tree:
	QDomNodeList list = domIn.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "DOC" )
				exportDocument( s, e );
		}
	}

	fileOut.close();
	delete storeIn;
	return KoFilter::OK;
}

void
EpsExport::exportDocument( QTextStream& s, const QDomElement& node )
{
	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "LAYER" )
				exportLayer( s, e );
		}
	}
}

void
EpsExport::exportLayer( QTextStream& s, const QDomElement& node )
{
	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "PATH" )
				exportPath( s, e );
		}
	}
}

void
EpsExport::exportPath( QTextStream& s, const QDomElement& node )
{

	s << "newpath" << endl;

	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "SEGMENTS" )
				exportSegments( s, e );
		}
	}

	if( node.attribute( "closed" ) != 0 )
		s << "closepath" << endl;

	s << "stroke" << endl;
}

void
EpsExport::exportSegments( QTextStream& s, const QDomElement& node )
{
	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "CURVE" )
			{
				s <<
					e.attribute( "x1" ) << " " <<
					e.attribute( "y1" ) << " " <<
					e.attribute( "x2" ) << " " <<
					e.attribute( "y2" ) << " " <<
					e.attribute( "x3" ) << " " <<
					e.attribute( "y3" ) << " c" << endl;
			}
			else if( e.tagName() == "LINE" )
			{
				s <<
					e.attribute( "x" ) << " " <<
					e.attribute( "y" ) << " l" << endl;
			}
			else if( e.tagName() == "MOVE" )
			{
				s <<
					e.attribute( "x" ) << " " <<
					e.attribute( "y" ) << " m" << endl;
			}
		}
	}
}

#include "epsexport.moc"

