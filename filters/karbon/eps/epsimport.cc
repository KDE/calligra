/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcstring.h>
#include <qstring.h>

#include <kgenericfactory.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <krun.h>

#include <kdebug.h>

#include "epsimport.h"


class EpsImportFactory : KGenericFactory<EpsImport, KoFilter>
{
public:
	EpsImportFactory( void )
		: KGenericFactory<EpsImport, KoFilter>( "karbonepsimport" )
	{}

protected:
	virtual void setupTranslations( void )
	{
		KGlobal::locale()->insertCatalogue( "karbonepsfilter" );
	}
};

K_EXPORT_COMPONENT_FACTORY( libkarbonepsimport, EpsImportFactory() );

EpsImport::EpsImport( KoFilter*, const char*, const QStringList& )
	: KoFilter()
{
}

EpsImport::~EpsImport()
{
}

KoFilter::ConversionStatus
EpsImport::convert( const QCString& from, const QCString& to )
{
	if(
		to != "application/illustrator" ||
		(
			from != "image/x-eps" &&
			from != "application/postscript" ) )
	{
		return KoFilter::NotImplemented;
	}

	// Copy input filename:
	QString input = m_chain->inputFile();

	// Quote spaces in filename:
	KRun::shellQuote( input );

	// Build ghostscript call to convert ps/eps -> ai:
	QString command = QString(
		"gs -q -dNOPAUSE -dSAFER -dNODISPLAY ps2ai.ps %1 > %2" ).
			arg( input ).arg( m_chain->outputFile() );

	// Execute it:
	if( !system( command.latin1() ) )
		return KoFilter::OK;
	else
		return KoFilter::StupidError;
}

#include "epsimport.moc"

