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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qdom.h>
#include <qcstring.h>
#include <qfile.h>
#include <qstring.h>

#include <kgenericfactory.h>
#include <KoFilterChain.h>
#include <KoStore.h>

#include <kdebug.h>

#include "aiimport.h"
#include "karbonaiparserbase.h"

class AiImportFactory : KGenericFactory<AiImport, KoFilter>
{
public:
	AiImportFactory( void )
		: KGenericFactory<AiImport, KoFilter>( "karbonaiimport" )
	{}

protected:
	virtual void setupTranslations( void )
	{
		KGlobal::locale()->insertCatalogue( "kofficefilters" );
	}
};

K_EXPORT_COMPONENT_FACTORY( libkarbonaiimport, AiImportFactory() )

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

        QDomDocument doc ("DOC");
	KarbonAIParserBase parser;

        if (!parser.parse (fileIn, doc))
        {
		fileIn.close();
		return KoFilter::CreationError;
        }
	QString result = doc.toString();

        kdDebug() << result << endl;
	KoStoreDevice* storeOut = m_chain->storageFile( "root", KoStore::Write );
	if( !storeOut )
	{
		fileIn.close();
		return KoFilter::StorageCreationError;
	}

	QCString cStr = result.latin1();
	storeOut->writeBlock( cStr, cStr.size() - 1 );

	return KoFilter::OK;
}


#include "aiimport.moc"



