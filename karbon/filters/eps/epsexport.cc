/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>
#include <qcstring.h>
#include <qfile.h>
#include <qstring.h>

#include <kgenericfactory.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <koStore.h>

#include <kdebug.h>

#include "epsexport.h"


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

	KoStore koStoreIn( m_chain->inputFile(), KoStore::Read );
	if( !koStoreIn.open( "root" ) )
	{
		koStoreIn.close();
		kdError() << "Unable to open input file!" << endl;
		return KoFilter::StupidError;
	}

	QFile fileOut( m_chain->outputFile() );
	if( !fileOut.open( IO_WriteOnly ) ) {
		kdError() << "Unable to open output file: " << m_chain->outputFile() << endl;
		return KoFilter::StupidError;
	}

	QByteArray byteArrayIn = koStoreIn.read( koStoreIn.size() );
	koStoreIn.close();

	QDomDocument domIn;
	domIn.setContent( byteArrayIn );
	QDomElement docNode = domIn.documentElement();

	QTextStream s( &fileOut );


	// header:
	s <<
		"%!PS-Adobe-2.0\n"
		"%%Creator: Karbon14 0.0.1\n"
		"%%BoundingBox: 0 0 800 800" << endl;	// TODO

	// defs:
	s <<
		"/m {moveto} def\n"
		"/c {curveto} def\n"
		"/l {lineto} def" << endl;


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

