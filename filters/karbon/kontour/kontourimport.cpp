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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kontourimport.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <KoUnit.h>
#include <KoGlobal.h>
#include <shapes/vellipse.h>
#include <shapes/vrectangle.h>
#include <shapes/vpolygon.h>
#include <commands/vtransformcmd.h>
#include <core/vpath.h>
#include <core/vfill.h>
#include <core/vstroke.h>
#include <QColor>
#include <QFile>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3CString>

#define DPI 90

typedef KGenericFactory<KontourImport> KontourImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonkontourimport, KontourImportFactory( "kofficefilters" ) )

KontourImport::KontourImport(QObject *parent, const QStringList&) :
    KoFilter(parent),
    outdoc( "DOC" )
{
}

KontourImport::~KontourImport()
{

}

KoFilter::ConversionStatus KontourImport::convert(const QByteArray& from, const QByteArray& to)
{
	// check for proper conversion
	if ( to != "application/x-karbon" || ( from != "application/x-kontour" && from != "application/x-killustrator") )
		return KoFilter::NotImplemented;


	KoStoreDevice* inpdev = m_chain->storageFile( "root", KoStore::Read );
	if ( !inpdev )
	{
		kError(30502) << "Unable to open input stream" << endl;
		return KoFilter::StorageCreationError;
	}

	inpdoc.setContent( inpdev );

	// Do the conversion!
	convert();

	KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
	if(!out)
	{
		kError(30502) << "Unable to open output file!" << endl;
		return KoFilter::StorageCreationError;
	}
	QByteArray cstring = outdoc.toByteArray(); // utf-8 already
	out->write( cstring.data(), cstring.length() );

	return KoFilter::OK; // was successful
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
				// set color stops
				grad.clearStops();
				QColor c;
				c.setNamedColor( e.attribute( "gradcolor1" ) );
				VColor color( c );
				grad.addStop( color, 0.0, 0.5 );
				c.setNamedColor( e.attribute( "gradcolor2" ) );
				VColor color2( c );
				grad.addStop( color2, 1.0, 0.5 );
				// set coords
				KoRect bbox = object->boundingBox();
				grad.setOrigin( KoPoint( bbox.left(), bbox.y() ) );
				grad.setVector( KoPoint( bbox.right(), bbox.y() ) );
				grad.setType( (VGradient::VGradientType)e.attribute( "gradstyle" ).toInt() );
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
			case 0:	stroke.setType( VStroke::none );
					break;
			case 1:
			{
				QColor c;
				c.setNamedColor( e.attribute( "strokecolor" ) );
				VColor color( c );
				stroke.setColor( color );
			}
			break;
			case 2: case 3: case 4: case 5:
			{
				QColor c;
				c.setNamedColor( e.attribute( "strokecolor" ) );
				VColor color( c );
				stroke.setColor( color );
				VDashPattern dash;
				Q3ValueList<float> list;
				switch ( strokestyle )
				{
					case 2: // dashed line
						list << 10 << 5;
						break;
					case 3: // dotted line
						list << 1 << 5;
						break;
					case 4: // dash-dot
						list << 10 << 5 << 1 << 5;
						break;
					case 5: // dash-dot-dot
						list << 10 << 5 << 1 << 5 << 1 << 5;
						break;
				}

				dash.setArray( list );
				stroke.dashPattern() = dash;
			}
			break;
		}
		float lineWidth = e.attribute( "linewidth" ).toFloat();
		stroke.setLineWidth( lineWidth );
		object->setStroke( stroke );
	}
	// handle matrix
	QDomElement matrix = e.namedItem( "matrix" ).toElement();
	QMatrix mat( matrix.attribute( "m11" ).toDouble(),
				  matrix.attribute( "m12" ).toDouble(),
				  matrix.attribute( "m21" ).toDouble(),
				  matrix.attribute( "m22" ).toDouble(),
				  matrix.attribute( "dx" ).toDouble(),
				  matrix.attribute( "dy" ).toDouble() );

	// undo y-mirroring
	mat.scale( 1, -1 );
	mat.translate( 0, -m_document.height() );
	VTransformCmd trafo( 0L, mat );
	trafo.visit( *object );

}

void
KontourImport::convert()
{
	QDomElement docElem = inpdoc.documentElement();
	QDomElement lay;
	double height;
	double width;
	if( docElem.attribute( "version" ).toInt() == 2 )
	{
		lay = docElem;
		height = lay.firstChild().namedItem( "layout" ).toElement().attribute( "height" ).toDouble();
		width = lay.firstChild().namedItem( "layout" ).toElement().attribute( "width" ).toDouble();
	}
	else
	{
		lay = docElem.namedItem( "page" ).toElement();
		height = lay.firstChild().toElement().attribute( "height" ).toDouble();
		width = lay.firstChild().toElement().attribute( "width" ).toDouble();
	}

	m_document.setHeight( ( ( height / 72.0 ) * DPI ) );
	m_document.setWidth( ( ( width / 72.0 ) * DPI )  );

	parseGroup( lay.firstChild().toElement() );

	outdoc = m_document.saveXML();
}

void
KontourImport::parseGroup( const QDomElement &e )
{
	QDomElement b = e;
	for( ; !b.isNull(); b = b.nextSibling().toElement() )
	{
		if ( b.tagName() == "rectangle" )
		{
			int x = b.attribute( "x" ).toInt();
			int y = b.attribute( "y" ).toInt();
			int width = b.attribute( "width" ).toInt();
			int height = b.attribute( "height" ).toInt();
			VObject *rect = new VRectangle( 0L, KoPoint( x, height + y ) , width, height );
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
			VPath *path = new VPath( &m_document );
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
			VPath *path = new VPath( &m_document );
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
		else if( b.tagName() == "bezier" )
		{
			QDomElement point = b.namedItem( "polyline" ).firstChild().toElement();
			VPath *path = new VPath( &m_document );
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
		else if( b.tagName() == "group" || b.tagName() == "layer" )
		{
			parseGroup( b.toElement().firstChild().toElement() );
		}
	}
}

#include <kontourimport.moc>
