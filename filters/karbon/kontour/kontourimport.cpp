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
    outdoc.appendChild( outdoc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );
            
	// Do the conversion!

	convert();
	kdDebug() << outdoc.toString() << endl;
    //return KoFilter::NotImplemented; // Change to KoFilter::OK if the conversion
    KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
    if(!out) {
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
	QDomElement karbondoc = outdoc.createElement( "DOC" );
	karbondoc.setAttribute( "editor", "karbon converter" );
    karbondoc.setAttribute( "mime", "application/x-karbon" );
    karbondoc.setAttribute( "syntaxVersion", 0.1 );
    outdoc.appendChild( karbondoc );
	
	QDomElement docElem = inpdoc.documentElement();
	
	QDomElement page = docElem.namedItem( "page" ).toElement();
	QDomElement paper = page.namedItem( "layout" ).toElement();
    int ptPageHeight = paper.attribute( "height" ).toInt();
    int ptPageWidth = paper.attribute( "width" ).toInt();

	QDomElement outPaper = outdoc.createElement( "PAPER" );
    karbondoc.appendChild( outPaper );
    outPaper.setAttribute( "width", ptPageWidth );
    outPaper.setAttribute( "height", ptPageHeight );
    outPaper.setAttribute( "unit", KoUnit::unitName(KoUnit::U_PT) );
	
	QDomElement layer = outdoc.createElement( "LAYER" );
    karbondoc.appendChild( layer );
    layer.setAttribute( "name", "Layer" );
    layer.setAttribute( "visible", "1" );
		
	QDomElement composite = outdoc.createElement( "COMPOSITE" );
	layer.appendChild( composite );
	
	QDomElement stroke = outdoc.createElement( "STROKE" );
	composite.appendChild( stroke );
	
	QDomElement lay = page.namedItem( "layer" ).toElement();
	QDomElement rect = lay.namedItem( "rectangle" ).toElement();
	QDomElement poly = rect.namedItem( "polyline" ).toElement();
	QDomElement gobject = poly.namedItem( "gobject" ).toElement();
	
	int lineWidth = gobject.attribute( "linewidth" ).toInt();
	stroke.setAttribute( "lineWidth", lineWidth );
	stroke.setAttribute( "lineCap", "0" );
	stroke.setAttribute( "lineJoin", "0" );
	stroke.setAttribute( "miterLimit", "10" );
	
	QDomElement color = outdoc.createElement( "COLOR" );
	stroke.appendChild( color );
	QColor fgColor = gobject.attribute( "strokecolor" );
	
	color.setAttribute( "v1", fgColor.red() );
	color.setAttribute( "v2", fgColor.green() );
	color.setAttribute( "v3", fgColor.blue() );
	color.setAttribute( "opacity", "1" );
	color.setAttribute( "colorSpace", "0" );
		
	QDomElement fill = outdoc.createElement( "FILL" );	
	composite.appendChild( fill );	
	
	QDomElement segment = outdoc.createElement( "SEGMENTS");
	QDomElement seg = docElem.namedItem( "seg" ).toElement();
	int kind = seg.attribute( "kind" ).toInt();
	composite.appendChild( segment );
	segment.setAttribute( "isClosed", kind );
	QDomElement move = outdoc.createElement( "MOVE" );
	int x = rect.attribute( "x" ).toInt();
	int y = rect.attribute( "y" ).toInt();
	segment.appendChild( move );
	move.setAttribute( "x", x );
	move.setAttribute( "y", y );
			
	QDomElement c = poly.firstChild().toElement();
	for( ; !c.isNull(); c = c.nextSibling().toElement() )
	{	
		if ( c.tagName() == "point" )
		{
			QDomElement line = outdoc.createElement( "LINE" );
			int lineX = c.attribute( "x" ).toInt();
			int lineY = c.attribute( "y" ).toInt();
			segment.appendChild( line );
			line.setAttribute( "x", lineX );
			line.setAttribute( "y", lineY );
		}
	}	
}

#include <kontourimport.moc>
