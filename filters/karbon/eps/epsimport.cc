/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qcstring.h>
#include <qfile.h>
#include <qstring.h>

#include <kgenericfactory.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <koStore.h>
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
EpsImport::convert( const QCString& from, const QCString& /*to*/ )
{
kdDebug() << "****0" << endl;

	if( from != "image/x-eps" )
//	if( to != "application/illustrator" || from != "image/x-eps" )
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

kdDebug() << "****1" << endl;

	// initialize ghostscript:
	KProcIO* proc = new KProcIO();
	*proc << "gs - -q -dNOPAUSE -dSAFER -dNODISPLAY ps2ai.ps";

kdDebug() << "****2" << endl;

	// start ghostscript:
	proc->start( KProcess::DontCare, KProcess::All );

kdDebug() << "****3" << endl;

	// write data to ghostscript:
	proc->fputs( byteArrayIn );
	proc->closeWhenDone();

	QString line;

kdDebug() << "****4" << endl;

	while( proc->readln( line, true ) )
	{
kdDebug() << line << endl;
	}

	// stop ghostview:
	delete( proc );

	fileOut.close();
	delete storeIn;
	return KoFilter::OK;
}

#include "epsimport.moc"

