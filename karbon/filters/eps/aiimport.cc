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

#include "aiimport.h"


class AiImportFactory : KGenericFactory<AiImport, KoFilter>
{
public:
	AiImportFactory( void )
		: KGenericFactory<AiImport, KoFilter>( "karbonaiimport" )
	{}

protected:
	virtual void setupTranslations( void )
	{
		KGlobal::locale()->insertCatalogue( "karbonepsfilter" );
	}
};

K_EXPORT_COMPONENT_FACTORY( libkarbonaiimport, AiImportFactory() );

AiImport::AiImport( KoFilter*, const char*, const QStringList& )
	: KoFilter()
{
}

KoFilter::ConversionStatus
AiImport::convert( const QCString& from, const QCString& to )
{
	if ( from != "application/illustrator" || to != "application/x-karbon" )
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


	fileOut.close();
	return KoFilter::OK;
}


#include "aiimport.moc"

