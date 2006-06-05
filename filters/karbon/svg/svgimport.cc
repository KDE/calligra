/* This file is part of the KDE project
   Copyright (C) 2002, 2003, The Karbon Developers

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

#include <svgimport.h>
#include "color.h"
#include <KoFilterChain.h>
#include <KoPageLayout.h>
#include <kgenericfactory.h>
#include <kdebug.h>
#include <KoUnit.h>
#include <KoGlobal.h>
#include <shapes/vellipse.h>
#include <shapes/vrectangle.h>
#include <shapes/vpolygon.h>
#include <commands/vtransformcmd.h>
#include <core/vsegment.h>
#include <core/vtext.h>
#include <core/vglobal.h>
#include <core/vgroup.h>
#include <core/vimage.h>
#include <core/vlayer.h>
#include <qcolor.h>
#include <qfile.h>
#include <kfilterdev.h>

typedef KGenericFactory<SvgImport, KoFilter> SvgImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkarbonsvgimport, SvgImportFactory( "kofficefilters" ) )

SvgImport::SvgImport(KoFilter *, const char *, const QStringList&) :
    KoFilter(),
    outdoc( "DOC" )
{
	m_gc.setAutoDelete( true );
}

SvgImport::~SvgImport()
{
}

KoFilter::ConversionStatus SvgImport::convert(const QCString& from, const QCString& to)
{
	// check for proper conversion
	if( to != "application/x-karbon" || from != "image/svg+xml" )
		return KoFilter::NotImplemented;

	//Find the last extension
	QString strExt;
	QString fileIn ( m_chain->inputFile() );
	const int result=fileIn.findRev('.');
	if (result>=0)
		strExt=fileIn.mid(result).lower();

	QString strMime; // Mime type of the compressor
	if ((strExt==".gz")      //in case of .svg.gz (logical extension)
       ||(strExt==".svgz")) //in case of .svgz (extension used prioritary)
		strMime="application/x-gzip"; // Compressed with gzip
	else if (strExt==".bz2") //in case of .svg.bz2 (logical extension)
		strMime="application/x-bzip2"; // Compressed with bzip2
	else
		strMime="text/plain";

	/*kdDebug(30514) << "File extension: -" << strExt << "- Compression: " << strMime << endl;*/

	QIODevice* in = KFilterDev::deviceForFile(fileIn,strMime);

	if (!in->open(IO_ReadOnly))
	{
		kdError(30514) << "Cannot open file! Aborting!" << endl;
		delete in;
		return KoFilter::FileNotFound;
	}

	int line, col;
	QString errormessage;

	const bool parsed=inpdoc.setContent( in, &errormessage, &line, &col );

	in->close();
	delete in;

	if ( ! parsed )
	{
		kdError(30514) << "Error while parsing file: "
		        << "at line " << line << " column: " << col
		        << " message: " << errormessage << endl;
		// ### TODO: feedback to the user
		return KoFilter::ParsingError;
	}

	// Do the conversion!
	convert();
	// add paper info, we always need custom for svg (Rob)
	QDomElement paper = outdoc.createElement( "PAPER" );
	outdoc.documentElement().appendChild( paper );
	paper.setAttribute( "format", PG_CUSTOM );
	paper.setAttribute( "width", m_document.width() );
	paper.setAttribute( "height", m_document.height() );

	KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );
	if( !out )
	{
		kdError(30514) << "Unable to open output file!" << endl;
		return KoFilter::StorageCreationError;
	}
	QCString cstring = outdoc.toCString(); // utf-8 already
	out->writeBlock( cstring.data(), cstring.length() );

	return KoFilter::OK; // was successful
}

void SvgImport::convert()
{
	SvgGraphicsContext *gc = new SvgGraphicsContext;
	QDomElement docElem = inpdoc.documentElement();
	KoRect bbox( 0, 0, 550.0, 841.0 );
	double width	= !docElem.attribute( "width" ).isEmpty() ? parseUnit( docElem.attribute( "width" ), true, false, bbox ) : 550.0;
	double height	= !docElem.attribute( "height" ).isEmpty() ? parseUnit( docElem.attribute( "height" ), false, true, bbox ) : 841.0;
	m_document.setWidth( width );
	m_document.setHeight( height );

	m_outerRect = m_document.boundingBox();

	// undo y-mirroring
	//m_debug->append(QString("%1\tUndo Y-mirroring.").arg(m_time.elapsed()));
	if( !docElem.attribute( "viewBox" ).isEmpty() )
	{
		// allow for viewbox def with ',' or whitespace
		QString viewbox( docElem.attribute( "viewBox" ) );
		QStringList points = QStringList::split( ' ', viewbox.replace( ',', ' ').simplifyWhiteSpace() );

		gc->matrix.scale( width / points[2].toFloat() , height / points[3].toFloat() );
		m_outerRect.setWidth( m_outerRect.width() * ( points[2].toFloat() / width ) );
		m_outerRect.setHeight( m_outerRect.height() * ( points[3].toFloat() / height ) );
	}

	m_gc.push( gc );
	parseGroup( 0L, docElem );
	
	QWMatrix mat;
	mat.scale( 1, -1 );
	mat.translate( 0, -m_document.height() );
	VTransformCmd trafo( 0L, mat );
	trafo.visit( m_document );
	outdoc = m_document.saveXML();
}

#define DPI 90

// Helper functions
// ---------------------------------------------------------------------------------------

double SvgImport::toPercentage( QString s )
{
	if( s.endsWith( "%" ) )
		return s.remove( '%' ).toDouble();
	else
		return s.toDouble() * 100.0;
}

double SvgImport::fromPercentage( QString s )
{
	if( s.endsWith( "%" ) )
		return s.remove( '%' ).toDouble() / 100.0;
	else
		return s.toDouble();
}

double SvgImport::getScalingFromMatrix( QWMatrix &matrix )
{
	double xscale = matrix.m11() + matrix.m12();
	double yscale = matrix.m22() + matrix.m21();
	return sqrt( xscale*xscale + yscale*yscale ) / sqrt( 2.0 );
}

// parses the number into parameter number
const char * getNumber( const char *ptr, double &number )
{
	int integer, exponent;
	double decimal, frac;
	int sign, expsign;

	exponent = 0;
	integer = 0;
	frac = 1.0;
	decimal = 0;
	sign = 1;
	expsign = 1;

	// read the sign
	if(*ptr == '+')
		ptr++;
	else if(*ptr == '-')
	{
		ptr++;
		sign = -1;
	}

	// read the integer part
	while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
		integer = (integer * 10) + *(ptr++) - '0';
	if(*ptr == '.') // read the decimals
	{
		ptr++;
		while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
			decimal += (*(ptr++) - '0') * (frac *= 0.1);
	}

	if(*ptr == 'e' || *ptr == 'E') // read the exponent part
	{
		ptr++;

		// read the sign of the exponent
		if(*ptr == '+')
			ptr++;
		else if(*ptr == '-')
		{
			ptr++;
			expsign = -1;
		}

		exponent = 0;
		while(*ptr != '\0' && *ptr >= '0' && *ptr <= '9')
		{
			exponent *= 10;
			exponent += *ptr - '0';
			ptr++;
		}
	}
	number = integer + decimal;
	number *= sign * pow( (double)10, double( expsign * exponent ) );

	return ptr;
}

void SvgImport::addGraphicContext()
{
	SvgGraphicsContext *gc = new SvgGraphicsContext;
	// set as default
	if( m_gc.current() )
		*gc = *( m_gc.current() );
	m_gc.push( gc );
}

void SvgImport::setupTransform( const QDomElement &e )
{
	SvgGraphicsContext *gc = m_gc.current();

	QWMatrix mat = VPath::parseTransform( e.attribute( "transform" ) );
	gc->matrix = mat * gc->matrix;
}

VObject* SvgImport::findObject( const QString &name, VGroup* group )
{
	if( ! group )
		return 0L;

	VObjectListIterator itr = group->objects();

	for( uint objcount = 1; itr.current(); ++itr, objcount++ )
		if( itr.current()->state() != VObject::deleted )
		{
			if( itr.current()->name() == name )
				return itr.current();
			
			if( dynamic_cast<VGroup *>( itr.current() ) )
			{
				VObject *obj = findObject( name, dynamic_cast<VGroup *>( itr.current() ) );
				if( obj )
					return obj;
			}
		}
	
	return 0L;
}

VObject* SvgImport::findObject( const QString &name )
{
	QPtrVector<VLayer> vector;
	m_document.layers().toVector( &vector );
	for( int i = vector.count() - 1; i >= 0; i-- )
	{
		if ( vector[i]->state() != VObject::deleted )
		{
			VObject* obj = findObject( name, dynamic_cast<VGroup *>( vector[i] ) );
			if( obj )
				return obj;
		}
	}
	
	return 0L;
}

SvgImport::GradientHelper* SvgImport::findGradient( const QString &id, const QString &href)
{
	// check if gradient was already parsed, and return it 
	if( m_gradients.contains( id ) )
		return &m_gradients[ id ];

	// check if gradient was stored for later parsing
	if( !m_defs.contains( id ) )
		return 0L;

	QDomElement e = m_defs[ id ];
	if(e.childNodes().count() == 0)
	{
		QString mhref = e.attribute("xlink:href").mid(1);
		
		if(m_defs.contains(mhref))
			return findGradient(mhref, id);
		else
			return 0L;
	}
	else
	{
		// ok parse gradient now
		parseGradient( m_defs[ id ], m_defs[ href ] );
	}
	
	// return successfully parsed gradient or NULL
	QString n;
	if(href.isEmpty())
		n = id;
	else
		n = href;

	if( m_gradients.contains( n ) )
		return &m_gradients[ n ];
	else
		return 0L;
}

QDomElement SvgImport::mergeStyles( const QDomElement &referencedBy, const QDomElement &referencedElement )
{
	// First use all the style attributes of the element being referenced.
	QDomElement e = referencedElement;

	// Now go through the style attributes of the element that is referencing and substitute the original ones.
	if( !referencedBy.attribute( "color" ).isEmpty() )
		e.setAttribute( "color", referencedBy.attribute( "color" ) );
	if( !referencedBy.attribute( "fill" ).isEmpty() )
		e.setAttribute( "fill", referencedBy.attribute( "fill" ) );
	if( !referencedBy.attribute( "fill-rule" ).isEmpty() )
		e.setAttribute( "fill-rule", referencedBy.attribute( "fill-rule" ) );
	if( !referencedBy.attribute( "stroke" ).isEmpty() )
		e.setAttribute( "stroke", referencedBy.attribute( "stroke" ) );
	if( !referencedBy.attribute( "stroke-width" ).isEmpty() )
		e.setAttribute( "stroke-width", referencedBy.attribute( "stroke-width" ) );
	if( !referencedBy.attribute( "stroke-linejoin" ).isEmpty() )
		e.setAttribute( "stroke-linejoin", referencedBy.attribute( "stroke-linejoin" ) );
	if( !referencedBy.attribute( "stroke-linecap" ).isEmpty() )
		e.setAttribute( "stroke-linecap", referencedBy.attribute( "stroke-linecap" ) );
	if( !referencedBy.attribute( "stroke-dasharray" ).isEmpty() )
		e.setAttribute( "stroke-dasharray", referencedBy.attribute( "stroke-dasharray" ) );
	if( !referencedBy.attribute( "stroke-dashoffset" ).isEmpty() )
		e.setAttribute( "stroke-dashoffset", referencedBy.attribute( "stroke-dashoffset" ) );
	if( !referencedBy.attribute( "stroke-opacity" ).isEmpty() )
		e.setAttribute( "stroke-opacity", referencedBy.attribute( "stroke-opacity" ) );
	if( !referencedBy.attribute( "stroke-miterlimit" ).isEmpty() )
		e.setAttribute( "stroke-miterlimit", referencedBy.attribute( "stroke-miterlimit" ) );
	if( !referencedBy.attribute( "fill-opacity" ).isEmpty() )
		e.setAttribute( "fill-opacity", referencedBy.attribute( "fill-opacity" ) );
	if( !referencedBy.attribute( "opacity" ).isEmpty() )
		e.setAttribute( "opacity", referencedBy.attribute( "opacity" ) );

	// TODO merge style attribute too.

	return e;
}


// Parsing functions
// ---------------------------------------------------------------------------------------

double SvgImport::parseUnit( const QString &unit, bool horiz, bool vert, KoRect bbox )
{
	// TODO : percentage?
	double value = 0;
	const char *start = unit.latin1();
	if(!start) {
		return 0;
	}
	const char *end = getNumber( start, value );

	if( uint( end - start ) < unit.length() )
	{
		if( unit.right( 2 ) == "pt" )
			value = ( value / 72.0 ) * DPI;
		else if( unit.right( 2 ) == "cm" )
			value = ( value / 2.54 ) * DPI;
		else if( unit.right( 2 ) == "pc" )
			value = ( value / 6.0 ) * DPI;
		else if( unit.right( 2 ) == "mm" )
			value = ( value / 25.4 ) * DPI;
		else if( unit.right( 2 ) == "in" )
			value = value * DPI;
		else if( unit.right( 2 ) == "pt" )
			value = ( value / 72.0 ) * DPI;
		else if( unit.right( 2 ) == "em" )
			value = value * m_gc.current()->font.pointSize() / ( sqrt( pow( m_gc.current()->matrix.m11(), 2 ) + pow( m_gc.current()->matrix.m22(), 2 ) ) / sqrt( 2.0 ) );
		else if( unit.right( 1 ) == "%" )
		{
			if( horiz && vert )
				value = ( value / 100.0 ) * (sqrt( pow( bbox.width(), 2 ) + pow( bbox.height(), 2 ) ) / sqrt( 2.0 ) );
			else if( horiz )
				value = ( value / 100.0 ) * bbox.width();
			else if( vert )
				value = ( value / 100.0 ) * bbox.height();
		}
	}
	/*else
	{
		if( m_gc.current() )
		{
			if( horiz && vert )
				value *= sqrt( pow( m_gc.current()->matrix.m11(), 2 ) + pow( m_gc.current()->matrix.m22(), 2 ) ) / sqrt( 2.0 );
			else if( horiz )
				value /= m_gc.current()->matrix.m11();
			else if( vert )
				value /= m_gc.current()->matrix.m22();
		}
	}*/
	return value;
}

QColor SvgImport::parseColor( const QString &rgbColor )
{
	int r, g, b;
	keywordToRGB( rgbColor, r, g, b );
	return QColor( r, g, b );
}

void SvgImport::parseColor( VColor &color, const QString &s )
{
	if( s.startsWith( "rgb(" ) )
	{
		QString parse = s.stripWhiteSpace();
		QStringList colors = QStringList::split( ',', parse );
		QString r = colors[0].right( ( colors[0].length() - 4 ) );
		QString g = colors[1];
		QString b = colors[2].left( ( colors[2].length() - 1 ) );

		if( r.contains( "%" ) )
		{
			r = r.left( r.length() - 1 );
			r = QString::number( int( ( double( 255 * r.toDouble() ) / 100.0 ) ) );
		}

		if( g.contains( "%" ) )
		{
			g = g.left( g.length() - 1 );
			g = QString::number( int( ( double( 255 * g.toDouble() ) / 100.0 ) ) );
		}

		if( b.contains( "%" ) )
		{
			b = b.left( b.length() - 1 );
			b = QString::number( int( ( double( 255 * b.toDouble() ) / 100.0 ) ) );
		}

		QColor c( r.toInt(), g.toInt(), b.toInt() );
		color.set( c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0 );
	}
	else if( s == "currentColor" )
	{
		SvgGraphicsContext *gc = m_gc.current();
		color = gc->color;
	}
	else
	{
		QString rgbColor = s.stripWhiteSpace();
		QColor c;
		if( rgbColor.startsWith( "#" ) )
			c.setNamedColor( rgbColor );
		else
			c = parseColor( rgbColor );
		color.set( c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0 );
	}
}

void SvgImport::parseColorStops( VGradient *gradient, const QDomElement &e )
{
	VColor c;
	for( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() )
	{
		QDomElement stop = n.toElement();
		if( stop.tagName() == "stop" )
		{
			float offset;
			QString temp = stop.attribute( "offset" );
			if( temp.contains( '%' ) )
			{
				temp = temp.left( temp.length() - 1 );
				offset = temp.toFloat() / 100.0;
			}
			else
				offset = temp.toFloat();

			if( !stop.attribute( "stop-color" ).isEmpty() )
				parseColor( c, stop.attribute( "stop-color" ) );
			else
			{
				// try style attr
				QString style = stop.attribute( "style" ).simplifyWhiteSpace();
				QStringList substyles = QStringList::split( ';', style );
				for( QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it )
				{
					QStringList substyle = QStringList::split( ':', (*it) );
					QString command	= substyle[0].stripWhiteSpace();
					QString params	= substyle[1].stripWhiteSpace();
					if( command == "stop-color" )
						parseColor( c, params );
					if( command == "stop-opacity" )
						c.setOpacity( params.toDouble() );
				}

			}
			if( !stop.attribute( "stop-opacity" ).isEmpty() )
				c.setOpacity( stop.attribute( "stop-opacity" ).toDouble() );
			gradient->addStop( c, offset, 0.5 );
		}
	} 
}

void SvgImport::parseGradient( const QDomElement &e , const QDomElement &referencedBy)
{
	// IMPROVEMENTS:
	// - Store the parsed colorstops in some sort of a cache so they don't need to be parsed again.
	// - A gradient inherits attributes it does not have from the referencing gradient.
	// - Gradients with no color stops have no fill or stroke.
	// - Gradients with one color stop have a solid color.
	
	SvgGraphicsContext *gc = m_gc.current();
	if( !gc ) return;

	GradientHelper gradhelper;
	gradhelper.gradient.clearStops();
	gradhelper.gradient.setRepeatMethod( VGradient::none );

	if(e.childNodes().count() == 0)
	{
		QString href = e.attribute("xlink:href").mid(1);
		
		if(href.isEmpty())
		{
			//gc->fill.setType( VFill::none ); // <--- TODO Fill OR Stroke are none
			return;
		}
		else 
		{
			// copy the referenced gradient if found
			GradientHelper *pGrad = findGradient( href );
			if( pGrad )
				gradhelper = *pGrad;
		}
	}

	// Use the gradient that is referencing, or if there isn't one, the original gradient.
	QDomElement b;
	if( !referencedBy.isNull() )
		b = referencedBy;
	else
		b = e;
	
	QString id = b.attribute("id");
	if( !id.isEmpty() )
	{ 
		// Copy existing gradient if it exists
		if( m_gradients.find( id ) != m_gradients.end() )
			gradhelper.gradient = m_gradients[ id ].gradient;
	}

	gradhelper.bbox = b.attribute( "gradientUnits" ) != "userSpaceOnUse";

	// parse color prop
	VColor c = m_gc.current()->color;
	
	if( !b.attribute( "color" ).isEmpty() )
	{
		parseColor( c, b.attribute( "color" ) );
	}
	else
	{
		// try style attr
		QString style = b.attribute( "style" ).simplifyWhiteSpace();
		QStringList substyles = QStringList::split( ';', style );
		for( QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it )
		{
			QStringList substyle = QStringList::split( ':', (*it) );
			QString command	= substyle[0].stripWhiteSpace();
			QString params	= substyle[1].stripWhiteSpace();
			if( command == "color" )
				parseColor( c, params );
		}
	}
	m_gc.current()->color = c;

	if( b.tagName() == "linearGradient" )
	{
		if( gradhelper.bbox )
		{			
			gradhelper.gradient.setOrigin( KoPoint( toPercentage( b.attribute( "x1", "0%" ) ), toPercentage( b.attribute( "y1", "0%" ) ) ) );
			gradhelper.gradient.setVector( KoPoint( toPercentage( b.attribute( "x2", "100%" ) ), toPercentage( b.attribute( "y2", "0%" ) ) ) );
		}
		else
		{
			gradhelper.gradient.setOrigin( KoPoint( b.attribute( "x1" ).toDouble(), b.attribute( "y1" ).toDouble() ) );
			gradhelper.gradient.setVector( KoPoint( b.attribute( "x2" ).toDouble(), b.attribute( "y2" ).toDouble() ) );
		}
		gradhelper.gradient.setType( VGradient::linear );
	}
	else
	{
		if( gradhelper.bbox )
		{
			gradhelper.gradient.setOrigin( KoPoint( toPercentage( b.attribute( "cx", "50%" ) ), toPercentage( b.attribute( "cy", "50%" ) ) ) );
			gradhelper.gradient.setVector( KoPoint( toPercentage( b.attribute( "cx", "50%" ) ) + toPercentage( b.attribute( "r", "50%" ) ), toPercentage( b.attribute( "cy", "50%" ) ) ) );
			gradhelper.gradient.setFocalPoint( KoPoint( toPercentage( b.attribute( "fx", "50%" ) ), toPercentage( b.attribute( "fy", "50%" ) ) ) );
		}
		else
		{
			gradhelper.gradient.setOrigin( KoPoint( b.attribute( "cx" ).toDouble(), b.attribute( "cy" ).toDouble() ) );
			gradhelper.gradient.setFocalPoint( KoPoint( b.attribute( "fx" ).toDouble(), b.attribute( "fy" ).toDouble() ) );
			gradhelper.gradient.setVector( KoPoint( b.attribute( "cx" ).toDouble() + b.attribute( "r" ).toDouble(), b.attribute( "cy" ).toDouble() ) );
		}
		gradhelper.gradient.setType( VGradient::radial );
	}
	// handle spread method
	QString spreadMethod = b.attribute( "spreadMethod" );
	if( !spreadMethod.isEmpty() )
	{
		if( spreadMethod == "reflect" )
			gradhelper.gradient.setRepeatMethod( VGradient::reflect );
		else if( spreadMethod == "repeat" )
			gradhelper.gradient.setRepeatMethod( VGradient::repeat );
		else
			gradhelper.gradient.setRepeatMethod( VGradient::none );
	}
	else
		gradhelper.gradient.setRepeatMethod( VGradient::none );

	// Parse the color stops. The referencing gradient does not have colorstops, 
	// so use the stops from the gradient it references to (e in this case and not b)
	parseColorStops( &gradhelper.gradient, e );
	//gradient.setGradientTransform( parseTransform( e.attribute( "gradientTransform" ) ) );
	gradhelper.gradientTransform = VPath::parseTransform( b.attribute( "gradientTransform" ) );
	m_gradients.insert( b.attribute( "id" ), gradhelper );
}

void SvgImport::parsePA( VObject *obj, SvgGraphicsContext *gc, const QString &command, const QString &params )
{
	VColor fillcolor = gc->fill.color();
	VColor strokecolor = gc->stroke.color();

	if( params == "inherit" ) return;

	if( command == "fill" )
	{
		if( params == "none" )
			gc->fill.setType( VFill::none );
		else if( params.startsWith( "url(" ) )
		{
			unsigned int start = params.find("#") + 1;
			unsigned int end = params.findRev(")");
			QString key = params.mid( start, end - start );
			GradientHelper *gradHelper = findGradient( key );
			if( gradHelper )
			{
				gc->fill.gradient() = gradHelper->gradient;
	
				if( gradHelper->bbox )
				{
					// adjust to bbox
					KoRect bbox = obj->boundingBox();
					//kdDebug() << "bbox x : " << bbox.x() << endl;
					//kdDebug() << "!!!!!!bbox y : " << bbox.y() << endl;
					//kdDebug() << gc->fill.gradient().origin().x() << endl;
					//kdDebug() << gc->fill.gradient().vector().x() << endl;
					double offsetx = parseUnit( QString( "%1%" ).arg( gc->fill.gradient().origin().x() ), true, false, bbox );
					double offsety = parseUnit( QString( "%1%" ).arg( gc->fill.gradient().origin().y() ), false, true, bbox );
					gc->fill.gradient().setOrigin( KoPoint( bbox.x() + offsetx, bbox.y() + offsety ) );
					if(gc->fill.gradient().type() == VGradient::radial)
					{
						offsetx = parseUnit( QString( "%1%" ).arg( gc->fill.gradient().focalPoint().x() ), true, false, bbox );
						offsety = parseUnit( QString( "%1%" ).arg( gc->fill.gradient().focalPoint().y() ), false, true, bbox );
						gc->fill.gradient().setFocalPoint( KoPoint( bbox.x() + offsetx, bbox.y() + offsety ) );
					}
					offsetx = parseUnit( QString( "%1%" ).arg( gc->fill.gradient().vector().x() ), true, false, bbox );
					offsety = parseUnit( QString( "%1%" ).arg( gc->fill.gradient().vector().y() ), false, true, bbox );
					gc->fill.gradient().setVector( KoPoint( bbox.x() + offsetx, bbox.y() + offsety ) );
					//kdDebug() << offsety << endl;
					//kdDebug() << gc->fill.gradient().origin().x() << endl;
					//kdDebug() << gc->fill.gradient().origin().y() << endl;
					//kdDebug() << gc->fill.gradient().vector().x() << endl;
					//kdDebug() << gc->fill.gradient().vector().y() << endl;
				}
				gc->fill.gradient().transform( gradHelper->gradientTransform );

				if( !gradHelper->bbox )
					gc->fill.gradient().transform( gc->matrix );

				gc->fill.setType( VFill::grad );
			}
			else
				gc->fill.setType( VFill::none );
		}
		else
		{
			parseColor( fillcolor,  params );
			gc->fill.setType( VFill::solid );
		}
	}
	else if( command == "fill-rule" )
	{
		if( params == "nonzero" )
			gc->fillRule = winding;
		else if( params == "evenodd" )
			gc->fillRule = evenOdd;
	}
	else if( command == "stroke" )
	{
		if( params == "none" )
			gc->stroke.setType( VStroke::none );
		else if( params.startsWith( "url(" ) )
		{
			unsigned int start = params.find("#") + 1;
			unsigned int end = params.findRev(")");
			QString key = params.mid( start, end - start );

			GradientHelper *gradHelper = findGradient( key );
			if( gradHelper )
			{
				gc->stroke.gradient() = gradHelper->gradient;
				gc->stroke.gradient().transform( gradHelper->gradientTransform );
				gc->stroke.gradient().transform( gc->matrix );
				gc->stroke.setType( VStroke::grad );
			}
			else 
				gc->stroke.setType( VStroke::none );
		}
		else
		{
			parseColor( strokecolor, params );
			gc->stroke.setType( VStroke::solid );
		}
	}
	else if( command == "stroke-width" )
		gc->stroke.setLineWidth( parseUnit( params, true, true, m_outerRect ) );
	else if( command == "stroke-linejoin" )
	{
		if( params == "miter" )
			gc->stroke.setLineJoin( VStroke::joinMiter );
		else if( params == "round" )
			gc->stroke.setLineJoin( VStroke::joinRound );
		else if( params == "bevel" )
			gc->stroke.setLineJoin( VStroke::joinBevel );
	}
	else if( command == "stroke-linecap" )
	{
		if( params == "butt" )
			gc->stroke.setLineCap( VStroke::capButt );
		else if( params == "round" )
			gc->stroke.setLineCap( VStroke::capRound );
		else if( params == "square" )
			gc->stroke.setLineCap( VStroke::capSquare );
	}
	else if( command == "stroke-miterlimit" )
		gc->stroke.setMiterLimit( params.toFloat() );
	else if( command == "stroke-dasharray" )
	{
		QValueList<float> array;
		if(params != "none")
		{
			QStringList dashes = QStringList::split( ' ', params );
		    for( QStringList::Iterator it = dashes.begin(); it != dashes.end(); ++it )
				array.append( (*it).toFloat() );
		}
		gc->stroke.dashPattern().setArray( array );
	}
	else if( command == "stroke-dashoffset" )
		gc->stroke.dashPattern().setOffset( params.toFloat() );
	// handle opacity
	else if( command == "stroke-opacity" )
		strokecolor.setOpacity( fromPercentage( params ) );
	else if( command == "fill-opacity" )
		fillcolor.setOpacity( fromPercentage( params ) );
	else if( command == "opacity" )
	{
		fillcolor.setOpacity( fromPercentage( params ) );
		strokecolor.setOpacity( fromPercentage( params ) );
	}
	else if( command == "font-family" )
	{
		QString family = params;
		family.replace( '\'' , ' ' );
		gc->font.setFamily( family );
	}
	else if( command == "font-size" )
	{
		float pointSize = parseUnit( params );
		gc->font.setPointSizeFloat( pointSize * getScalingFromMatrix( gc->matrix ) );
	}
	else if( command == "font-weight" )
	{
		int weight = QFont::Normal;

		// map svg weight to qt weight
		// svg value		qt value
		// 100,200,300		1, 17, 33
		// 400				50			(normal)
		// 500,600			58,66
		// 700				75			(bold)
		// 800,900			87,99

		if( params == "bold" )
			weight = QFont::Bold;
		else if( params == "lighter" )
		{
			weight = gc->font.weight();
			if( weight <= 17 ) 
				weight = 1;
			else if( weight <= 33 )
				weight = 17;
			else if( weight <= 50 )
				weight = 33;
			else if( weight <= 58 )
				weight = 50;
			else if( weight <= 66 )
				weight = 58;
			else if( weight <= 75 )
				weight = 66;
			else if( weight <= 87 )
				weight = 75;
			else if( weight <= 99 )
				weight = 87;
		}
		else if( params == "bolder" )
		{
			weight = gc->font.weight();
			if( weight >= 87 ) 
				weight = 99;
			else if( weight >= 75 )
				weight = 87;
			else if( weight >= 66 )
				weight = 75;
			else if( weight >= 58 )
				weight = 66;
			else if( weight >= 50 )
				weight = 58;
			else if( weight >= 33 )
				weight = 50;
			else if( weight >= 17 )
				weight = 50;
			else if( weight >= 1 )
				weight = 17;
		}
		else
		{
			bool ok;
			// try to read numerical weight value
			weight = params.toInt( &ok, 10 );

			if( !ok )
				return;

			switch( weight )
			{
				case 100: weight = 1; break;
				case 200: weight = 17; break;
				case 300: weight = 33; break;
				case 400: weight = 50; break;
				case 500: weight = 58; break;
				case 600: weight = 66; break;
				case 700: weight = 75; break;
				case 800: weight = 87; break;
				case 900: weight = 99; break;
			}
		}
		gc->font.setWeight( weight );
	}
	else if( command == "text-decoration" )
	{
		if( params == "line-through" )
			gc->font.setStrikeOut( true );
		else if( params == "underline" )
			gc->font.setUnderline( true );
	}
	else if( command == "color" )
	{
		VColor color;
		parseColor( color, params );
		gc->color = color;
	}
	if( gc->fill.type() != VFill::none )
		gc->fill.setColor( fillcolor, false );
	//if( gc->stroke.type() == VStroke::solid )
		gc->stroke.setColor( strokecolor );
}

void SvgImport::parseStyle( VObject *obj, const QDomElement &e )
{
	SvgGraphicsContext *gc = m_gc.current();
	if( !gc ) return;

	// try normal PA
	if( !e.attribute( "color" ).isEmpty() )
		parsePA( obj, gc, "color", e.attribute( "color" ) );
	if( !e.attribute( "fill" ).isEmpty() )
		parsePA( obj, gc, "fill", e.attribute( "fill" ) );
	if( !e.attribute( "fill-rule" ).isEmpty() )
		parsePA( obj, gc, "fill-rule", e.attribute( "fill-rule" ) );
	if( !e.attribute( "stroke" ).isEmpty() )
		parsePA( obj, gc, "stroke", e.attribute( "stroke" ) );
	if( !e.attribute( "stroke-width" ).isEmpty() )
		parsePA( obj, gc, "stroke-width", e.attribute( "stroke-width" ) );
	if( !e.attribute( "stroke-linejoin" ).isEmpty() )
		parsePA( obj, gc, "stroke-linejoin", e.attribute( "stroke-linejoin" ) );
	if( !e.attribute( "stroke-linecap" ).isEmpty() )
		parsePA( obj, gc, "stroke-linecap", e.attribute( "stroke-linecap" ) );
	if( !e.attribute( "stroke-dasharray" ).isEmpty() )
		parsePA( obj, gc, "stroke-dasharray", e.attribute( "stroke-dasharray" ) );
	if( !e.attribute( "stroke-dashoffset" ).isEmpty() )
		parsePA( obj, gc, "stroke-dashoffset", e.attribute( "stroke-dashoffset" ) );
	if( !e.attribute( "stroke-opacity" ).isEmpty() )
		parsePA( obj, gc, "stroke-opacity", e.attribute( "stroke-opacity" ) );
	if( !e.attribute( "stroke-miterlimit" ).isEmpty() )
		parsePA( obj, gc, "stroke-miterlimit", e.attribute( "stroke-miterlimit" ) );
	if( !e.attribute( "fill-opacity" ).isEmpty() )
		parsePA( obj, gc, "fill-opacity", e.attribute( "fill-opacity" ) );
	if( !e.attribute( "opacity" ).isEmpty() )
		parsePA( obj, gc, "opacity", e.attribute( "opacity" ) );

	// try style attr
	QString style = e.attribute( "style" ).simplifyWhiteSpace();
	QStringList substyles = QStringList::split( ';', style );
	for( QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it )
	{
		QStringList substyle = QStringList::split( ':', (*it) );
		QString command	= substyle[0].stripWhiteSpace();
		QString params	= substyle[1].stripWhiteSpace();
		parsePA( obj, gc, command, params );
	}

	if(!obj)
		return;

	obj->setFill( gc->fill );
	if( dynamic_cast<VPath *>( obj ) )
		dynamic_cast<VPath *>( obj )->setFillRule( gc->fillRule );
	// stroke scaling
	double lineWidth = gc->stroke.lineWidth();
	gc->stroke.setLineWidth( lineWidth * getScalingFromMatrix( gc->matrix ) );
	obj->setStroke( gc->stroke );
	gc->stroke.setLineWidth( lineWidth );
}

void SvgImport::parseFont( const QDomElement &e )
{
	SvgGraphicsContext *gc = m_gc.current();
	if( !gc ) return;

	if( ! e.attribute( "font-family" ).isEmpty() )	
		parsePA( 0L, m_gc.current(), "font-family", e.attribute( "font-family" ) );
	if( ! e.attribute( "font-size" ).isEmpty() )	
		parsePA( 0L, m_gc.current(), "font-size", e.attribute( "font-size" ) );
	if( ! e.attribute( "font-weight" ).isEmpty() )	
		parsePA( 0L, m_gc.current(), "font-weight", e.attribute( "font-weight" ) );
	if( ! e.attribute( "text-decoration" ).isEmpty() )
		parsePA( 0L, m_gc.current(), "text-decoration", e.attribute( "text-decoration" ) );
}

void SvgImport::parseUse( VGroup *grp, const QDomElement &e )
{
	QString id = e.attribute( "xlink:href" );

	if( !id.isEmpty() )
	{
		addGraphicContext();
		setupTransform( e );

		QString key = id.mid( 1 );

		if( !e.attribute( "x" ).isEmpty() && !e.attribute( "y" ).isEmpty() )
		{
			double tx = e.attribute( "x" ).toDouble();
			double ty = e.attribute( "y" ).toDouble();

			m_gc.current()->matrix.translate(tx,ty);
		}

		if(m_defs.contains(key))
		{
			QDomElement a = m_defs[key];
			if(a.tagName() == "g" || a.tagName() == "a")
				parseGroup( grp, a);
			else
			{
				// Create the object with the merged styles.
				// The object inherits all style attributes from the use tag, but keeps it's own attributes.
				// So, not just use the style attributes of the use tag, but merge them first.
				createObject( grp, a, VObject::normal, mergeStyles(e, a) );
			}
		}
		delete( m_gc.pop() );
	}
}

void SvgImport::parseGroup( VGroup *grp, const QDomElement &e )
{
	for( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() )
	{
		QDomElement b = n.toElement();
		if( b.isNull() ) continue;
		
		// treat svg link <a> as group so we don't miss its child elements
		if( b.tagName() == "g" || b.tagName() == "a" )
		{
			VGroup *group;
			if ( grp )
				group = new VGroup( grp );
			else
				group = new VGroup( &m_document );

			addGraphicContext();
			setupTransform( b );
			parseStyle( group, b );
			parseFont( b );
			parseGroup( group, b );

			// handle id
			if( !b.attribute("id").isEmpty() )
				group->setName( b.attribute("id") );
			if( grp )
				grp->append( group );
			else
				m_document.append( group );
			delete( m_gc.pop() );
			continue;
		}
		if( b.tagName() == "defs" )
		{
			parseDefs( b );
			continue;
		}
		else if( b.tagName() == "linearGradient" || b.tagName() == "radialGradient" )
		{
			parseGradient( b );
			continue;
		}
		if( b.tagName() == "rect" ||
			b.tagName() == "ellipse" ||
			b.tagName() == "circle" ||
			b.tagName() == "line" ||
			b.tagName() == "polyline" ||
			b.tagName() == "polygon" ||
			b.tagName() == "path" ||
			b.tagName() == "image" )
		{
			createObject( grp, b );
			continue;
		}
		else if( b.tagName() == "text" )
		{
			createText( grp, b );
			continue;
		}
		else if( b.tagName() == "use" )
		{
			parseUse( grp, b );
			continue;
		}
	}
}

void SvgImport::parseDefs( const QDomElement &e )
{
	for( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() )
	{
		QDomElement b = n.toElement();
		if( b.isNull() ) continue;

		QString definition = b.attribute( "id" );
		if( !definition.isEmpty() )
		{
			if( !m_defs.contains( definition ) )
				m_defs.insert( definition, b );
		}
	}
}


// Creating functions
// ---------------------------------------------------------------------------------------

void SvgImport::createText( VGroup *grp, const QDomElement &b )
{
	const double pathLength = 10.0;

	VText *text = 0L;
	QString content;
	QString anchor;
	VSubpath base( 0L );
	VPath *path = 0L;
	double offset = 0.0;

	addGraphicContext();
	setupTransform( b );
	
	parseFont( b );

	if( ! b.attribute( "text-anchor" ).isEmpty() )
		anchor = b.attribute( "text-anchor" );

	if( b.hasChildNodes() )
	{
		if( base.isEmpty() && ! b.attribute( "x" ).isEmpty() && ! b.attribute( "y" ).isEmpty() )
		{
			double x = parseUnit( b.attribute( "x" ) );
			double y = parseUnit( b.attribute( "y" ) );
			base.moveTo( KoPoint( x, y ) );
			base.lineTo( KoPoint( x + pathLength, y ) );
		}

		for( QDomNode n = b.firstChild(); !n.isNull(); n = n.nextSibling() )
		{
			QDomElement e = n.toElement();
			if( e.isNull() ) 
			{
				content += n.toCharacterData().data();
			}
			else if( e.tagName() == "textPath" )
			{
				if( e.attribute( "xlink:href" ).isEmpty() )
					continue;

				QString key = e.attribute( "xlink:href" ).mid( 1 );
				if( ! m_defs.contains(key) )
				{
					// try to find referenced object in document
					VObject* obj = findObject( key );
					// try to find referenced object in actual group, which is not yet part of document
					if( ! obj )
						obj = findObject( key, grp );
					if( obj ) 
						path = dynamic_cast<VPath*>( obj );
				}
				else
				{
					QDomElement p = m_defs[key];
					createObject( grp, p, VObject::deleted);
				}
				if( ! path )
					continue;
				base = *path->paths().getFirst();
				content += e.text();
				
				if( ! e.attribute( "startOffset" ).isEmpty() )
				{
					QString start = e.attribute( "startOffset" );
					if( start.endsWith( "%" ) )
						offset = 0.01 * start.remove( '%' ).toDouble();
					else
					{
						float pathLength = 0;
						VSubpathIterator pIt( base );
							
						for( ; pIt.current(); ++pIt )
							pathLength += pIt.current()->length();
						
						if( pathLength > 0.0 )
							offset = start.toDouble() / pathLength;
					}
				}
			}
			else if( e.tagName() == "tspan" )
			{
				// only use text of tspan element, as we are not supporting text 
				// with different styles
				content += e.text();
				if( base.isEmpty() && ! e.attribute( "x" ).isEmpty() && ! e.attribute( "y" ).isEmpty() )
				{
					QStringList posX = QStringList::split( ", ", e.attribute( "x" ) );
					QStringList posY = QStringList::split( ", ", e.attribute( "y" ) );
					if( posX.count() && posY.count() )
					{
						double x = parseUnit( posX.first() );
						double y = parseUnit( posY.first() );
						base.moveTo( KoPoint( x, y ) );
						base.lineTo( KoPoint( x + pathLength, y ) );
					}
				}
			}
			else if( e.tagName() == "tref" )
			{
				if( e.attribute( "xlink:href" ).isEmpty() )
					continue;

				QString key = e.attribute( "xlink:href" ).mid( 1 );
				if( ! m_defs.contains(key) )
				{
					// try to find referenced object in document
					VObject* obj = findObject( key );
					// try to find referenced object in actual group, which is not yet part of document
					if( ! obj )
						obj = findObject( key, grp );
					if( obj ) 
						content += dynamic_cast<VText*>( obj )->text();
				}
				else
				{
					QDomElement p = m_defs[key];
					content += p.text();
				}
			}
			else 
				continue;

			if( ! e.attribute( "text-anchor" ).isEmpty() )
				anchor = e.attribute( "text-anchor" );
		}
		text = new VText( m_gc.current()->font, base, VText::Above, VText::Left, content.simplifyWhiteSpace() );
	}
	else
	{
		VSubpath base( 0L );
		double x = parseUnit( b.attribute( "x" ) );
		double y = parseUnit( b.attribute( "y" ) );
		base.moveTo( KoPoint( x, y ) );
		base.lineTo( KoPoint( x + pathLength, y ) );
		text = new VText( m_gc.current()->font, base, VText::Above, VText::Left, b.text().simplifyWhiteSpace() );
	}

	if( text )
	{
		text->setParent( &m_document );
		
		parseStyle( text, b );

		text->setFont( m_gc.current()->font );

		VTransformCmd trafo( 0L, m_gc.current()->matrix );
		trafo.visit( *text );

		if( !b.attribute("id").isEmpty() )
			text->setName( b.attribute("id") );

		if( anchor == "middle" )
			text->setAlignment( VText::Center );
		else if( anchor == "end" )
			text->setAlignment( VText::Right );
		
		if( offset > 0.0 )
			text->setOffset( offset );

		if( grp ) 
			grp->append( text );
		else 
			m_document.append( text );
	}

	delete( m_gc.pop() );
}

void SvgImport::createObject( VGroup *grp, const QDomElement &b, const VObject::VState state, const QDomElement &style )
{
	VObject *obj = 0L;

	addGraphicContext();
	setupTransform( b );

	if( b.tagName() == "rect" )
	{
		double x		= parseUnit( b.attribute( "x" ), true, false, m_outerRect );
		double y		= parseUnit( b.attribute( "y" ), false, true, m_outerRect );
		double width	= parseUnit( b.attribute( "width" ), true, false, m_outerRect );
		double height	= parseUnit( b.attribute( "height" ), false, true, m_outerRect );
		double rx       = parseUnit( b.attribute( "rx" ) );
		double ry       = parseUnit( b.attribute( "ry" ) );
		obj = new VRectangle( 0L, KoPoint( x, height + y ) , width, height, rx, ry );
	}
	else if( b.tagName() == "ellipse" )
	{
		double rx		= parseUnit( b.attribute( "rx" ) );
		double ry		= parseUnit( b.attribute( "ry" ) );
		double left		= parseUnit( b.attribute( "cx" ) ) - rx;
		double top		= parseUnit( b.attribute( "cy" ) ) - ry;
		obj = new VEllipse( 0L, KoPoint( left, top ), rx * 2.0, ry * 2.0 );
	}
	else if( b.tagName() == "circle" )
	{
		double r		= parseUnit( b.attribute( "r" ) );
		double left		= parseUnit( b.attribute( "cx" ) ) - r;
		double top		= parseUnit( b.attribute( "cy" ) ) - r;
		obj = new VEllipse( 0L, KoPoint( left, top ), r * 2.0, r * 2.0 );
	}
	else if( b.tagName() == "line" )
	{
		VPath *path = new VPath( &m_document );
		double x1 = b.attribute( "x1" ).isEmpty() ? 0.0 : parseUnit( b.attribute( "x1" ) );
		double y1 = b.attribute( "y1" ).isEmpty() ? 0.0 : parseUnit( b.attribute( "y1" ) );
		double x2 = b.attribute( "x2" ).isEmpty() ? 0.0 : parseUnit( b.attribute( "x2" ) );
		double y2 = b.attribute( "y2" ).isEmpty() ? 0.0 : parseUnit( b.attribute( "y2" ) );
		path->moveTo( KoPoint( x1, y1 ) );
		path->lineTo( KoPoint( x2, y2 ) );
		obj = path;
	}
	else if( b.tagName() == "polyline" || b.tagName() == "polygon" )
	{
		VPath *path = new VPath( &m_document );
		bool bFirst = true;

		QString points = b.attribute( "points" ).simplifyWhiteSpace();
		points.replace( ',', ' ' );
		points.remove( '\r' );
		points.remove( '\n' );
		QStringList pointList = QStringList::split( ' ', points );
		for( QStringList::Iterator it = pointList.begin(); it != pointList.end(); ++it)
		{
			KoPoint point;
			point.setX( (*it).toDouble() );
			++it;
			point.setY( (*it).toDouble() );
			if( bFirst )
			{
				path->moveTo( point );
				bFirst = false;
			}
			else
				path->lineTo( point );
		}
		if( b.tagName() == "polygon" ) path->close();
		obj = path;
	}
	else if( b.tagName() == "path" )
	{
		VPath *path = new VPath( &m_document );
		path->loadSvgPath( b.attribute( "d" ) );
		obj = path;
	}
	else if( b.tagName() == "image" )
	{
		QString fname = b.attribute("xlink:href");
		obj = new VImage( 0L, fname );
	}

	if( !obj ) 
		return;

	if (state != VObject::normal)
		obj->setState(state);

	VTransformCmd trafo( 0L, m_gc.current()->matrix );
	trafo.visit( *obj );
	
	if( !style.isNull() )
		parseStyle( obj, style );
	else
		parseStyle( obj, b );

	// handle id
	if( !b.attribute("id").isEmpty() )
		obj->setName( b.attribute("id") );
	if( grp )
		grp->append( obj );
	else
		m_document.append( obj );

	delete( m_gc.pop() );
}

#include <svgimport.moc>
