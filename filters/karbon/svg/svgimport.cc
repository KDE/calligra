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

#include <svgimport.h>
#include <koFilterChain.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <koUnit.h>
#include <koGlobal.h>
#include <shapes/vellipse.h>
#include <shapes/vrectangle.h>
#include <shapes/vpolygon.h>
#include <qcolor.h>
#include <qfile.h>

typedef KGenericFactory<SvgImport, KoFilter> SvgImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonsvgimport, SvgImportFactory( "karbonsvgimport" ) );

SvgImport::SvgImport(KoFilter *, const char *, const QStringList&) :
    KoFilter(),
    outdoc( "DOC" )
{
}

SvgImport::~SvgImport()
{

}

KoFilter::ConversionStatus SvgImport::convert(const QCString& from, const QCString& to)
{
	// check for proper conversion
	if( to != "application/x-karbon" || from != "image/svg+xml" )
		return KoFilter::NotImplemented;

	QFile in( m_chain->inputFile() );
    if( !in.open( IO_ReadOnly ) )
	{
		kdError(30502) << "Unable to open input file" << endl;
		in.close();
		return KoFilter::FileNotFound;
	}

	QTextStream stream( &in );
	inpdoc.setContent( stream.device() );

	// Do the conversion!

	convert();

	KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
	if( !out ) 
	{
		kdError(30502) << "Unable to open output file!" << endl;
		return KoFilter::StorageCreationError;
	}
	QCString cstring = outdoc.toCString(); // utf-8 already
	out->writeBlock( cstring.data(), cstring.length() );

	in.close();
	return KoFilter::OK; // was successfull
}

/*void
SvgImport::parseGObject( VObject *object, const QDomElement &e )
{
	if( !e.attribute( "fillcolor" ).isEmpty() )
	{
		VFill fill;
		QColor c;
		c.setNamedColor( e.attribute( "fillcolor" ) );
		VColor color( c );
		fill.setColor( color );
		object->setFill( fill );
	}
	if( !e.attribute( "strokecolor" ).isEmpty() )
	{
		VStroke stroke;
		QColor c;
		c.setNamedColor( e.attribute( "strokecolor" ) );
		VColor color( c );
		stroke.setColor( color );
		object->setStroke( stroke );
	}
}*/

void
SvgImport::convert()
{	
	QDomElement docElem = inpdoc.documentElement();

	QDomElement b = docElem.firstChild().toElement();
	for( ; !b.isNull(); b = b.nextSibling().toElement() )
	{
		if( b.tagName() == "rect" )
		{
			int x = b.attribute( "x" ).toInt();
			int y = b.attribute( "y" ).toInt();
			int width = b.attribute( "width" ).toInt();
			int height = b.attribute( "height" ).toInt();
			VObject *rect = new VRectangle( 0L, KoPoint( x, height + y ) , width, height );
			//QDomElement object = b.namedItem( "polyline" ).namedItem( "gobject" ).toElement();
			//parseGObject( rect, object );
			m_document.append( rect );
		}
		else if( b.tagName() == "ellipse" )
		{
			//QDomElement object = b.namedItem( "gobject" ).toElement();
			//QDomElement matrix = object.namedItem( "matrix" ).toElement();
			double rx = b.attribute( "rx" ).toDouble();
			double ry = b.attribute( "ry" ).toDouble();
			double left	= b.attribute( "cx" ).toDouble() - ( rx / 2.0 );
			double top	= b.attribute( "cy" ).toDouble() + ( ry / 2.0 );
			// Append the ellipse to the document
			VObject *ellipse = new VEllipse( 0L, KoPoint( left, top ), rx * 2.0, ry * 2.0 );
			//parseGObject( ellipse, object );
			m_document.append( ellipse );
		}
		else if( b.tagName() == "circle" )
		{
			double r = b.attribute( "r" ).toDouble();
			double left	= b.attribute( "cx" ).toDouble() - ( r / 2.0 );
			double top	= b.attribute( "cy" ).toDouble() + ( r / 2.0 );
			// Append the ellipse to the document
			VObject *circle = new VEllipse( 0L, KoPoint( left, top ), r * 2.0, r * 2.0 );
			//parseGObject( circle, object );
			m_document.append( circle );
		}
		else if( b.tagName() == "polyline" )
		{
			QDomElement point = b.firstChild().toElement();
			VComposite *path = new VComposite( &m_document );
			int x, y;
			x = point.attribute( "x" ).toInt();
			y = point.attribute( "y" ).toInt();
			path->moveTo( KoPoint( x, y ) );
			point = point.nextSibling().toElement();
			for( ; !point.isNull(); point = point.nextSibling().toElement() )
			{
				x = point.attribute( "x" ).toInt();
				y = point.attribute( "y" ).toInt();
				path->lineTo( KoPoint( x, y ) );
			}
			path->close();
			QDomElement object = b.namedItem( "gobject" ).toElement();
			//parseGObject( path, object );
			m_document.append( path );	
		}
	}
	
	outdoc = m_document.saveXML();
}

#include <svgimport.moc>
