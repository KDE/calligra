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
#include <kprocess.h>

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
	stopProc();
}

KoFilter::ConversionStatus
EpsImport::convert( const QCString& from, const QCString& to )
{
	if ( to != "application/illustrator" || from != "image/x-eps" )
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

	// initialize ghostscript:
	m_proc = new KProcess();
	*m_proc << "gs - -q -dNOPAUSE -dSAFER -dNODISPLAY ps2ai.ps";

	connect( m_proc, SIGNAL( processExited( KProcess* ) ),
		this, SLOT( procExited( KProcess* ) ) );
	connect( m_proc, SIGNAL( receivedStdout( KProcess*, char*, int ) ),
		this, SLOT( procOutput( KProcess*, char*, int ) ) );

	// start ghostscript:
	m_proc->start( KProcess::DontCare, KProcess::All );

	// write data to ghostscript:
	m_proc->writeStdin( byteArrayIn, byteArrayIn.size() );
	m_proc->closeStdin();

	fileOut.close();
	delete storeIn;
	return KoFilter::OK;
}

void
EpsImport::procOutput( KProcess* proc, char* buffer, int len)
{
	QString line = QString::fromLocal8Bit( buffer, len );

kdDebug() << line << endl;
}

void
EpsImport::procFailed()
{
	stopProc();
}

void
EpsImport::procExited( KProcess* proc )
{
	delete m_proc;
}

void
EpsImport::stopProc()
{
	delete( m_proc );
	m_proc = 0L;
}

#include "epsimport.moc"

