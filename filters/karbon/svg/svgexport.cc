/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
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
#include "vdashpattern.h"
#include "vdocument.h"
#include "vfill.h"
#include "vgradient.h"
#include "vgroup.h"
#include "vlayer.h"
#include "vpath.h"
#include "vsegment.h"
#include "vsegmentlist.h"
#include "vselection.h"
#include "vstroke.h"
//#include "vtext.h"

#include <kdebug.h>


typedef KGenericFactory<SvgExport, KoFilter> SvgExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonsvgexport, SvgExportFactory( "karbonsvgexport" ) );


SvgExport::SvgExport( KoFilter*, const char*, const QStringList& )
	: KoFilter()
{
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

	// get the bounding box of all selected objects:
	const KoRect& rect = document.selection()->boundingBox();

	// standard header:
	*m_defs <<
		"<?xml version=\"1.0\" standalone=\"no\"?>\n" <<
		"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" " <<
		"\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">"
	<< endl;

	*m_defs <<
		"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" <<
		rect.right() << "\" height=\"" << rect.bottom() << "\">" << endl;
	*m_defs << "<defs>" << endl;

	// we dont need the selection anymore:
	document.selection()->clear();

	// export layers:
	VVisitor::visitVDocument( document );

	// end tag:
	*m_defs << "</defs>" << endl;
	*m_body << "</svg>" << endl;
}

void
SvgExport::visitVPath( VPath& path )
{
	*m_body << "<path";

	VVisitor::visitVPath( path );

	getFill( *( path.fill() ) );
	getStroke( *( path.stroke() ) );

	*m_body << " />" << endl;

}

void
SvgExport::visitVSegmentList( VSegmentList& segmentList )
{

	*m_body << " d=\"";

	// export segments:
	VSegmentListIterator itr( segmentList );
	for( ; itr.current(); ++itr )
	{
		switch( itr.current()->type() )
		{
			case VSegment::curve:
				*m_body << " C " <<
					itr.current()->ctrlPoint1().x() << " " <<
					itr.current()->ctrlPoint1().y() << " " <<
					itr.current()->ctrlPoint2().x() << " " <<
					itr.current()->ctrlPoint2().y() << " " <<
					itr.current()->knot().x() << " " <<
					itr.current()->knot().y();
			break;
			case VSegment::line:
				*m_body << " L " <<
					itr.current()->knot().x() << " " <<
					itr.current()->knot().y();
			break;
			case VSegment::begin:
				*m_body << " M " <<
					itr.current()->knot().x() << " " <<
					itr.current()->knot().y();
			break;
			default:
			break;
		}
	}

	if( segmentList.isClosed() )
		*m_body << "Z";

	*m_body << "\"";
}

QString createUID()
{
	static unsigned int nr = 0;

	return "defitem" + QString().setNum( nr++ );
}

void
SvgExport::getColorStops( const QValueList<VGradient::VColorStop> &colorStops )
{
	QValueListConstIterator<VGradient::VColorStop> itr;
	for( itr = colorStops.begin(); itr != colorStops.end(); ++itr )
	{
		*m_defs << "<stop stop-color=\"";
		getHexColor( m_defs, (*itr).color );
		*m_defs << "\" offset=\"";
		*m_defs << QString().setNum( (*itr).rampPoint ) << "\" />" << endl;
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

void
SvgExport::getFill( const VFill& fill )
{
	*m_body << " fill=\"";
	if( fill.type() == VFill::none )
		*m_body << "none";
	else if( fill.type() == VFill::grad )
		getGradient( fill.gradient() );
	else 
		getHexColor( m_body, fill.color() );
	*m_body << "\"";
	*m_body << " fill-opacity=\"" << fill.color().opacity() << "\"";
	if( fill.fillRule() == VFill::evenOdd )
		*m_body << " fill-rule=\"evenodd\"";
	else
		*m_body << " fill-rule=\"nonzero\"";
}

void
SvgExport::getStroke( const VStroke& stroke )
{
	*m_body << " stroke=\"";
	if( stroke.type() == VStroke::none )
		*m_body << "none";
	else if( stroke.type() == VStroke::grad )
		getGradient( stroke.gradient() );
	else
		getHexColor( m_body, stroke.color() );
	*m_body << "\"";
	*m_body << " stroke-opacity=\"" << stroke.color().opacity() << "\"";

	*m_body << " stroke-width=\"" << stroke.lineWidth() << "\"";

	if( stroke.lineCap() == VStroke::capButt )
		*m_body << " stroke-linecap=\"butt\"";
	else if( stroke.lineCap() == VStroke::capRound )
		*m_body << " stroke-linecap=\"round\"";
	else if( stroke.lineCap() == VStroke::capSquare )
			*m_body << " stroke-linecap=\"square\"";

	if( stroke.lineJoin() == VStroke::joinMiter )
	{
		*m_body << " stroke-linejoin=\"miter\"";
		*m_body << " stroke-miterlimit=\"" << stroke.miterLimit() << "\"";
	}
	else if( stroke.lineJoin() == VStroke::joinRound )
		*m_body << " stroke-linejoin=\"round\"";
	else if( stroke.lineJoin() == VStroke::joinBevel )
			*m_body << " stroke-linejoin=\"bevel\"";

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

	float v1, v2, v3, v4;
	color.values( &v1, &v2, &v3, &v4 );
	// rgb
	if( color.colorSpace() == VColor::rgb )
	{
		Output.sprintf( "#%02x%02x%02x", int( v1 * 255 ), int( v2 * 255 ), int( v3 * 255 ) );
	}

	// cmyk
	else if( color.colorSpace() == VColor::cmyk )
	{
		Output.sprintf( "#%02x%02x%02x", int( ( 1 - v1 - v4 ) * 255 ), int( ( 1 - v2 - v4 ) * 255 ), int( ( 1 - v3 - v4 ) * 255 ) );
	}

	// hsb
	else if( color.colorSpace() == VColor::hsb )
	{
		// maybe do this manually - or could it stay like this?
		QColor hsvColor;
		int rComponent;
		int gComponent;
		int bComponent;
		hsvColor.setHsv( int( v1 * 359 ), int( v2 * 255 ), int( v3 * 255 ) );
		hsvColor.rgb(&rComponent, &gComponent, &bComponent);

		Output.sprintf( "#%02x%02x%02x", rComponent, gComponent, bComponent );
	}

	// grey
	//else if( color.colorSpace() == VColor::gray )
	//{
	//	Output.sprintf( "#%02x%02x%02x", int( node.attribute( "v" ).toFloat() * 255 ), int( node.attribute( "v" ).toFloat() * 255 ), int( node.attribute( "v" ).toFloat() * 255 ) );
	//}

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

