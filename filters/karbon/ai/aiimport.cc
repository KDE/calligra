/* This file is part of the KDE project
   Copyright (C) 2002, Dirk Schönberger <dirk.schoenberger@sz-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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


class AiImportFactory : KGenericFactory<AiImport, KoFilter>
{
public:
	AiImportFactory( void )
		: KGenericFactory<AiImport, KoFilter>( "karbonaiimport" )
	{}

protected:
	virtual void setupTranslations( void )
	{
		KGlobal::locale()->insertCatalogue( "karbonaifilter" );
	}
};

K_EXPORT_COMPONENT_FACTORY( libkarbonaiimport, AiImportFactory() );

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

        QDomDocument doc ("DOC");

        if (!parse (fileIn, doc))
        {
		fileIn.close();
		return KoFilter::CreationError;
        }
	m_result = doc.toString().latin1();

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



