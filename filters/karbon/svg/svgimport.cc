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
#include <qregexp.h>
#include <iostream>

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

void
SvgImport::convert()
{
	QDomElement docElem = inpdoc.documentElement();
	// undo y-mirroring
	GraphicsContext *gc = new GraphicsContext;
	gc->matrix.scale( 1, -1 );
	gc->matrix.translate( 0, -841 );
	m_gc.push( gc );
	parseGroup( 0L, docElem );

	outdoc = m_document.saveXML();
}

VColor
SvgImport::parseColor( const QString &s )
{
	VColor color;
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
	else
	{
		QColor c;
		c.setNamedColor( s );
		color.set( c.red() / 255.0, c.green() / 255.0, c.blue() / 255.0 );
	}
	return color;
}

void
SvgImport::parseStyle( VObject *obj, const QDomElement &e )
{
	GraphicsContext *gc = new GraphicsContext;
	// set as default
	if( m_gc.current() )
		*gc = *( m_gc.current() );

	VColor strokecolor	= gc->stroke.color();
	VColor fillcolor	= gc->fill.color();

	// try normal PA
	if( !e.attribute( "fill" ).isEmpty() )
	{
		if( e.attribute( "fill" ) == "none" )
			gc->fill.setType( VFill::none );
		else
		{
			fillcolor = parseColor( e.attribute( "fill" ) );
			gc->fill.setType( VFill::solid );
		}
	}
	if( !e.attribute( "fill-rule" ).isEmpty() )
		if( e.attribute( "fill-rule" ) == "nonzero" )
			gc->fill.setFillRule( VFill::winding );
		else if( e.attribute( "fill-rule" ) == "evenodd" )
			gc->fill.setFillRule( VFill::evenOdd );

	if( !e.attribute( "stroke" ).isEmpty() )
	{
		if( e.attribute( "stroke" ) == "none" )
			gc->stroke.setType( VStroke::none );
		else
		{
			strokecolor = parseColor( e.attribute( "stroke" ) );
			gc->stroke.setType( VStroke::solid );
		}
	}
	if( !e.attribute( "stroke-width" ).isEmpty() )
		gc->stroke.setLineWidth( e.attribute( "stroke-width" ).toDouble() );
	if( !e.attribute( "stroke-linestyle" ).isEmpty() )
	{
		if( e.attribute( "stroke-linestyle" ) == "miter" )
			gc->stroke.setLineJoin( VStroke::joinMiter );
		else if( e.attribute( "stroke-linestyle" ) == "round" )
			gc->stroke.setLineJoin( VStroke::joinRound );
		else if( e.attribute( "stroke-linestyle" ) == "bevel" )
			gc->stroke.setLineJoin( VStroke::joinBevel );
	}
	if( !e.attribute( "stroke-linecap" ).isEmpty() )
	{
		if( e.attribute( "stroke-linecap" ) == "butt" )
			gc->stroke.setLineCap( VStroke::capButt );
		else if( e.attribute( "stroke-linecap" ) == "round" )
			gc->stroke.setLineCap( VStroke::capRound );
		else if( e.attribute( "stroke-linecap" ) == "square" )
			gc->stroke.setLineCap( VStroke::capSquare );
	}
	// handle opacity
	if( !e.attribute( "stroke-opacity" ).isEmpty() )
		strokecolor.setOpacity( e.attribute( "stroke-opacity" ).toFloat() );
	else if( !e.attribute( "fill-opacity" ).isEmpty() )
		fillcolor.setOpacity( e.attribute( "fill-opacity" ).toFloat() );
	if( !e.attribute( "opacity" ).isEmpty() )
	{
		fillcolor.setOpacity( e.attribute( "opacity" ).toFloat() );
		strokecolor.setOpacity( e.attribute( "opacity" ).toFloat() );
	}

	// try style attr
	QString style = e.attribute( "style" ).simplifyWhiteSpace();
	QStringList substyles = QStringList::split( ';', style );
    for( QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it )
	{
		QStringList substyle = QStringList::split( ':', (*it) );
		QString command	= substyle[0].stripWhiteSpace();
		QString params	= substyle[1].stripWhiteSpace();
		if( command == "fill" )
		{
			if( params == "none" )
				gc->fill.setType( VFill::none );
			else
			{
				fillcolor = parseColor( params );
				gc->fill.setType( VFill::solid );
			}
		}
		else if( command == "fill-rule" )
		{
			if( params == "fill-rule" )
				gc->fill.setFillRule( VFill::winding );
			else if( params == "fill-rule" )
				gc->fill.setFillRule( VFill::evenOdd );
		}
		else if( command == "stroke" )
		{
			if( params == "none" )
				gc->stroke.setType( VStroke::none );
			else
			{
				strokecolor = parseColor( params );
				gc->stroke.setType( VStroke::solid );
			}
		}
		else if( command == "stroke-width" )
			gc->stroke.setLineWidth( params.toDouble() );
		else if( command == "stroke-linestyle" )
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
		// handle opacity
		else if( command == "stroke-opacity" )
			strokecolor.setOpacity( params.toFloat() );
		else if( command == "fill-opacity" )
			fillcolor.setOpacity( params.toFloat() );
		else if( command == "opacity" )
		{
			fillcolor.setOpacity( params.toFloat() );
			strokecolor.setOpacity( params.toFloat() );
		}
	}

	if( gc->fill.type() == VFill::solid )
		gc->fill.setColor( fillcolor );
	if( gc->stroke.type() == VStroke::solid )
		gc->stroke.setColor( strokecolor );
	obj->setFill( gc->fill );
	obj->setStroke( gc->stroke );
	obj->transform( gc->matrix );
	m_gc.push( gc );
}

void
SvgImport::parseGroup( VGroup *grp, const QDomElement &e )
{
	QDomElement b = e.firstChild().toElement();
	for( ; !b.isNull(); b = b.nextSibling().toElement() )
	{
		VObject *obj = 0L;
		if( b.tagName() == "g" )
		{
			VGroup *group;
			if ( grp )
				group = new VGroup( grp );
			else
				group = new VGroup( &m_document );

			parseStyle( group, b );
			parseGroup( group, b );
			if( grp )
				grp->append( group );
			else
				m_document.append( group );
			m_gc.pop();
			continue;
		}
		else if( b.tagName() == "rect" )
		{
			int x = b.attribute( "x" ).toInt();
			int y = b.attribute( "y" ).toInt();
			int width = b.attribute( "width" ).toInt();
			int height = b.attribute( "height" ).toInt();
			obj = new VRectangle( 0L, KoPoint( x, height + y ) , width, height );
		}
		else if( b.tagName() == "ellipse" )
		{
			double rx = b.attribute( "rx" ).toDouble();
			double ry = b.attribute( "ry" ).toDouble();
			double left	= b.attribute( "cx" ).toDouble() - ( rx / 2.0 );
			double top	= b.attribute( "cy" ).toDouble() + ( ry / 2.0 );
			// Append the ellipse to the document
			obj = new VEllipse( 0L, KoPoint( left, top ), rx * 2.0, ry * 2.0 );
		}
		else if( b.tagName() == "circle" )
		{
			double r = b.attribute( "r" ).toDouble();
			double left	= b.attribute( "cx" ).toDouble() - ( r / 2.0 );
			double top	= b.attribute( "cy" ).toDouble() + ( r / 2.0 );
			// Append the ellipse to the document
			obj = new VEllipse( 0L, KoPoint( left, top ), r * 2.0, r * 2.0 );
		}
		else if( b.tagName() == "line" )
		{
			VComposite *path = new VComposite( &m_document );
			double x1 = b.attribute( "x1" ).isEmpty() ? 0.0 : b.attribute( "x1" ).toDouble();
			double y1 = b.attribute( "y1" ).isEmpty() ? 0.0 : b.attribute( "y1" ).toDouble();
			double x2 = b.attribute( "x2" ).isEmpty() ? 0.0 : b.attribute( "x2" ).toDouble();
			double y2 = b.attribute( "y2" ).isEmpty() ? 0.0 : b.attribute( "y2" ).toDouble();
			path->moveTo( KoPoint( x1, y1 ) );
			path->lineTo( KoPoint( x2, y2 ) );
			obj = path;
		}
		else if( b.tagName() == "polyline" || b.tagName() == "polygon" )
		{
			VComposite *path = new VComposite( &m_document );
			bool bFirst = true;

			QString points = b.attribute( "points" ).simplifyWhiteSpace();
			points.replace( QRegExp( "," ), " " );
			points.replace( QRegExp( "\r" ), "" );
		    points.replace( QRegExp( "\n" ), "" );
			QStringList pointList = QStringList::split( ' ', points );
			for( QStringList::Iterator it = pointList.begin(); it != pointList.end(); it++ )
			{
				if( bFirst )
				{
					path->moveTo( KoPoint( (*(it++)).toDouble(), (*it).toDouble() ) );
					bFirst = false;
				}
				else
					path->lineTo( KoPoint( (*(it++)).toDouble(), (*it).toDouble() ) );
			}
			if( b.tagName() == "polygon" ) path->close();
			obj = path;
		}
		else if( b.tagName() == "path" )
		{
			VComposite *path = new VComposite( &m_document );
			parsePath( path, b );
			obj = path;
		}
		if( !obj ) continue;
		parseStyle( obj, b );
		if( grp )
			grp->append( obj );
		else
			m_document.append( obj );
		m_gc.pop();
	}
}

// parses the coord into number and forwards to the next token
const char *
SvgImport::getCoord( const char *ptr, double &number )
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
	number *= sign * pow(10, expsign * exponent);

	// skip the following space
	if(*ptr == ' ')
		ptr++;
	
	return ptr;
}

void
SvgImport::parsePath( VComposite *obj, const QDomElement &e )
{
	QString d = e.attribute( "d" );

	if( !d.isEmpty() )
	{
		d = d.simplifyWhiteSpace();

		const char *ptr = d.latin1();
		const char *end = d.latin1() + d.length() + 1;

		double contrlx, contrly, curx, cury, tox, toy, x1, y1, x2, y2;
		bool relative;
		VPath *path = 0L;
		char command = *(ptr++), lastCommand = ' ';

		curx = cury = contrlx = contrly = 0.0;
		while( ptr < end )
		{
			if( *ptr == ' ' )
				ptr++;

			relative = false;

			cout << "Command : " << command << endl;
			switch( command )
			{
				case 'm':
					relative = true;
				case 'M':
				{
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;

					if( path )
					{
						obj->combinePath( *path );
						delete path;
					}
					path = new VPath( 0L );
					//if( lastCommand == 'z' || lastCommand == 'Z' )
					//	path->close();
					path->moveTo( KoPoint( curx, cury ) );
					break;
				}
				case 'l':
					relative = true;
				case 'L':
				{
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;

					path->lineTo( KoPoint( curx, cury ) );
					break;
				}
				case 'h':
				{
					ptr = getCoord( ptr, tox );
					curx += tox;
					path->lineTo( KoPoint( curx, cury ) );
					break;
				}
				case 'H':
				{
					ptr = getCoord( ptr, tox );
					curx = tox;
					path->lineTo( KoPoint( curx, cury ) );
					break;
				}
				case 'v':
				{
					ptr = getCoord( ptr, toy );
					cury += toy;
					path->lineTo( KoPoint( curx, cury ) );
					break;
				}
				case 'V':
				{
					ptr = getCoord( ptr, toy );
					cury = toy;
					path->lineTo( KoPoint( curx, cury ) );
					break;
				}
				case 'z':
				case 'Z':
				{
					path->close();
					obj->combinePath( *path );
					delete path;
					path = 0;
					break;
				}
				case 'c':
					relative = true;
				case 'C':
				{
					ptr = getCoord( ptr, x1 );
					ptr = getCoord( ptr, y1 );
					ptr = getCoord( ptr, x2 );
					ptr = getCoord( ptr, y2 );
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					if(relative)
						path->curveTo( KoPoint( curx + x1, cury + y1 ), KoPoint( curx + x2, cury + y2 ),
									   KoPoint( curx + tox, cury + toy ) );
					else
						path->curveTo( KoPoint( x1, y1 ), KoPoint( x2, y2 ), KoPoint( tox, toy ) );
					contrlx = relative ? curx + x2 : x2;
					contrly = relative ? cury + y2 : y2;
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;

					break;
				}
				case 's':
					relative = true;
				case 'S':
				{
					ptr = getCoord( ptr, x2 );
					ptr = getCoord( ptr, y2 );
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					if(relative)
						path->curveTo( KoPoint( 2 * cury - contrly, 2 * cury - contrly ),
									   KoPoint( curx + x2, cury + y2 ),
									   KoPoint( curx + tox, cury + toy ) );
					else
						path->curveTo( KoPoint( 2 * cury - contrly, 2 * cury - contrly ),
									   KoPoint( x2, y2 ), KoPoint( tox, toy ) );
					contrlx = relative ? curx + x2 : x2;
					contrly = relative ? cury + y2 : y2;
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;
					break;
				}
				case 'q':
					relative = true;
				case 'Q':
				{
					ptr = getCoord( ptr, x1 );
					ptr = getCoord( ptr, y1 );
					ptr = getCoord( ptr, tox );
					ptr = getCoord( ptr, toy );

					if(relative)
						path->curve2To( KoPoint( curx + x1, cury + y1 ),
									   KoPoint( curx + tox, cury + toy ) );
					else
						path->curve2To( KoPoint( x1, y1 ), KoPoint( tox, toy ) );
					curx = relative ? curx + tox : tox;
					cury = relative ? cury + toy : toy;
					break;
				}
				/*case 't':
					relative = true;
				case 'T':
				{
					ptr = getCoord(ptr, tox);
					ptr = getCoord(ptr, toy);

					if(relative)
						pathSegList()->appendItem(createSVGPathSegCurvetoQuadraticSmoothRel(tox, toy));
					else
						pathSegList()->appendItem(createSVGPathSegCurvetoQuadraticSmoothAbs(tox, toy));
					break;
				}
				case 'a':
					relative = true;
				case 'A':
				{
					bool largeArc, sweep;
					double rx, ry, angle;
					ptr = getCoord(ptr, rx);
					ptr = getCoord(ptr, ry);
					ptr = getCoord(ptr, angle);
					ptr = getCoord(ptr, tox);

					largeArc = tox == 1;

					ptr = getCoord(ptr, tox);

					sweep = tox == 1;

					ptr = getCoord(ptr, tox);
					ptr = getCoord(ptr, toy);

					if(relative)
						pathSegList()->appendItem(createSVGPathSegArcRel(tox, toy, rx, ry, angle, largeArc, sweep));
					else
						pathSegList()->appendItem(createSVGPathSegArcAbs(tox, toy, rx, ry, angle, largeArc, sweep));
					break;
				}*/
			}

			lastCommand = command;

			if(*ptr == '+' || *ptr == '-' || (*ptr >= '0' && *ptr <= '9'))
			{
				// there are still coords in this command
				if(command == 'M')
					command = 'L';
				else if(command == 'm')
					command = 'l';
			}
			else
				command = *(ptr++);

			if( lastCommand != 'C' && lastCommand != 'c' &&
				lastCommand != 'S' && lastCommand != 's' &&
				lastCommand != 'Q' && lastCommand != 'q' &&
				lastCommand != 'T' && lastCommand != 't')
			{
				contrlx = curx;
				contrly = cury;
			}
		}
		if( path )
		{
			obj->combinePath( *path );
			delete path;
		}
	}
}

#include <svgimport.moc>
