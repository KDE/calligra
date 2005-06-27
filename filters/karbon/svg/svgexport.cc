/* This file is part of the KDE project
   Copyright (C) 2002, 2003 The Karbon Developers

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
#include <qdom.h>
#include <qfile.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <kgenericfactory.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <koStore.h>

#include "svgexport.h"
#include "vcolor.h"
#include "vcomposite.h"
#include "vdashpattern.h"
#include "vdocument.h"
#include "vfill.h"
#include "vgradient.h"
#include "vgroup.h"
#include "vimage.h"
#include "vlayer.h"
#include "vpath.h"
#include "vpattern.h"
#include "vsegment.h"
#include "vselection.h"
#include "vstroke.h"
//#include "vtext.h"

#include <kdebug.h>


typedef KGenericFactory<SvgExport, KoFilter> SvgExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonsvgexport, SvgExportFactory( "kofficefilters" ) )


SvgExport::SvgExport( KoFilter*, const char*, const QStringList& )
	: KoFilter()
{
	m_gc.setAutoDelete( true );
}

KoFilter::ConversionStatus
SvgExport::convert( const QCString& from, const QCString& to )
{
	if ( to != "image/svg+xml" || from != "application/x-karbon" )
	{
		return KoFilter::NotImplemented;
	}

	KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );
	if( !storeIn )
		return KoFilter::StupidError;

	QFile fileOut( m_chain->outputFile() );
	if( !fileOut.open( IO_WriteOnly ) )
	{
		delete storeIn;
		return KoFilter::StupidError;
	}

	QDomDocument domIn;
	domIn.setContent( storeIn );
	QDomElement docNode = domIn.documentElement();

	m_stream = new QTextStream( &fileOut );
	QString body;
	m_body = new QTextStream( &body, IO_ReadWrite );
	QString defs;
	m_defs = new QTextStream( &defs, IO_ReadWrite );


	// load the document and export it:
	VDocument doc;
	doc.load( docNode );
	doc.accept( *this );

	*m_stream << defs;
	*m_stream << body;

	fileOut.close();

	delete m_stream;
	delete m_defs;
	delete m_body;

	return KoFilter::OK;
}

void
SvgExport::visitVDocument( VDocument& document )
{
	// select all objects:
	document.selection()->append();

	// get the bounding box of the page
	KoRect rect( 0, 0, document.width(), document.height() );

	// standard header:
	*m_defs <<
		"<?xml version=\"1.0\" standalone=\"no\"?>\n" <<
		"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" " <<
		"\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">"
	<< endl;

	// add some PR.  one line is more than enough.  
	*m_defs <<
		"<!-- Created using Karbon14, part of koffice: http://www.koffice.org/karbon -->" << endl;

	*m_defs <<
		"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" <<
		rect.width() << "px\" height=\"" << rect.height() << "px\">" << endl;
	*m_defs << "<defs>" << endl;

	*m_body << "<g transform=\"scale(1, -1) translate(0, -" << rect.height() << ")\">" << endl;

	// we dont need the selection anymore:
	document.selection()->clear();

	// set up gc
	SvgGraphicsContext *gc = new SvgGraphicsContext;
	m_gc.push( gc );

	// export layers:
	VVisitor::visitVDocument( document );

	// end tag:
	*m_body << "</g>" << endl;
	*m_defs << "</defs>" << endl;
	*m_body << "</svg>" << endl;
}

QString
SvgExport::getID( VObject *obj )
{
	if( obj && !obj->name().isEmpty() )
		return QString( " id=\"%1\"" ).arg( obj->name() );
	return QString();
}

void
SvgExport::visitVGroup( VGroup& group )
{
	*m_body << "<g" << getID( &group ) << ">" << endl;
	VVisitor::visitVGroup( group );
	*m_body << "</g>" << endl;
}

// horrible but at least something gets exported now
// will need this for patterns
void
SvgExport::visitVImage( VImage& image )
{
	*m_body << "<image ";
	VVisitor::visitVImage( image );
	*m_body << "x=\"" << "\" ";
	*m_body << "y=\"" << "\" ";
	*m_body << "width=\"" << "\" ";
	*m_body << "height=\"" << "\" ";
	*m_body << "xlink:href=\"" << "\"";
	*m_body << " />" << endl;
}

void
SvgExport::visitVLayer( VLayer& layer )
{
	*m_body << "<g" << getID( &layer ) << ">" << endl;
	VVisitor::visitVLayer( layer );
	*m_body << "</g>" << endl;
}

void
SvgExport::visitVPath( VPath& composite )
{
	*m_body << "<path" << getID( &composite );

	VVisitor::visitVPath( composite );

	getFill( *( composite.fill() ) );
	getStroke( *( composite.stroke() ) );

	QString d;
	composite.saveSvgPath( d );
	*m_body << " d=\"" << d << "\" ";

	if( composite.fillRule() != m_gc.current()->fillRule )
	{
		if( composite.fillRule() == evenOdd )
			*m_body << " fill-rule=\"evenodd\"";
		else
			*m_body << " fill-rule=\"nonzero\"";
	}

	*m_body << " />" << endl;
}

void
SvgExport::visitVSubpath( VSubpath& )
{
}

void 
SvgExport::visitVLayer( VLayer& layer ) 
{ 
        *m_body << "<g" << getID( &layer ) << ">" << endl; 
        VVisitor::visitVLayer( layer ); 
        *m_body << "</g>" << endl; 
}

QString createUID()
{
	static unsigned int nr = 0;

	return "defitem" + QString().setNum( nr++ );
}

void
SvgExport::getColorStops( const QPtrVector<VColorStop> &colorStops )
{
	for( unsigned int i = 0; i < colorStops.count() ; i++ )
	{
		*m_defs << "<stop stop-color=\"";
		getHexColor( m_defs, colorStops.at( i )->color );
		*m_defs << "\" offset=\"" << QString().setNum( colorStops.at( i )->rampPoint );
		*m_defs << "\" stop-opacity=\"" << colorStops.at( i )->color.opacity() << "\"" << " />" << endl;
	}
}

void
SvgExport::getGradient( const VGradient& grad )
{
	QString uid = createUID();
	if( grad.type() == VGradient::linear )
	{
		// do linear grad
		*m_defs << "<linearGradient id=\"" << uid << "\" ";
		*m_defs << "gradientUnits=\"userSpaceOnUse\" ";
		*m_defs << "x1=\"" << grad.origin().x() << "\" ";
		*m_defs << "y1=\"" << grad.origin().y() << "\" ";
		*m_defs << "x2=\"" << grad.vector().x() << "\" ";
		*m_defs << "y2=\"" << grad.vector().y() << "\" ";
		if( grad.repeatMethod() == VGradient::reflect )
			*m_defs << "spreadMethod=\"reflect\" ";
		else if( grad.repeatMethod() == VGradient::repeat )
			*m_defs << "spreadMethod=\"repeat\" ";
		*m_defs << ">" << endl;

		// color stops
		getColorStops( grad.colorStops() );

		*m_defs << "</linearGradient>" << endl;
		*m_body << "url(#" << uid << ")";
	}
	else if( grad.type() == VGradient::radial )
	{
		// do radial grad
		*m_defs << "<radialGradient id=\"" << uid << "\" ";
		*m_defs << "gradientUnits=\"userSpaceOnUse\" ";
		*m_defs << "cx=\"" << grad.origin().x() << "\" ";
		*m_defs << "cy=\"" << grad.origin().y() << "\" ";
		*m_defs << "fx=\"" << grad.focalPoint().x() << "\" ";
		*m_defs << "fy=\"" << grad.focalPoint().y() << "\" ";
		double r = sqrt( pow( grad.vector().x() - grad.origin().x(), 2 ) + pow( grad.vector().y() - grad.origin().y(), 2 ) );
		*m_defs << "r=\"" << QString().setNum( r ) << "\" ";
		if( grad.repeatMethod() == VGradient::reflect )
			*m_defs << "spreadMethod=\"reflect\" ";
		else if( grad.repeatMethod() == VGradient::repeat )
			*m_defs << "spreadMethod=\"repeat\" ";
		*m_defs << ">" << endl;

		// color stops
		getColorStops( grad.colorStops() );

		*m_defs << "</radialGradient>" << endl;
		*m_body << "url(#" << uid << ")";
	}
	// gah! pointless abbreviation of conical to conic
	else if( grad.type() == VGradient::conic )
	{
		// fake conical grad as radial.  
		// fugly but better than data loss.  
		*m_defs << "<radialGradient id=\"" << uid << "\" ";
		*m_defs << "gradientUnits=\"userSpaceOnUse\" ";
		*m_defs << "cx=\"" << grad.origin().x() << "\" ";
		*m_defs << "cy=\"" << grad.origin().y() << "\" ";
		*m_defs << "fx=\"" << grad.focalPoint().x() << "\" ";
		*m_defs << "fy=\"" << grad.focalPoint().y() << "\" ";
		double r = sqrt( pow( grad.vector().x() - grad.origin().x(), 2 ) + pow( grad.vector().y() - grad.origin().y(), 2 ) );
		*m_defs << "r=\"" << QString().setNum( r ) << "\" ";
		if( grad.repeatMethod() == VGradient::reflect )
			*m_defs << "spreadMethod=\"reflect\" ";
		else if( grad.repeatMethod() == VGradient::repeat )
			*m_defs << "spreadMethod=\"repeat\" ";
		*m_defs << ">" << endl;

		// color stops
		getColorStops( grad.colorStops() );

		*m_defs << "</radialGradient>" << endl;
		*m_body << "url(#" << uid << ")";
	}
}

// better than nothing
void
SvgExport::getPattern( const VPattern & patt )
{
	QString uid = createUID();
	*m_defs << "<pattern id=\"" << uid << "\" ";
	*m_defs << "width=\"" << "\" ";
	*m_defs << "height=\"" << "\" ";
	*m_defs << "patternUnits=\"userSpaceOnUse\" ";
	*m_defs << "patternContentUnits=\"userSpaceOnUse\" "; 
	*m_defs << " />" << endl;
	// TODO: insert hard work here ;)
	*m_defs << "</pattern>" << endl;
	*m_body << "url(#" << uid << ")";
}

void
SvgExport::getFill( const VFill& fill )
{
	*m_body << " fill=\"";
	if( fill.type() == VFill::none )
		*m_body << "none";
	else if( fill.type() == VFill::grad )
		getGradient( fill.gradient() );
	else if( fill.type() == VFill::patt )
		getPattern( fill.pattern() );
	else
		getHexColor( m_body, fill.color() );
	*m_body << "\"";

	if( fill.color().opacity() != m_gc.current()->fill.color().opacity() )
		*m_body << " fill-opacity=\"" << fill.color().opacity() << "\"";
}

void
SvgExport::getStroke( const VStroke& stroke )
{
	if( stroke.type() != m_gc.current()->stroke.type() )
	{
		*m_body << " stroke=\"";
		if( stroke.type() == VStroke::none )
			*m_body << "none";
		else if( stroke.type() == VStroke::grad )
			getGradient( stroke.gradient() );
		else
			getHexColor( m_body, stroke.color() );
		*m_body << "\"";
	}

	if( stroke.color().opacity() != m_gc.current()->stroke.color().opacity() )
		*m_body << " stroke-opacity=\"" << stroke.color().opacity() << "\"";

	if( stroke.lineWidth() != m_gc.current()->stroke.lineWidth() )
		*m_body << " stroke-width=\"" << stroke.lineWidth() << "\"";

	if( stroke.lineCap() != m_gc.current()->stroke.lineCap() )
	{
		if( stroke.lineCap() == VStroke::capButt )
			*m_body << " stroke-linecap=\"butt\"";
		else if( stroke.lineCap() == VStroke::capRound )
			*m_body << " stroke-linecap=\"round\"";
		else if( stroke.lineCap() == VStroke::capSquare )
			*m_body << " stroke-linecap=\"square\"";
	}

	if( stroke.lineJoin() != m_gc.current()->stroke.lineJoin() )
	{
		if( stroke.lineJoin() == VStroke::joinMiter )
		{
			*m_body << " stroke-linejoin=\"miter\"";
			*m_body << " stroke-miterlimit=\"" << stroke.miterLimit() << "\"";
		}
		else if( stroke.lineJoin() == VStroke::joinRound )
			*m_body << " stroke-linejoin=\"round\"";
		else if( stroke.lineJoin() == VStroke::joinBevel )
				*m_body << " stroke-linejoin=\"bevel\"";
	}

	// dash
	if( stroke.dashPattern().array().count() > 0 )
	{
		*m_body << " stroke-dashoffset=\"" << stroke.dashPattern().offset() << "\"";
		*m_body << " stroke-dasharray=\" ";

		QValueListConstIterator<float> itr;
		for(itr = stroke.dashPattern().array().begin(); itr != stroke.dashPattern().array().end(); ++itr )
		{
			*m_body << *itr << " ";
		}
		*m_body << "\"";
	}
}

void
SvgExport::getHexColor( QTextStream *stream, const VColor& color )
{
	// Convert the various color-spaces to hex

	QString Output;

	VColor copy( color );
	copy.setColorSpace( VColor::rgb );

	Output.sprintf( "#%02x%02x%02x", int( copy[0] * 255.0 ), int( copy[1] * 255.0 ), int( copy[2] * 255.0 ) );

	*stream << Output;
}

void
SvgExport::visitVText( VText& /*text*/ )
{
/*
	// TODO: set placement once karbon supports it

	*m_body << "<text";

	if( !node.attribute( "size" ).isNull() )
	{
		*m_body << " font-size=\"" << node.attribute( "size" ) << "\"";
	}

	if( !node.attribute( "family" ).isNull() )
	{
		*m_body << " font-family=\"" << node.attribute( "family" ) << "\"";
	}

	if( !node.attribute( "bold" ).isNull() )
	{
		*m_body << " font-weight=\"bold\"";
	}

	if( !node.attribute( "italic" ).isNull() )
	{
		*m_body << " font-style=\"italic\"";
	}

	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "FILL" )
				exportFill( s, e );
			if( e.tagName() == "STROKE" )
				exportStroke( s, e );
		}
	}

	*m_body << ">";


	if( !node.attribute( "text" ).isNull() )
	{
		*m_body << node.attribute( "text" );
	}

	*m_body << "</text>" << endl;
*/
}

#include "svgexport.moc"

