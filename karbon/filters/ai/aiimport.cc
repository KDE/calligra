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

AiImport::~AiImport()
{
}

KoFilter::ConversionStatus
AiImport::convert( const QCString& from, const QCString& to )
{
	if ( from != "application/illustrator" || to != "application/x-karbon" )
	{
		return KoFilter::NotImplemented;
	}
	QFile fileIn( m_chain->inputFile() );
	if( !fileIn.open( IO_ReadOnly ) )
	{
		fileIn.close();
		return KoFilter::FileNotFound;
	}

        if (!parse (fileIn))
        {
		fileIn.close();
		return KoFilter::CreationError;
        }  

        kdDebug() << m_buffer << endl;
	KoStoreDevice* storeOut = m_chain->storageFile( "root", KoStore::Write );
	if( !storeOut )
	{
		fileIn.close();
		return KoFilter::StorageCreationError;
	}

	QCString cStr = m_buffer.latin1();
	storeOut->writeBlock( cStr, cStr.length() );

	return KoFilter::OK;
}


void AiImport::gotStartTag (const char *tagName, Parameters& params) {
  QString data;
  data += "<";
  data += tagName;
  data += " ";
  data += getParamList (params);
  data += ">\n";

  m_buffer += data;
}

void AiImport::gotEndTag (const char *tagName){
  QString data;
  data += "</";
  data += tagName;
  data += ">\n";

  m_buffer += data;
}

void AiImport::parsingStarted(){
  m_buffer += getHeader();
}

void AiImport::parsingFinished(){
  m_buffer += getFooter();
}

void AiImport::parsingAborted(){
  m_buffer = "";
}

#include "aiimport.moc"



