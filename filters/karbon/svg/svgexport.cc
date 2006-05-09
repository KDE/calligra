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

#include <q3cstring.h>
#include <qdom.h>
#include <qfile.h>
#include <qstring.h>
#include <q3valuelist.h>
//Added by qt3to4:
#include <QTextStream>

#include <kgenericfactory.h>
#include <KoFilter.h>
#include <KoFilterChain.h>
#include <KoStore.h>

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
#include "vtext.h"
#include <commands/vtransformcmd.h>

#include <kdebug.h>

QString INDENT("  ");

void
printIndentation( QTextStream *stream, unsigned int indent )
{
	for( unsigned int i = 0; i < indent;++i)
		*stream << INDENT;
}

typedef KGenericFactory<SvgExport> SvgExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonsvgexport, SvgExportFactory( "kofficefilters" ) )


SvgExport::SvgExport( KoFilter*, const char*, const QStringList& )
	: KoFilter(parent), m_indent( 0 ), m_indent2( 0 ), m_trans( 0L )
{
	m_gc.setAutoDelete( true );
}

KoFilter::ConversionStatus
SvgExport::convert( const QByteArray& from, const QByteArray& to )
{
	if ( to != "image/svg+xml" || from != "application/x-karbon" )
	{
		return KoFilter::NotImplemented;
	}

	KoStoreDevice* storeIn = m_chain->storageFile( "root", KoStore::Read );
	if( !storeIn )
		return KoFilter::StupidError;

	QFile fileOut( m_chain->outputFile() );
	if( !fileOut.open( QIODevice::WriteOnly ) )
	{
		delete storeIn;
		return KoFilter::StupidError;
	}

	QDomDocument domIn;
	domIn.setContent( storeIn );
	QDomElement docNode = domIn.documentElement();

	m_stream = new QTextStream( &fileOut );
	QString body;
	m_body = new QTextStream( &body, QIODevice::ReadWrite );
	QString defs;
	m_defs = new QTextStream( &defs, QIODevice::ReadWrite );

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
		"<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"" <<
		rect.width() << "px\" height=\"" << rect.height() << "px\">" << endl;
	printIndentation( m_defs, ++m_indent2 );
	*m_defs << "<defs>" << endl;

	m_indent++;
	m_indent2++;

	// we dont need the selection anymore:
	document.selection()->clear();

	// set up gc
	SvgGraphicsContext *gc = new SvgGraphicsContext;
	m_gc.push( gc );

	QMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( 0, -document.height() );

	m_trans = new VTransformCmd( 0L, mat, false );
	
	// export layers:
	VVisitor::visitVDocument( document );

	delete m_trans;
	m_trans = 0L;

	// end tag:
	printIndentation( m_defs, --m_indent2 );
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
	printIndentation( m_body, m_indent++ );
	*m_body << "<g" << getID( &group ) << ">" << endl;
	VVisitor::visitVGroup( group );
	printIndentation( m_body, --m_indent );
	*m_body << "</g>" << endl;
}

// horrible but at least something gets exported now
// will need this for patterns
void
SvgExport::visitVImage( VImage& image )
{
	printIndentation( m_body, m_indent );
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
	printIndentation( m_body, m_indent++ );
	*m_body << "<g" << getID( &layer ) << ">" << endl;
	//*m_body << " transform=\"scale(1, -1) translate(0, -" << layer.document()->height() << ")\">" << endl;
	VVisitor::visitVLayer( layer );
	printIndentation( m_body, --m_indent );
	*m_body << "</g>" << endl;
}

void
SvgExport::writePathToStream( VPath &composite, const QString &id, QTextStream *stream, unsigned int indent )
{
	if( ! stream )
		return;

	printIndentation( stream, indent );
	*stream << "<path" << id;

	VVisitor::visitVPath( composite );

	getFill( *( composite.fill() ), stream );
	getStroke( *( composite.stroke() ), stream );

	QString d;
	composite.saveSvgPath( d );
	*stream << " d=\"" << d << "\" ";

	if( composite.fillRule() != m_gc.current()->fillRule )
	{
		if( composite.fillRule() == evenOdd )
			*stream << " fill-rule=\"evenodd\"";
		else
			*stream << " fill-rule=\"nonzero\"";
	}

	*stream << " />" << endl;
}

void
SvgExport::visitVPath( VPath& composite )
{
	m_trans->visitVPath( composite );
	writePathToStream( composite, getID( &composite ), m_body, m_indent );
	m_trans->visitVPath( composite );
}

void
SvgExport::visitVSubpath( VSubpath& )
{
}

QString createUID()
{
	static unsigned int nr = 0;

	return "defitem" + QString().setNum( nr++ );
}

void
SvgExport::getColorStops( const Q3PtrVector<VColorStop> &colorStops )
{
	m_indent2++;
	for( unsigned int i = 0; i < colorStops.count() ; i++ )
	{
		printIndentation( m_defs, m_indent2 );
		*m_defs << "<stop stop-color=\"";
		getHexColor( m_defs, colorStops.at( i )->color );
		*m_defs << "\" offset=\"" << QString().setNum( colorStops.at( i )->rampPoint );
		*m_defs << "\" stop-opacity=\"" << colorStops.at( i )->color.opacity() << "\"" << " />" << endl;
	}
	m_indent2--;
}

void
SvgExport::getGradient( const VGradient& grad )
{
	QString uid = createUID();
	if( grad.type() == VGradient::linear )
	{
		printIndentation( m_defs, m_indent2 );
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

		printIndentation( m_defs, m_indent2 );
		*m_defs << "</linearGradient>" << endl;
		*m_body << "url(#" << uid << ")";
	}
	else if( grad.type() == VGradient::radial )
	{
		// do radial grad
		printIndentation( m_defs, m_indent2 );
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

		printIndentation( m_defs, m_indent2 );
		*m_defs << "</radialGradient>" << endl;
		*m_body << "url(#" << uid << ")";
	}
	// gah! pointless abbreviation of conical to conic
	else if( grad.type() == VGradient::conic )
	{
		// fake conical grad as radial.  
		// fugly but better than data loss.  
		printIndentation( m_defs, m_indent2 );
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

		printIndentation( m_defs, m_indent2 );
		*m_defs << "</radialGradient>" << endl;
		*m_body << "url(#" << uid << ")";
	}
}

// better than nothing
void
SvgExport::getPattern( const VPattern & )
{
	QString uid = createUID();
	printIndentation( m_defs, m_indent2 );
	*m_defs << "<pattern id=\"" << uid << "\" ";
	*m_defs << "width=\"" << "\" ";
	*m_defs << "height=\"" << "\" ";
	*m_defs << "patternUnits=\"userSpaceOnUse\" ";
	*m_defs << "patternContentUnits=\"userSpaceOnUse\" "; 
	*m_defs << " />" << endl;
	// TODO: insert hard work here ;)
	printIndentation( m_defs, m_indent2 );
	*m_defs << "</pattern>" << endl;
	*m_body << "url(#" << uid << ")";
}

void
SvgExport::getFill( const VFill& fill, QTextStream *stream )
{
	*stream << " fill=\"";
	if( fill.type() == VFill::none )
		*stream << "none";
	else if( fill.type() == VFill::grad )
		getGradient( fill.gradient() );
	else if( fill.type() == VFill::patt )
		getPattern( fill.pattern() );
	else
		getHexColor( stream, fill.color() );
	*stream << "\"";

	if( fill.color().opacity() != m_gc.current()->fill.color().opacity() )
		*stream << " fill-opacity=\"" << fill.color().opacity() << "\"";
}

void
SvgExport::getStroke( const VStroke& stroke, QTextStream *stream )
{
	if( stroke.type() != m_gc.current()->stroke.type() )
	{
		*stream << " stroke=\"";
		if( stroke.type() == VStroke::none )
			*stream << "none";
		else if( stroke.type() == VStroke::grad )
			getGradient( stroke.gradient() );
		else
			getHexColor( stream, stroke.color() );
		*stream << "\"";
	}

	if( stroke.color().opacity() != m_gc.current()->stroke.color().opacity() )
		*stream << " stroke-opacity=\"" << stroke.color().opacity() << "\"";

	if( stroke.lineWidth() != m_gc.current()->stroke.lineWidth() )
		*stream << " stroke-width=\"" << stroke.lineWidth() << "\"";

	if( stroke.lineCap() != m_gc.current()->stroke.lineCap() )
	{
		if( stroke.lineCap() == VStroke::capButt )
			*stream << " stroke-linecap=\"butt\"";
		else if( stroke.lineCap() == VStroke::capRound )
			*stream << " stroke-linecap=\"round\"";
		else if( stroke.lineCap() == VStroke::capSquare )
			*stream << " stroke-linecap=\"square\"";
	}

	if( stroke.lineJoin() != m_gc.current()->stroke.lineJoin() )
	{
		if( stroke.lineJoin() == VStroke::joinMiter )
		{
			*stream << " stroke-linejoin=\"miter\"";
			*stream << " stroke-miterlimit=\"" << stroke.miterLimit() << "\"";
		}
		else if( stroke.lineJoin() == VStroke::joinRound )
			*stream << " stroke-linejoin=\"round\"";
		else if( stroke.lineJoin() == VStroke::joinBevel )
				*stream << " stroke-linejoin=\"bevel\"";
	}

	// dash
	if( stroke.dashPattern().array().count() > 0 )
	{
		*stream << " stroke-dashoffset=\"" << stroke.dashPattern().offset() << "\"";
		*stream << " stroke-dasharray=\" ";

		Q3ValueListConstIterator<float> itr;
		for(itr = stroke.dashPattern().array().begin(); itr != stroke.dashPattern().array().end(); ++itr )
		{
			*stream << *itr << " ";
		}
		*stream << "\"";
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
SvgExport::visitVText( VText& text )
{
	VPath path( 0L );
	path.combinePath( text.basePath() );

	m_trans->visitVPath( path );

	QString id = createUID();
	writePathToStream( path, " id=\""+ id + "\"", m_defs, m_indent2 );

	printIndentation( m_body, m_indent++ );
	*m_body << "<text" << getID( &text );
	//*m_body << " transform=\"scale(1, -1) translate(0, -" << text.document()->height() << ")\"";
	getFill( *( text.fill() ), m_body );
	getStroke( *( text.stroke() ), m_body );

	*m_body << " font-family=\"" << text.font().family() << "\"";
	*m_body << " font-size=\"" << text.font().pointSize() << "\"";
	if( text.font().bold() )
		*m_body << " font-weight=\"bold\"";
	if( text.font().italic() )
		*m_body << " font-style=\"italic\"";
	if( text.alignment() == VText::Center )
		*m_body << " text-anchor=\"middle\"";
	else if( text.alignment() == VText::Right )
		*m_body << " text-anchor=\"end\"";

	*m_body << ">" << endl;
	
	printIndentation( m_body, m_indent );
	*m_body << "<textPath xlink:href=\"#" << id << "\"";
	if( text.offset() > 0.0 )
		*m_body << " startOffset=\"" << text.offset() * 100.0 << "%\"";	
	*m_body << ">";
	*m_body << text.text();
	*m_body << "</textPath>" << endl;
	printIndentation( m_body, --m_indent );
	*m_body << "</text>" << endl;
}

#include "svgexport.moc"

