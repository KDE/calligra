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
		( rect.right() - rect.left() ) <<
		"\" height=\"" <<
		( rect.bottom() - rect.top() ) <<"\">"
	<< endl;

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


/*
	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "SEGMENTS" )
				exportSegments( s, e );
			if( e.tagName() == "FILL" )
			{
				getFill( s, e );

				// i think this is right?
				if( fill_rule == 0 )
				{
					*m_stream << " fill-rule=\"evenodd\"";
				}
				else
				{
					*m_stream << " fill-rule=\"nonzero\"";
				}
			}
			if( e.tagName() == "STROKE" )
				getStroke( s, e );
		}
	}
*/


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
				*m_stream <<
					itr.current()->ctrlPoint1().x() << " " <<
					itr.current()->ctrlPoint1().y() << " " <<
					itr.current()->ctrlPoint2().x() << " " <<
					itr.current()->ctrlPoint2().y() << " " <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y() << " " <<
					"c\n";
			break;
			case segment_curve1:
				*m_stream <<
					itr.current()->knot1().x() << " " <<
					itr.current()->knot1().y() << " " <<
					itr.current()->ctrlPoint2().x() << " " <<
					itr.current()->ctrlPoint2().y() << " " <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y() << " " <<
					"c\n";
			break;
			case segment_curve2:
				*m_stream <<
					itr.current()->ctrlPoint1().x() << " " <<
					itr.current()->ctrlPoint1().y() << " " <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y() << " " <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y() << " " <<
					"c\n";
			break;
			case segment_line:
				*m_stream <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y() << " " <<
					"l\n";
			break;
			case segment_begin:
				*m_stream <<
					itr.current()->knot2().x() << " " <<
					itr.current()->knot2().y() << " " <<
					"m\n";
			break;
			case segment_end:
			break;
		}
	}



/*
	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			// convert the varoius karbon curves to svg "C" curves
			if( e.tagName() == "CURVE" )
			{
			*m_stream <<
				"C" <<
				e.attribute( "x1" ) << "," <<
				e.attribute( "y1" ) << " " <<
				e.attribute( "x2" ) << "," <<
				e.attribute( "y2" ) << " " <<
				e.attribute( "x3" ) << "," <<
				e.attribute( "y3" ) << " ";
			}
			if( e.tagName() == "CURVE1" )
			{
			*m_stream <<
				"C" <<
				e.attribute( "x1" ) << "," <<
				e.attribute( "y1" ) << " " <<
				e.attribute( "x1" ) << "," <<
				e.attribute( "y1" ) << " " <<
				e.attribute( "x3" ) << "," <<
				e.attribute( "y3" ) << " ";
			}
			if( e.tagName() == "CURVE2" )
			{
			*m_stream <<
				"C" <<
				e.attribute( "x1" ) << "," <<
				e.attribute( "y1" ) << " " <<
				e.attribute( "x3" ) << "," <<
				e.attribute( "y3" ) << " " <<
				e.attribute( "x3" ) << "," <<
				e.attribute( "y3" ) << " ";
			}
			else if( e.tagName() == "LINE" )
			{
				*m_stream <<
					"L" <<
					e.attribute( "x" ) << " " <<
					e.attribute( "y" ) << " ";
			}
			else if( e.tagName() == "MOVE" )
			{
				*m_stream <<
					"M" <<
					e.attribute( "x" ) << " " <<
					e.attribute( "y" ) << " ";
			}
		}
	}
*/

	if( segmentList.isClosed() )
		*m_stream << "Z";

	*m_stream << "\"";
}

void
SvgExport::getFill( const VFill& fill )
{
/*
	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "COLOR" )
			{

				// make sure getHexColor returns something
				// shouldn't be needed really
				if( !e.attribute( "colorSpace" ).isNull() )
				{
					*m_stream << " fill=\"";
					getHexColor( s, e );
					*m_stream << "\"";
				}

				if( !e.attribute( "opacity" ).isNull() )
				{
					*m_stream << " fill-opacity=\"" << e.attribute( "opacity" ) << "\"";
				}
			}
		}
	}
*/
}

void
SvgExport::getStroke( const VStroke& stroke )
{
/*
	if( !node.attribute( "lineWidth" ).isNull() )
	{
		*m_stream << " stroke-width=\"" << node.attribute( "lineWidth" ) << "\"";
	}

	if( !node.attribute( "lineCap" ).isNull() )
	{
		if( node.attribute( "lineCap" ).toInt() == 0 )
		{
			*m_stream << " stroke-linecap=\"butt\"";
		}
		else if( node.attribute( "lineCap" ).toInt() == 1 )
		{
			*m_stream << " stroke-linecap=\"round\"";
		}
		else if( node.attribute( "lineCap" ).toInt() == 2 )
		{
			*m_stream << " stroke-linecap=\"square\"";
		}
	}

	if( !node.attribute( "lineJoin" ).isNull() )
	{
		if( node.attribute( "lineJoin" ).toInt() == 0 )
		{
			*m_stream << " stroke-linejoin=\"miter\"";
		}
		else if( node.attribute( "lineJoin" ).toInt() == 1 )
		{
			*m_stream << " stroke-linejoin=\"round\"";
		}
		else if( node.attribute( "lineJoin" ).toInt() == 2 )
		{
			*m_stream << " stroke-linejoin=\"bevel\"";
		}
	}

	if( !node.attribute( "miterLimit" ).isNull() )
	{
		*m_stream << " stroke-miterlimit=\"" << node.attribute( "miterLimit" ) << "\"";
	}

	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "COLOR" )
			{

				// make sure getHexColor returns something
				// shouldn't be needed really
				if( !e.attribute( "colorSpace" ).isNull() )
				{
					*m_stream << " stroke=\"";
					getHexColor( s, e );
					*m_stream << "\"";
				}

				if( !e.attribute( "opacity" ).isNull() )
				{
					*m_stream << " stroke-opacity=\"" << e.attribute( "opacity" ) << "\"";
				}
			}
			else if( e.tagName() == "DASHPATTERN" )
			{
				*m_stream << " stroke-dashoffset=\"" << e.attribute( "offset" ) << "\"";

				QDomNodeList dashlist = e.childNodes();
				*m_stream << " stroke-dasharray=\" ";
				for( uint j = 0; j < dashlist.count(); ++j )
				{
					QDomElement e = dashlist.item( j ).toElement();
					if( e.tagName() == "DASH" )
					{
						*m_stream << e.attribute( "l", "0.0" ).toFloat() << " ";
					}
				}
				*m_stream << "\"";
			}
		}
	}
*/
}

void
SvgExport::getHexColor( const VColor& color )
{
/*
	// Convert the various color-spaces to hex

	QString Output;

	// rgb
	if( node.attribute( "colorSpace" ).toInt() == 0 )
	{
		Output.sprintf( "#%02x%02x%02x", int( node.attribute( "v1" ).toFloat() * 255 ), int( node.attribute( "v2" ).toFloat() * 255 ), int( node.attribute( "v3" ).toFloat() * 255 ) );
	}

	// cmyk
	else if( node.attribute( "colorSpace" ).toInt() == 1 )
	{
		Output.sprintf( "#%02x%02x%02x", int( ( 1 - node.attribute( "v1" ).toFloat() - node.attribute( "v4" ).toFloat() ) * 255 ), int( ( 1 - node.attribute( "v2" ).toFloat() - node.attribute( "v4" ).toFloat() ) * 255 ), int( ( 1 - node.attribute( "v3" ).toFloat() - node.attribute( "v4" ).toFloat() ) * 255 ) );
	}

	// hsb
	else if( node.attribute( "colorSpace" ).toInt() == 2 )
	{
		// maybe do this manually - or could it stay like this?
		QColor hsvColor;
		int rComponent;
		int gComponent;
		int bComponent;
		hsvColor.setHsv( int( node.attribute( "v1" ).toFloat() * 359 ), int( node.attribute( "v2" ).toFloat() * 255 ), int( node.attribute( "v3" ).toFloat() * 255 ) );
		hsvColor.rgb(&rComponent, &gComponent, &bComponent);

		Output.sprintf( "#%02x%02x%02x", rComponent, gComponent, bComponent );
	}

	// grey
	else if( node.attribute( "colorSpace" ).toInt() == 3 )
	{
		Output.sprintf( "#%02x%02x%02x", int( node.attribute( "v" ).toFloat() * 255 ), int( node.attribute( "v" ).toFloat() * 255 ), int( node.attribute( "v" ).toFloat() * 255 ) );
	}

	*m_stream << Output;
*/
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

