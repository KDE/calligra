/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>
#include <qcstring.h>
#include <qfile.h>
#include <qstring.h>

#include <kgenericfactory.h>
#include <koFilterChain.h>
#include <koStore.h>

#include <kdebug.h>

#include "aiimport.h"


typedef KGenericFactory<AiImport, KoFilter> AiImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonaiimport, AiImportFactory( "karbonaiimport" ) );


AiImport::AiImport( KoFilter*, const char*, const QStringList& )
	: KoFilter(), m_result ()
{
}

AiImport::~AiImport()
{
}

KoFilter::ConversionStatus
AiImport::convert( const QCString& from, const QCString& to )
{
	if( from != "application/illustrator" || to != "application/x-karbon" )
	{
		return KoFilter::NotImplemented;
	}

	QFile fileIn( m_chain->inputFile() );
	if( !fileIn.open( IO_ReadOnly ) )
	{
		fileIn.close();
		return KoFilter::FileNotFound;
	}

	if( !parse( fileIn ) )
	{
		fileIn.close();
		return KoFilter::CreationError;
	}
	m_result = getHeader() + m_result + getFooter();

kdDebug() << m_result << endl;
	KoStoreDevice* storeOut = m_chain->storageFile( "root", KoStore::Write );
	if( !storeOut )
	{
		fileIn.close();
		return KoFilter::StorageCreationError;
	}

	QCString cStr = m_result.latin1();
	storeOut->writeBlock( cStr, cStr.size() - 1 );

	return KoFilter::OK;
}


void AiImport::gotStartTag (const char *tagName, Parameters& params) {
  QString data;
  data += "<";
  data += tagName;
  data += " ";
  data += getParamList (params);
  data += ">\n";

  m_result += data;
}

void AiImport::gotEndTag (const char *tagName){
  QString data;
  data += "</";
  data += tagName;
  data += ">\n";

  m_result += data;
}

void AiImport::gotSimpleTag (const char *tagName, Parameters& params) {
  QString data;
  data += "<";
  data += tagName;
  data += " ";
  data += getParamList (params);
  data += "/>\n";

  m_result += data;
}


void AiImport::parsingStarted(){
//  QString str = getHeader();
//  m_buffer += str;
//  qDebug ("buffer is\n %s", m_buffer.latin1());

}

void AiImport::parsingFinished(){
//  m_buffer += getFooter();
}

void AiImport::parsingAborted(){
//  qDebug ("parsing aborted called");
  m_result = "";
}

#include "aiimport.moc"



