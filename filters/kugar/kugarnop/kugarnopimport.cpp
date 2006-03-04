/***************************************************************************
                          kugarnopimport.cpp  -  description
                             -------------------
    copyright            : (C) 2002 by Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License verstion 2 as    *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/

#include "kugarnopimport.h"
#include "kugarnopimport.moc"
#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <qfile.h>
#include "KoDocument.h"
#include <qdom.h>

typedef KGenericFactory<KugarNOPImport, KoFilter> KugarNOPImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkugarnopimport, KugarNOPImportFactory( "kugarnopimport" ) )


KugarNOPImport::KugarNOPImport( KoFilter *, const char *, const QStringList& )
    : KoFilter()
{
}

KoFilter::ConversionStatus KugarNOPImport::convert( const QCString& /*from*/, const QCString& /*to*/ )
{

	QFile in( m_chain->inputFile());
	if (!in.open( IO_ReadOnly ))
	{
	        kdError() << "Unable to open input file!" << endl;
	        in.close();
	        return KoFilter::FileNotFound;
	}
	
	KoDocument *doc=m_chain->outputDocument();
	doc->loadXML(&in,QDomDocument());
	in.close();
	doc->resetURL();

	return KoFilter::OK;
}
