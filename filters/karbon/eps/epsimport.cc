/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

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

#include <qcstring.h>
#include <qstring.h>
#include <qfile.h>

#include <kgenericfactory.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <krun.h>
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

 	QString command(
 		"gs -q -dNOPAUSE -dSAFER -dNODISPLAY ps2ai.ps ");
 	command += KShellProcess::quote(input);
 	command += " > ";
 	command += KShellProcess::quote(m_chain->outputFile());

 	qDebug ("command to execute is (%s)", QFile::encodeName(command).data());

  	// Execute it:
 	if( !system( QFile::encodeName(command)) )
		return KoFilter::OK;
	else
		return KoFilter::StupidError;
}

#include "epsimport.moc"

