/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcstring.h>
#include <qstring.h>

#include <kgenericfactory.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <kprocio.h>

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
	if( to != "application/illustrator" || from != "image/x-eps" )
	{
		return KoFilter::NotImplemented;
	}

	// initialize ghostscript:
	KProcIO* proc = new KProcIO();
	*proc
		<< "/usr/bin/gs -q -dNOPAUSE -dSAFER -dNODISPLAY "
		<< " ps2ai.ps " << m_chain->inputFile()
		<< " >" << m_chain->outputFile();

	// start ghostscript:
	proc->start( KProcess::Block );

	// stop ghostview:
	delete( proc );

	return KoFilter::OK;
}

#include "epsimport.moc"

