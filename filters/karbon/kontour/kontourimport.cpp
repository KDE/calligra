/* This file is part of the KDE project
   Copyright (C) 2002, Sven Lüppken <sven@kde.org>

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

#include <kontourimport.h>
#include <koFilterChain.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <koUnit.h>
#include <koGlobal.h>
#include <qcolor.h>

typedef KGenericFactory<KontourImport, KoFilter> KontourImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonkontourimport, KontourImportFactory( "karbonkontourimport" ) );

KontourImport::KontourImport(KoFilter *, const char *, const QStringList&) :
    KoFilter(),
    outdoc( "DOC" )
{
}

KontourImport::~KontourImport()
{

}

KoFilter::ConversionStatus KontourImport::convert(const QCString& from, const QCString& to)
{
	// check for proper conversion
	if ( to != "application/x-karbon" || ( from != "application/x-kontour" && from != "application/x-killustrator") )
		return KoFilter::NotImplemented;


	KoStoreDevice* inpdev = m_chain->storageFile( "root", KoStore::Read );
	if ( !inpdev )
	{
		kdError(30502) << "Unable to open input stream" << endl;
		return KoFilter::StorageCreationError;
	}
	
	inpdoc.setContent( inpdev );
		            
	// Do the conversion!

	convert();
	
	KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
	if(!out) 
	{
		kdError(30502) << "Unable to open output file!" << endl;
		return KoFilter::StorageCreationError;
	}
	QCString cstring = outdoc.toCString(); // utf-8 already
	out->writeBlock( cstring.data(), cstring.length() );

	return KoFilter::OK;
                                     // was successfull
}

void KontourImport::convert()
{	
	QDomElement docElem = inpdoc.documentElement();
	QDomElement page = docElem.namedItem( "page" ).toElement();
    QDomElement paper = page.namedItem( "layout" ).toElement();
	
	
	QDomElement lay = page.namedItem( "layer" ).toElement();
	
	QDomElement b = lay.firstChild().toElement();
	for( ; !b.isNull(); b = b.nextSibling().toElement() )
	{       
		if ( b.tagName() == "rectangle" )
			{
				int x = b.attribute( "x" ).toInt();
				int y = b.attribute( "y" ).toInt();
				int width = b.attribute( "width" ).toInt();
				int height = b.attribute( "height" ).toInt();
				m_document.append( new VRectangle( 0L, KoPoint( x, y ) , width, height ) );
			}
        }
	
	
	m_document.saveXML( outdoc );
}

#include <kontourimport.moc>
