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
		"%%Creator: Karbon14\n"
		"%%BoundingBox: 0 0 100 100" << endl;

	// defs:
	s <<
		"/m {moveto} def\n"
		"/c {curveto} def\n"
		"/l {lineto} def" << endl;


	fileOut.close();
	return KoFilter::OK;
}

#include "epsexport.moc"

