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


	// load the document and export it:
	VDocument doc;
	doc.load( docNode );
	doc.accept( *this );


	fileOut.close();

	delete m_stream;
	delete storeIn;

	return KoFilter::OK;
}

void
SvgExport::visitVDocument( VDocument& document )
{
	// select all objects:
	document.select();

	// get the bounding box of all selected objects:
	const KoRect& rect = document.selection()->boundingBox();

	// standard header:
	*m_stream <<
		"<?xml version=\"1.0\" standalone=\"no\"?>\n" <<
		"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20010904//EN\" " <<
		"\"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">"
	<< endl;

	*m_stream <<
		"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" <<
		rect.right() << "\" height=\"" << rect.bottom() << "\">" << endl;

	// we dont need the selection anymore:
	document.deselect();


	// export layers:
	VLayerListIterator itr( document.layers() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );


	// end tag:
	*m_stream << "</svg>" << endl;
}

void
SvgExport::visitVGroup( VGroup& group )
{
	// export objects:
	VObjectListIterator itr( group.objects() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );
}

void
SvgExport::visitVLayer( VLayer& layer )
{
	// export objects:
	VObjectListIterator itr( layer.objects() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );
}

void
SvgExport::visitVPath( VPath& path )
{
	*m_stream << "<path";

	// export segmentlists:
	VSegmentListListIterator itr( path.segmentLists() );
	for( ; itr.current(); ++itr )
		itr.current()->accept( *this );

	getFill( *( path.fill() ) );
	getStroke( *( path.stroke() ) );

	*m_stream << " />" << endl;

}

void
SvgExport::visitVSegmentList( VSegmentList& segmentList )
{

	*m_stream << " d=\"";

	// export segments:
	VSegmentListIterator itr( segmentList );
	for( ; itr.current(); ++itr )
	{
		switch( itr.current()->type() )
		{
			case segment_curve:
				*m_stream << " C " <<
					itr.current()->ctrlPoint1().x() << " " <<
					itr.current()->ctrlPoint1().y() << " " <<
					itr.current()->ctrlPoint2().x() << " " <<
					itr.current()->ctrlPoint2().y() << " " <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y();
			break;
			case segment_curve1:
				*m_stream << " C " <<
					itr.current()->knot1().x() << " " <<
					itr.current()->knot1().y() << " " <<
					itr.current()->ctrlPoint2().x() << " " <<
					itr.current()->ctrlPoint2().y() << " " <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y();
			break;
			case segment_curve2:
				*m_stream << " C " << 
					itr.current()->ctrlPoint1().x() << " " <<
					itr.current()->ctrlPoint1().y() << " " <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y() << " " <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y();
			break;
			case segment_line:
				*m_stream << " L " <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y();
			break;
			case segment_begin:
				*m_stream << " M " <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y();
			break;
			case segment_end:
			break;
		}
	}

	if( segmentList.isClosed() )
		*m_stream << "Z";

	*m_stream << "\"";
}

void
SvgExport::getFill( const VFill& fill )
{
	*m_stream << " fill=\"";
	if( fill.type() == fill_none )
		*m_stream << "none";
	else // TODO : gradient fills
		getHexColor( fill.color() );
	*m_stream << "\"";
	*m_stream << " fill-opacity=\"" << fill.color().opacity() << "\"";
	if( fill.fillRule() == fillrule_evenOdd )
		*m_stream << " fill-rule=\"evenodd\"";
	else
		*m_stream << " fill-rule=\"nonzero\"";
}

void
SvgExport::getStroke( const VStroke& stroke )
{
	*m_stream << " stroke=\"";
	if( stroke.type() == stroke_none )
		*m_stream << "none";
	else // TODO : gradient fills
		getHexColor( stroke.color() );
	*m_stream << "\"";
	*m_stream << " stroke-opacity=\"" << stroke.color().opacity() << "\"";

	*m_stream << " stroke-width=\"" << stroke.lineWidth() << "\"";

	if( stroke.lineCap() == cap_butt )
		*m_stream << " stroke-linecap=\"butt\"";
	else if( stroke.lineCap() == cap_round )
		*m_stream << " stroke-linecap=\"round\"";
	else if( stroke.lineCap() == cap_square )
			*m_stream << " stroke-linecap=\"square\"";

	if( stroke.lineJoin() == join_miter )
	{
		*m_stream << " stroke-linejoin=\"miter\"";
		*m_stream << " stroke-miterlimit=\"" << stroke.miterLimit() << "\"";
	}
	else if( stroke.lineJoin() == join_round )
		*m_stream << " stroke-linejoin=\"round\"";
	else if( stroke.lineJoin() == join_bevel )
			*m_stream << " stroke-linejoin=\"bevel\"";

	// dash
	if( stroke.dashPattern().array().count() > 0 )
	{
		*m_stream << " stroke-dashoffset=\"" << stroke.dashPattern().offset() << "\"";
		*m_stream << " stroke-dasharray=\" ";

		QValueListConstIterator<float> itr;
		for(itr = stroke.dashPattern().array().begin(); itr != stroke.dashPattern().array().end(); ++itr )
		{
			*m_stream << *itr << " ";
		}
		*m_stream << "\"";
	}
}

void
SvgExport::getHexColor( const VColor& color )
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

	*m_stream << Output;
}

void
SvgExport::visitVText( VText& /*text*/ )
{
/*
	// TODO: set placement once karbon supports it

	*m_stream << "<text";

	if( !node.attribute( "size" ).isNull() )
	{
		*m_stream << " font-size=\"" << node.attribute( "size" ) << "\"";
	}

	if( !node.attribute( "family" ).isNull() )
	{
		*m_stream << " font-family=\"" << node.attribute( "family" ) << "\"";
	}

	if( !node.attribute( "bold" ).isNull() )
	{
		*m_stream << " font-weight=\"bold\"";
	}

	if( !node.attribute( "italic" ).isNull() )
	{
		*m_stream << " font-style=\"italic\"";
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

	*m_stream << ">";


	if( !node.attribute( "text" ).isNull() )
	{
		*m_stream << node.attribute( "text" );
	}

	*m_stream << "</text>" << endl;
*/
}

#include "svgexport.moc"

