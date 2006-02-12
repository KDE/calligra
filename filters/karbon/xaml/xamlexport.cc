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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

// based on the SVG exporter.  Not intended for public release
// Microsoft WVG renamed to XAML Graphics.  Worry about that later.  

#include <qcstring.h>
#include <qdom.h>
#include <qfile.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <kgenericfactory.h>
#include <KoFilter.h>
#include <KoFilterChain.h>
#include <KoStore.h>

#include "xamlexport.h"
#include "vcolor.h"
#include "vcomposite.h"
#include "vdashpattern.h"
#include "vdocument.h"
#include "vfill.h"
#include "vgradient.h"
#include "vgroup.h"
#include "vlayer.h"
#include "vpath.h"
#include "vsegment.h"
#include "vselection.h"
#include "vstroke.h"
//#include "vtext.h"  // TODO Convert Text to Paths for basic export.  Not our problem.  
// TODO inline Images?

#include <kdebug.h>


typedef KGenericFactory<XAMLExport, KoFilter> XAMLExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonxamlexport, XAMLExportFactory( "kofficefilters" ) )


XAMLExport::XAMLExport( KoFilter*, const char*, const QStringList& )
	: KoFilter()
{
	m_gc.setAutoDelete( true );
}

KoFilter::ConversionStatus
XAMLExport::convert( const QCString& from, const QCString& to )
{
	// TODO: ???
	if ( to != "image/wvg+xml" || from != "application/x-karbon" )
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
XAMLExport::visitVDocument( VDocument& document )
{
	// select all objects:
	document.selection()->append();

	// get the bounding box of the page
	KoRect rect( 0, 0, document.width(), document.height() );

	// standard header:
	*m_defs <<
		"<?xml version=\"1.0\" ?>\n" <<
		/* "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" " <<* */
		// "\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">"
	/*<<*/ endl;

	// Add one line comment to identify Content Creator, 
	// probably remove this later
	// TODO: schemas
	// http://schemas.microsoft.com/winfx/avalon/2005 
	// http://schemas.microsoft.com/2003/xaml  
	// need to mention defs too Defenitions namespace xmlns:def="Definition"	
	*m_defs <<
		"<!-- Generator: Karbon14 WVG XAML Graphics export filter  $VERSION/$DATE.  -->" << endl;
	*m_defs <<
		"<Canvas xmlns=\"http://schemas.microsoft.com/winfx/avalon/2005\" Width=\"" << rect.width() << 
		"Height=\"" << rect.height() << "\">" << endl;
	*m_defs << "<Canvas.Resources>" << endl;

	// bleuch: this is horrible, do something about it TODO
	// Microsoft Acrylic has a transform group just like this
	*m_body << "<Transform=\"scale(1, -1) Translate(0, -" << rect.height() << ")\">" << endl;

	// we dont need the selection anymore:
	document.selection()->clear();

	// set up gc
	XAMLGraphicsContext *gc = new XAMLGraphicsContext;
	m_gc.push( gc );

	// export layers:
	VVisitor::visitVDocument( document );

	// end tag:
	*m_body << "</Canvas>" << endl;
	*m_defs << "</Canvas.Resources>" << endl;
	*m_body << "</Canvas>" << endl;
}

QString
XAMLExport::getID( VObject *obj )
{
	if( obj && !obj->name().isEmpty() )
		return QString( " Name=\"%1\"" ).arg( obj->name() );
	return QString();
}

// which markup to use?  Group or Canvas?
// for now assume Group will work.  TODO: Test properly!
void
XAMLExport::visitVGroup( VGroup& group )
{
	*m_body << "<Canvas" << getID( &group ) << ">" << endl;
	VVisitor::visitVGroup( group );
	*m_body << "</Canvas>" << endl;
}

void
XAMLExport::visitVPath( VPath& composite )
{
	*m_body << "<Path" << getID( &composite );

	VVisitor::visitVPath( composite );

	getFill( *( composite.fill() ) );
	getStroke( *( composite.stroke() ) );

	QString d;
	composite.saveSvgPath( d );
	*m_body << " Data=\"" << d << "\" ";

	if( composite.fillRule() != m_gc.current()->fillRule )
	{
		if( composite.fillRule() == evenOdd )
			*m_body << " FillRule=\"EvenOdd\"";
		else
			*m_body << " FillRule=\"NonZero\"";
	}

	*m_body << " />" << endl;
}

void
XAMLExport::visitVSubpath( VSubpath& )
{
}

QString createUID()
{
	static unsigned int nr = 0;

	return "defitem" + QString().setNum( nr++ );
}

void
XAMLExport::getColorStops( const QPtrVector<VColorStop> &colorStops )
{
	for( unsigned int i = 0; i < colorStops.count() ; i++ )
	{
		*m_defs << "<GradientStop Color=\"";
		getHexColor( m_defs, colorStops.at( i )->color );
		*m_defs << "\" Offset=\"" << QString().setNum( colorStops.at( i )->rampPoint );
		//  XAML uses ARGB values and other methods such as masks for Transparency/Opacity    #  aa  rrggbb  
		// *m_defs << "\" stop-opacity=\"" << colorStops.at( i )->color.opacity() << "\"" << " />" << endl;
		// Maybe this only applies to gradients, need to check.  
	}
}

void
XAMLExport::getGradient( const VGradient& grad )
{
	QString uid = createUID();
	if( grad.type() == VGradient::linear )
	{
		// do linear grad
		*m_defs << "<LinearGradientBrush id=\"" << uid << "\" ";
		*m_defs << "GradientUnits=\"UserSpaceOnUse\" ";
		*m_defs << "StartPoint=\"" << grad.origin().x() << ",";
		*m_defs << grad.origin().y() << "\" ";
		*m_defs << "EndPoint=\"" << grad.vector().x() << ",";
		*m_defs << grad.vector().y() << "\" ";
		if( grad.repeatMethod() == VGradient::reflect )
			*m_defs << "SpreadMethod=\"Reflect\" ";
		else if( grad.repeatMethod() == VGradient::repeat )
			*m_defs << "SpreadMethod=\"Repeat\" ";
		*m_defs << ">" << endl;

		// color stops
		getColorStops( grad.colorStops() );

		*m_defs << "</LinearGradientBrush>" << endl;
		*m_body << "url(#" << uid << ")";
	}
	else if( grad.type() == VGradient::radial )
	{
		// do radial grad
		*m_defs << "<RadialGradientBrush Name=\"" << uid << "\" ";
		// *m_defs << "gradientUnits=\"userSpaceOnUse\" "; // Absolute?
		*m_defs << "Center=\"" << grad.origin().x() << ",";
		*m_defs << grad.origin().y() << "\" ";
		// Gradient Origin also known as Focus
		*m_defs << "GradientOrigin=\"" << grad.focalPoint().x() << ",";
		*m_defs << grad.focalPoint().y() << "\" ";
		double r = sqrt( pow( grad.vector().x() - grad.origin().x(), 2 ) + pow( grad.vector().y() - grad.origin().y(), 2 ) );
		*m_defs << "Radius=\"" << QString().setNum( r ) << "\" ";
		if( grad.repeatMethod() == VGradient::reflect )
			*m_defs << "SpreadMethod=\"Reflect\" ";
		else if( grad.repeatMethod() == VGradient::repeat )
			*m_defs << "SpreadMethod=\"Repeat\" ";
		*m_defs << ">" << endl;

		// color stops
		getColorStops( grad.colorStops() );

		*m_defs << "</RadialGradientBrush>" << endl;
		*m_body << "url(#" << uid << ")";
	}
}

void
XAMLExport::getFill( const VFill& fill )
{
	*m_body << " Fill=\"";
	if( fill.type() == VFill::none )
		*m_body << "none";
	else if( fill.type() == VFill::grad )
		getGradient( fill.gradient() );
	else
		getHexColor( m_body, fill.color() );
	*m_body << "\"";

	if( fill.color().opacity() != m_gc.current()->fill.color().opacity() )
		*m_body << " FillOpacity=\"" << fill.color().opacity() << "\"";
}

void
XAMLExport::getStroke( const VStroke& stroke )
{
	if( stroke.type() != m_gc.current()->stroke.type() )
	{
		*m_body << " Stroke=\"";
		if( stroke.type() == VStroke::none )
			*m_body << "None";
		else if( stroke.type() == VStroke::grad )
			getGradient( stroke.gradient() );
		else
			getHexColor( m_body, stroke.color() );
		*m_body << "\"";
	}

	if( stroke.color().opacity() != m_gc.current()->stroke.color().opacity() )
		*m_body << " StrokeOpacity=\"" << stroke.color().opacity() << "\"";

	if( stroke.lineWidth() != m_gc.current()->stroke.lineWidth() )
		*m_body << " StrokeThickness=\"" << stroke.lineWidth() << "\"";

	if( stroke.lineCap() != m_gc.current()->stroke.lineCap() )
	{
		if( stroke.lineCap() == VStroke::capButt )
			*m_body << " StrokeLineCap=\"Butt\"";
		else if( stroke.lineCap() == VStroke::capRound )
			*m_body << " StrokeLineCap=\"round\"";
		else if( stroke.lineCap() == VStroke::capSquare )
			*m_body << " StrokeLineCap=\"square\"";
	}

	if( stroke.lineJoin() != m_gc.current()->stroke.lineJoin() )
	{
		if( stroke.lineJoin() == VStroke::joinMiter )
		{
			*m_body << " StrokeLineJoin=\"Miter\"";
			*m_body << " StrokeMiterLimit=\"" << stroke.miterLimit() << "\"";
		}
		else if( stroke.lineJoin() == VStroke::joinRound )
			*m_body << " StrokeLineJoin=\"Round\"";
		else if( stroke.lineJoin() == VStroke::joinBevel )
				*m_body << " StrokeLineJoin=\"Bevel\"";
	}

	// dash
	if( stroke.dashPattern().array().count() > 0 )
	{
		*m_body << " StrokeDashOffset=\"" << stroke.dashPattern().offset() << "\"";
		*m_body << " StrokeDashArray=\" ";

		QValueListConstIterator<float> itr;
		for(itr = stroke.dashPattern().array().begin(); itr != stroke.dashPattern().array().end(); ++itr )
		{
			*m_body << *itr << " ";
		}
		*m_body << "\"";
	}
}

void
XAMLExport::getHexColor( QTextStream *stream, const VColor& color )
{
	// Convert the various color-spaces to hex

	QString Output;

	VColor copy( color );
	copy.setColorSpace( VColor::rgb );

	Output.sprintf( "#%02x%02x%02x", int( copy[0] * 255.0 ), int( copy[1] * 255.0 ), int( copy[2] * 255.0 ) );

	*stream << Output;
}

#include "xamlexport.moc"

