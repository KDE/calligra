/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#include <qdom.h>
#include <qcstring.h>
#include <qfile.h>
#include <qstring.h>
#include <qcolor.h>

#include <kgenericfactory.h>
#include <koFilter.h>
#include <koFilterChain.h>
#include <koStore.h>

#include <kdebug.h>

#include "svgexport.h"


class SvgExportFactory : KGenericFactory<SvgExport, KoFilter>
{
public:
	SvgExportFactory( void )
		: KGenericFactory<SvgExport, KoFilter>( "karbonsvgexport" )
	{}

protected:
	virtual void setupTranslations( void )
	{
		KGlobal::locale()->insertCatalogue( "karbonsvgfilter" );
	}
};

K_EXPORT_COMPONENT_FACTORY( libkarbonsvgexport, SvgExportFactory() );

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

	KoStore storeIn( m_chain->inputFile(), KoStore::Read );
	if( !storeIn.open( "root" ) )
	{
		storeIn.close();
		return KoFilter::StupidError;
	}

	QFile fileOut( m_chain->outputFile() );
	if( !fileOut.open( IO_WriteOnly ) ) {
		storeIn.close();
		return KoFilter::StupidError;
	}

	QByteArray byteArrayIn = storeIn.read( storeIn.size() );
	storeIn.close();

	QDomDocument domIn;
	domIn.setContent( byteArrayIn );
	QDomElement docNode = domIn.documentElement();

	QTextStream s( &fileOut );


	// parse dom-tree:
	QDomNodeList list = domIn.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "DOC" )
				exportDocument( s, e );
		}
	}

	fileOut.close();
	return KoFilter::OK;
}

void
SvgExport::exportDocument( QTextStream& s, const QDomElement& node )
{
	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "LAYER" )
				exportLayer( s, e );
		}
	}
}

void
SvgExport::exportLayer( QTextStream& s, const QDomElement& node )
{
	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "PATH" )
				exportPath( s, e );
		}
	}
}

void
SvgExport::exportPath( QTextStream& s, const QDomElement& node )
{

	s << "<path" << endl;
	
	fill_rule = node.attribute("fillRule").toInt();

	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "SEGMENTS" )
				exportSegments( s, e );
			if( e.tagName() == "FILL" )
				exportFill( s, e );
			if( e.tagName() == "STROKE" )
				exportStroke( s, e );
		}
	}

	if( node.attribute( "closed" ) != 0 )
		s << " />" << endl;

}

void
SvgExport::exportSegments( QTextStream& s, const QDomElement& node )
{

	s << " d=\"" << endl;

	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "CURVE" )
			{
				s <<	
					"C " <<
					e.attribute( "x1" ) << " " <<
					e.attribute( "y1" ) << " " <<
					e.attribute( "x2" ) << " " <<
					e.attribute( "y2" ) << " " <<
					e.attribute( "x3" ) << " " <<
					e.attribute( "y3" ) << " " << 
				endl;
			}
			else if( e.tagName() == "LINE" )
			{
				s <<	
					"L " <<
					e.attribute( "x" ) << " " <<
					e.attribute( "y" ) << " " << 
				endl;
			}
			else if( e.tagName() == "MOVE" )
			{
				s <<	
					"M " <<
					e.attribute( "x" ) << " " <<
					e.attribute( "y" ) << " " << 
				endl;
			}
		}
	}
	
	s << "\"" << endl;
}

void
SvgExport::exportFill( QTextStream& s, const QDomElement& node)
{

	if(fill_rule == 1)
	{
		s <<
			" fill-rule=\"evenodd\"" <<
		endl;
	}

	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "COLOR" )
			{

				if(!e.attribute("colorSpace").isNull()) // make sure getHexColor returns something
				{					// shouldn't be needed really
					s << " fill=\"" << endl;
					getHexColor(s, e);
					s << "\"" << endl;
				}

				if(!e.attribute("opacity").isNull())
				{
					s <<
						" fill-opacity=\"" << e.attribute("opacity") << "\"" <<
					endl;
				}
			}
		}
	}

}

void
SvgExport::exportStroke( QTextStream& s, const QDomElement& node)
{

	if(!node.attribute("lineWidth").isNull())
	{
		s <<
			" stroke-width=\"" << node.attribute("lineWidth") << "\"" <<
		endl;
	}

// 	if(!node.attribute("lineCap").isNull())
// 	{
// 		s <<
// 			" stroke-linecap=\"" << node.attribute("lineCap") << "\"" <<
// 		endl;
// 	}

// 	if(!node.attribute("lineJoin").isNull())
// 	{
// 		s <<
// 			" stroke-linejoin=\"" << node.attribute("lineJoin") << "\"" <<
// 		endl;
//
// 	}

	if(!node.attribute("miterLimit").isNull())
	{
		s <<
			" stroke-miterlimit=\"" << node.attribute("miterLimit") << "\"" <<
		endl;
	}

	QDomNodeList list = node.childNodes();
	for( uint i = 0; i < list.count(); ++i )
	{
		if( list.item( i ).isElement() )
		{
			QDomElement e = list.item( i ).toElement();

			if( e.tagName() == "COLOR" )
			{
				if(!e.attribute("colorSpace").isNull()) // make sure getHexColor returns something
				{					// shouldn't be needed really
					s << " stroke=\"" << endl;
					getHexColor(s, e);
					s << "\"" << endl;
				}

				if(!e.attribute("opacity").isNull())
				{
					s <<
						" stroke-opacity=\"" << e.attribute("opacity") << "\"" <<
					endl;
				}
			}
		}
	}

}

void
SvgExport::getHexColor( QTextStream& s, const QDomElement& node)
{


	if(node.attribute("colorSpace").toInt() == 0) // rgb
	{
		s << printf( "#%02x%02x%02x", int(node.attribute("v1").toFloat()*255), int(node.attribute("v2").toFloat()*255), int(node.attribute("v3").toFloat()*255) ) << endl;
	}
	else if(node.attribute("colorSpace").toInt() == 1) // cmyk
	{
		s << printf( "#%02x%02x%02x", int((1 - node.attribute("v1").toFloat() - node.attribute("v4").toFloat())*255), int((1 - node.attribute("v2").toFloat() - node.attribute("v4").toFloat())*255), int((1 - node.attribute("v3").toFloat() - node.attribute("v4").toFloat())*255) ) << endl;
	}
	else if(node.attribute("colorSpace").toInt() == 2) // hsb
	{
		// maybe do this manually - or could it stay like this?
		QColor hsvColor;
		int rComponent;
		int gComponent;
		int bComponent;
		hsvColor.setHsv(int(node.attribute("v1").toFloat()*359), int(node.attribute("v2").toFloat()*255), int(node.attribute("v3").toFloat()*255));
		hsvColor.rgb(&rComponent, &gComponent, &bComponent);

		s << printf( "#%02x%02x%02x", rComponent, gComponent, bComponent ) << endl;
	}
	else if(node.attribute("colorSpace").toInt() == 3) // grey
	{
		s << printf( "#%02x%02x%02x", int(node.attribute("v").toFloat()*255), int(node.attribute("v").toFloat()*255), int(node.attribute("v").toFloat()*255) ) << endl;
	}

}

#include "svgexport.moc"

