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
#include <shapes/vellipse.h>
#include <shapes/vrectangle.h>
#include <shapes/vpolygon.h>
#include <core/vpath.h>
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

void
KontourImport::parseGObject( VObject *object, const QDomElement &e )
{
	if( !e.attribute( "fillstyle" ).isEmpty() )
	{
		VFill fill;
		int fillstyle = e.attribute( "fillstyle" ).toInt();
		switch( fillstyle )
		{
			case 1:
					{
					fill.setType( VFill::solid );
					QColor c;
					c.setNamedColor( e.attribute( "fillcolor" ) );
					VColor color( c );
					fill.setColor( color );
					}
					break;
			case 4: 
					{
					VGradient grad;
					fill.setType( VFill::grad );
					fill.gradient() = grad;
					}
					break;
		}
		object->setFill( fill );
	}
	if( !e.attribute( "strokecolor" ).isEmpty() )
	{
		VStroke stroke;
		int strokestyle = e.attribute( "strokestyle" ).toInt();
		switch( strokestyle )
		{
			case 1:
					{
					QColor c;
					c.setNamedColor( e.attribute( "strokecolor" ) );
					VColor color( c );
					stroke.setColor( color );
					}
					break;
		}
		float lineWidth = e.attribute( "linewidth" ).toFloat();
		stroke.setLineWidth( lineWidth );
		object->setStroke( stroke );
	}
}

void
KontourImport::convert()
{	
	QDomElement docElem = inpdoc.documentElement();    	
	QDomElement lay = docElem.namedItem( "page" ).namedItem( "layer" ).toElement();
	
	QDomElement b = lay.firstChild().toElement();
	for( ; !b.isNull(); b = b.nextSibling().toElement() )
	{       
		if ( b.tagName() == "rectangle" )
		{
			int x = b.attribute( "x" ).toInt();
			int y = b.attribute( "y" ).toInt();
			int width = b.attribute( "width" ).toInt();
			int height = b.attribute( "height" ).toInt();
			VObject *rect = new VRectangle( 0L, KoPoint( x, y ) , width, height );
			QDomElement object = b.namedItem( "polyline" ).namedItem( "gobject" ).toElement();
			parseGObject( rect, object );
			m_document.append( rect );
		}
		else
		if ( b.tagName() == "ellipse" )
		{
			QDomElement object = b.namedItem( "gobject" ).toElement();
			QDomElement matrix = object.namedItem( "matrix" ).toElement();
			/**
			* Kontour uses a quite different way to display ellipses, so we
			* need to calculate the values for the Karbon ellipse here
			*/
			double left = ( b.attribute( "x" ).toDouble() + matrix.attribute( "dx" ).toInt() ) - ( b.attribute( "rx" ).toDouble() / 2 );
			double right = left + b.attribute( "rx" ).toDouble();
			double top = ( b.attribute( "y" ).toDouble() + matrix.attribute( "dy" ).toInt() ) - ( b.attribute( "ry" ).toDouble() / 2 );
			double bottom = top + b.attribute( "ry" ).toDouble();
			double height =  top - bottom;
			double width = right - left; 
			// Append the ellipse to the document
			VObject *ellipse = new VEllipse( 0L, KoPoint( left, top ),  width, height );
			parseGObject( ellipse, object );
			m_document.append( ellipse );
		}
		else if( b.tagName() == "polyline" )
		{
			/**
			* Kontour is much simpler because it doesn't support curves, so
			* we're done with connecting points with lines.
			*/
			QDomElement point = b.firstChild().toElement();
			VComposite *path = new VComposite( &m_document );
			double x, y;
			x = point.attribute( "x" ).toDouble();
			y = point.attribute( "y" ).toDouble();
			path->moveTo( KoPoint( x, y ) );
			point = point.nextSibling().toElement();
			for( ; point.tagName() != "gobject"; point = point.nextSibling().toElement() )
			{
				x = point.attribute( "x" ).toDouble();
				y = point.attribute( "y" ).toDouble();
				path->lineTo( KoPoint( x, y ) );
			}
			parseGObject( path, point );
			m_document.append( path );	
		}
		else if( b.tagName() == "polygon" )
		{
			QDomElement point = b.namedItem( "polyline" ).firstChild().toElement();
			VComposite *path = new VComposite( &m_document );
			double x, y;
			x = point.attribute( "x" ).toDouble();
			y = point.attribute( "y" ).toDouble();
			path->moveTo( KoPoint( x, y ) );
			point = point.nextSibling().toElement();
			for( ; point.tagName() != "gobject"; point = point.nextSibling().toElement() )
			{
				x = point.attribute( "x" ).toDouble();
				y = point.attribute( "y" ).toDouble();
				path->lineTo( KoPoint( x, y ) );
			}
			path->close();
			// back to first point
			parseGObject( path, point );
			m_document.append( path );	
		}
	}
	
	m_document.saveXML( outdoc );
}

#include <kontourimport.moc>
