/* This file is part of the KDE project
   Copyright(C) 2001 Rob Buis <rwlbuis@wanadoo.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or(at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include "svgexport.h"

#include <GCurve.h>
#include <GText.h>
#include <float.h>

#include <qdom.h>
#include <qstring.h>
#include <qfile.h>
#include <qfontmetrics.h>
#include <qtextstream.h>

#include <kdebug.h>
#include <koStore.h>


SVGExport::SVGExport( KoFilter *parent, const char *name ) :
                      KoFilter( parent, name ) {
}

SVGExport::~SVGExport( ) {
}

bool SVGExport::filter( const QString &fileIn, const QString &fileOut,
                        const QString &from,   const QString &to, const QString & ) {
    if( to != "image/x-svg" || from != "application/x-kontour" )
    {
        kdWarning( 30514 ) << "Invalid mimetypes " << to << " " << from << endl;
        return false;
    }
    
    KoStore koStoreIn( fileIn, KoStore::Read );
    if( !koStoreIn.open( "root" ) )
    {
        koStoreIn.close();
        kdError( 30514 ) << "Unable to open input file!" << endl;
        return false;
    }

    QByteArray byteArrayIn = koStoreIn.read( koStoreIn.size() );
    koStoreIn.close();

    // read the temp file from the store into an xml tree
    QDomDocument qDomDocumentIn;
    qDomDocumentIn.setContent( byteArrayIn );
    QDomElement docNode = qDomDocumentIn.documentElement();

    QDomDocument qDomDocumentOut( "svg" );
    QDomElement svg = qDomDocumentOut.createElement( "svg" );
    qDomDocumentOut.appendChild( svg );

    // parse document from xml input tree
    exportDocument( svg, docNode );

    // write the svg xml tree to the destination file
    QFile out( fileOut );
    if( !out.open( IO_WriteOnly ) ) {
        kdError( 30514 ) << "Could not open the output file: " << fileOut << endl;
        return false;
    }
    QTextStream s( &out );
    qDomDocumentOut.save( s, 4 );
    out.close();

    return true;
}

void SVGExport::exportDocument( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QDomNodeList list = docNodeIn.childNodes();
    for( unsigned int i = 0 ; i < list.count(); i++ ) {
        if( list.item( i ).isElement() ) {
            QDomElement e = list.item( i ).toElement();
	    // only parse page node, the head node doesn't contain useful info for svg
	    if( e.tagName() == "page" )
	        exportPage( docNodeOut, e );
        }
    }
}

void SVGExport::exportPage( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QDomNodeList list = docNodeIn.childNodes();
    for( unsigned int i = 0 ; i < list.count(); i++ ) {
        if( list.item( i ).isElement() ) {
            QDomElement e = list.item( i ).toElement();
 	    if( e.tagName() == "layout" )
	        exportLayout( docNodeOut, e );
	    else if( e.tagName() == "layer" )
	        exportLayer( docNodeOut, e );
        }
    }
}

void SVGExport::exportLayout( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QString width  = docNodeIn.attribute( "width" ) + "mm";
    QString height = docNodeIn.attribute( "height" ) + "mm";
    docNodeOut.setAttribute( "width",  width );
    docNodeOut.setAttribute( "height", height );
}

void SVGExport::exportLayer( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    //QDomElement g = docNodeOut.ownerDocument().createElement( "g" );
    QDomNodeList list = docNodeIn.childNodes();
    for( unsigned int i = 0 ; i < list.count(); i++ ) {
        if( list.item( i ).isElement() ) {
            QDomElement e = list.item( i ).toElement();
            if( e.tagName() == "rectangle" )
	        exportRect( docNodeOut, e );
            else if( e.tagName() == "polygon" )
	        exportPolygon( docNodeOut, e );
            else if( e.tagName() == "polyline" )
	        exportPolyline( docNodeOut, e );
	    else if( e.tagName() == "ellipse" )
	        exportEllipse( docNodeOut, e );
            else if( e.tagName() == "curve" )
	        exportCurve( docNodeOut, e );
            else if( e.tagName() == "bezier" )
	        exportBezier( docNodeOut, e );
	    else if( e.tagName() == "group" )
	        exportGroup( docNodeOut, e );
	    else if( e.tagName() == "pixmap" )
	        exportPixmap( docNodeOut, e );
	    else if( e.tagName() == "text" )
	        exportText( docNodeOut, e );
        }
    }
    //docNodeOut.appendChild( g );
}

void SVGExport::exportGroup( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QDomElement g = docNodeOut.ownerDocument().createElement( "g" );
    QDomNodeList list = docNodeIn.childNodes();
    exportGObject( g, list.item( 0 ).toElement() );
    for( unsigned int i = 1 ; i < list.count(); i++ ) {
        if( list.item( i ).isElement() ) {
            QDomElement e = list.item( i ).toElement();
	    if( e.tagName() == "rectangle" )
	        exportRect( g, e );
            else if( e.tagName() == "polygon" )
	        exportPolygon( g, e );
            else if( e.tagName() == "polyline" )
	        exportPolyline( g, e );
	    else if( e.tagName() == "ellipse" )
	        exportEllipse( g, e );
            else if( e.tagName() == "curve" )
	        exportCurve( g, e );
            else if( e.tagName() == "bezier" )
	        exportBezier( g, e );
	    else if( e.tagName() == "group" )
	        exportGroup( g, e );
	    else if( e.tagName() == "pixmap" )
	        exportPixmap( g, e );
	    else if( e.tagName() == "text" )
	        exportText( g, e );
        }
    }
    docNodeOut.appendChild( g );
}

void SVGExport::exportRect( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QDomElement rect = docNodeOut.ownerDocument().createElement( "rect" );
    rect.setAttribute( "width",  docNodeIn.attribute( "width" ) );
    rect.setAttribute( "height", docNodeIn.attribute( "height" ) );
    rect.setAttribute( "x", docNodeIn.attribute( "x" ) );
    rect.setAttribute( "y", docNodeIn.attribute( "y" ) );
    int roundness = docNodeIn.attribute( "rounding" ).toInt();
    rect.setAttribute( "rx", roundness );
    rect.setAttribute( "ry", roundness );

    QList<Coord> coords;
    coords.setAutoDelete( true );
    coords = getPoints( docNodeIn.namedItem( "polyline" ).toElement() );
    exportGObject( rect, docNodeIn.namedItem( "polyline" ).namedItem( "gobject" ).toElement() );
    docNodeOut.appendChild( rect );
}

void SVGExport::exportPolygon( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QDomElement poly = docNodeOut.ownerDocument().createElement( "polygon" );

    QList<Coord> coords;
    coords.setAutoDelete( true );
    coords = getPoints( docNodeIn.namedItem( "polyline" ).toElement() );
    QString s;
    for( unsigned int i = 0; i < coords.count(); i++ ) {
        s += QString::number( coords.at( i )->x() ) + "," + 
	     QString::number( coords.at( i )->y() ) + " ";
    }
    poly.setAttribute( "points", s );
    exportGObject( poly, docNodeIn.namedItem( "polyline" ).namedItem( "gobject" ).toElement() );
    docNodeOut.appendChild( poly );
}

void SVGExport::exportPolyline( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QDomElement poly = docNodeOut.ownerDocument().createElement( "polyline" );
    //poly.setAttribute( "width",  docNodeIn.attribute( "width" ) );
    //poly.setAttribute( "height", docNodeIn.attribute( "height" ) );

    QList<Coord> coords;
    coords.setAutoDelete( true );
    coords = getPoints( docNodeIn );
    QString s;
    for( unsigned int i = 0; i < coords.count(); i++ ) {
        s += QString::number( coords.at( i )->x() ) + "," + 
	     QString::number( coords.at( i )->y() ) + " ";
    }
    poly.setAttribute( "points", s );
    exportGObject( poly, docNodeIn.namedItem( "polyline" ).namedItem( "gobject" ).toElement(), false );
    docNodeOut.appendChild( poly );
}

void SVGExport::exportEllipse( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QDomElement circle;
    // TODO Pie, Arc
    if( docNodeIn.attribute("rx").toInt() == docNodeIn.attribute("ry").toInt() )
    {
        circle = docNodeOut.ownerDocument().createElement( "circle" );
	circle.setAttribute( "r",  docNodeIn.attribute( "rx" ) );
    }
    else
    {
        circle = docNodeOut.ownerDocument().createElement( "ellipse" );
	circle.setAttribute( "rx",  docNodeIn.attribute( "rx" ) );
	circle.setAttribute( "ry",  docNodeIn.attribute( "ry" ) );
    }
    circle.setAttribute( "cx",  docNodeIn.attribute( "x" ) );
    circle.setAttribute( "cy",  docNodeIn.attribute( "y" ) );

    exportGObject( circle, docNodeIn.namedItem( "gobject" ).toElement() );
    docNodeOut.appendChild( circle );
}

void SVGExport::exportBezier( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QDomElement bezier = docNodeOut.ownerDocument().createElement( "path" );

    QList<Coord> coords;
    coords.setAutoDelete( true );
    coords = getPoints( docNodeIn.namedItem( "polyline" ).toElement() );
    QString s;
    bool bClosed = false;
    s = "M " + QString::number( coords.at( 1 )->x() ) + " " + 
               QString::number( coords.at( 1 )->y() ) + " ";
    unsigned int i = 2;
    while( i < coords.count() - 2 ) {
        // TODO : fix FLT_MAX comparison problem
        /*cout << "coord : " << (float)(coords.at( i )->x()) << " == " << (float)FLT_MAX << endl;
	cout << "coord : " << (float)(coords.at( i + 1 )->x()) << " == " << (float)FLT_MAX << endl;
        if( (coords.at( i )->x() == (float)FLT_MAX) || (coords.at( i + 1 )->x() == (float)FLT_MAX) ){
	    s += "L ";
	    s += QString::number( coords.at( i + 2 )->x() ) + " " + 
	         QString::number( coords.at( i + 2 )->y() ) + " ";
	}
	else
	{*/
            s += "C ";
            for( int n = 0; n < 3; n++ ) {
                s += QString::number( coords.at( i + n )->x() ) + " " + 
	             QString::number( coords.at( i + n )->y() ) + " ";
            }
            i += 3;
	//}
    }
    if( ( coords.at( 1 )->x() == coords.at( coords.count() - 1 )->x() ) &&
        ( coords.at( 1 )->y() == coords.at( coords.count() - 1 )->y() ) ) {
	bClosed = true;
        s += "Z";
    }
    bezier.setAttribute( "d", s );
    exportGObject( bezier, docNodeIn.namedItem( "polyline" ).namedItem( "gobject" ).toElement(), bClosed );
    docNodeOut.appendChild( bezier );
}

void SVGExport::exportCurve( QDomElement &docNodeOut, const QDomElement &docNodeIn )
{
    QDomElement curve = docNodeOut.ownerDocument().createElement("path");
    QString s;
    bool bClosed = false;
    bool first = true;
    float startx, starty;
    QList<Coord> coords;
    coords.setAutoDelete( true );
    QDomNodeList list = docNodeIn.childNodes();

    for( unsigned int i = 0 ; i < list.count(); i++ ) {
        if( list.item( i ).isElement() ) {
            QDomElement e = list.item( i ).toElement();

	    if( e.tagName() == "seg" )
	    {
	        coords = getSegments( e );
                if( first ) {
		    startx = coords.at( 0 )->x();
		    starty = coords.at( 0 )->y();
                    s += "M " + QString::number( startx ) + " " +
                                QString::number( starty ) + " ";
                    first = false;
		}
                if( coords.count() == 2 ) {
                    s += "L " + QString::number( coords.at( 1 )->x() ) + " " +
                                QString::number( coords.at( 1 )->y() ) + " ";
                }
                else if( coords.count() == 4 ) {
                    s += "C ";
                    for( int n = 1; n < 4; n++ )
                        s += QString::number( coords.at( n )->x() ) + " " +
                             QString::number( coords.at( n )->y() ) + " ";
	        }
                if( ( startx == coords.at( coords.count() - 1 )->x() ) &&
		    ( starty == coords.at( coords.count() - 1 )->y() ) ) {
		    bClosed = true;
                    s += "Z";
		}
	    }
        }
    }

    curve.setAttribute("d", s);
    exportGObject( curve, docNodeIn.namedItem( "gobject" ).toElement(), bClosed );
    docNodeOut.appendChild( curve );
}

QList<Coord> SVGExport::getSegments( const QDomElement &docNodeIn )
{
    QList<Coord> temp;
    int kind = docNodeIn.attribute( "kind" ).toInt();
    QDomNodeList list = docNodeIn.childNodes( );

    if( kind == GSegment::sk_Line ) {
        temp.append( createPoint( list.item( 0 ).toElement() ) );
	temp.append( createPoint( list.item( 1 ).toElement() ) );
    }
    else if( kind == GSegment::sk_Bezier ) {
        temp.append( createPoint( list.item( 0 ).toElement() ) );
	temp.append( createPoint( list.item( 1 ).toElement() ) );
        temp.append( createPoint( list.item( 2 ).toElement() ) );
	temp.append( createPoint( list.item( 3 ).toElement() ) );
   }
   return temp;
}

QList<Coord> SVGExport::getPoints( const QDomElement &docNodeIn )
{
    QList<Coord> temp;
    QDomNodeList list = docNodeIn.childNodes( );
    for( unsigned int i = 0 ; i < list.count() - 1 ; i++ )
         temp.append( createPoint( list.item( i ).toElement() ) );

    return temp;
}

Coord *SVGExport::createPoint( const QDomElement &docNodeIn )
{
    return new Coord( docNodeIn.attribute( "x" ).toFloat(), 
                      docNodeIn.attribute( "y" ).toFloat() );                           
}

void SVGExport::exportGObject( QDomElement &docNodeOut, const QDomElement &docNodeIn, bool bClosed ) {
    QString s;
    int fstyle = (GObject::FillInfo::Style) docNodeIn.attribute( "fillstyle" ).toInt();
    Qt::PenStyle ostyle = (Qt::PenStyle) docNodeIn.attribute( "strokestyle" ).toInt();

    s += "fill:";
    if( fstyle == GObject::FillInfo::NoFill  || !bClosed )
        s += "none ; ";
    else if( fstyle == GObject::FillInfo::SolidFill ) {
        s += docNodeIn.attribute( "fillcolor" ) + ";";
    }
    s += "stroke:";
    if( ostyle == Qt::NoPen )
        s += "none ; ";
    else
        s += docNodeIn.attribute( "strokecolor" ) + ";";
    s += "stroke-width:" + docNodeIn.attribute( "linewidth" );
    docNodeOut.setAttribute( "style", s );
    
    exportTransform( docNodeOut, docNodeIn.namedItem( "matrix" ).toElement() );
}

void SVGExport::exportTransform( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QString s;
    s += "matrix(" + docNodeIn.attribute( "m11" ) + " " + 
                     docNodeIn.attribute( "m12" ) + " " +
                     docNodeIn.attribute( "m21" ) + " " + 
		     docNodeIn.attribute( "m22" ) + " " + 
		     docNodeIn.attribute( "dx" )  + " " +
		     docNodeIn.attribute( "dy" )  + ")";
    docNodeOut.setAttribute( "transform", s );
}

void SVGExport::exportPixmap( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QDomElement pixmap = docNodeOut.ownerDocument().createElement( "image" );
    pixmap.setAttribute( "xlink:href", docNodeIn.attribute( "src" ) );
    exportGObject( pixmap, docNodeIn.namedItem( "gobject" ).toElement() );
    docNodeOut.appendChild( pixmap );
}

void SVGExport::exportText( QDomElement &docNodeOut, const QDomElement &docNodeIn ) {
    QStringList list;
    kdDebug() << "Text: " << docNodeIn.text() << endl;
    int pos1 = 0, pos2; 
    QString s = docNodeIn.text();
    // Note : had to use this code from GText since splitting does not work, see below.
    do {
        pos2 = s.find ( '\n', pos1 );
        if ( pos2 != -1 ) {
            QString sub = s.mid( pos1, pos2 - pos1 );
            list.append( sub );
            pos1 = pos2 + 1;
        }
        else {
            if ( s.length() - pos1 == 0 )
            break;
 
          QString sub = s.mid( pos1, s.length () - pos1 );
          list.append( sub );
        }
    } while ( pos2 != -1 );
    /*
    QRegExp reg("\n");
    list.split( reg, docNodeIn.text() );
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        //docNodeOut.appendChild( docNodeOut.ownerDocument().createCDATASection( (*it).local8Bit() ) );
	exportTextLine( docNodeOut, (*it).local8Bit(), 0, 0, 0 );
    }*/
    QDomElement f = docNodeIn.namedItem( "font" ).toElement();
    QFont font = QFont::defaultFont();
    font.setFamily( f.attribute( "face" ) );
    font.setPointSize( f.attribute( "point-size" ).toInt() );
    font.setWeight( f.attribute( "weight" ).toInt() );
    font.setItalic( f.attribute( "italic" ).toInt() );

    if( list.count() > 1 ) {
        QFontMetrics fm( font );
        float yoff = fm.ascent();

        for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
            int ws = fm.width( (*it).local8Bit() );
            float xoff = 0;
            if( docNodeIn.attribute( "align" ).toInt() == GText::TextInfo::AlignCenter )
                xoff = -ws / 2;
            else if( docNodeIn.attribute( "align" ).toInt() == GText::TextInfo::AlignRight )
                xoff = -ws;
            exportTextLine( docNodeOut, docNodeIn, (*it), font, xoff, yoff );
            yoff += fm.height();
        }
    }
    else
        exportTextLine( docNodeOut, docNodeIn, list.first(), font, 0, 0 );
}

void SVGExport::exportTextLine( QDomElement &docNodeOut, const QDomElement &docNodeIn,
                                const QString &text,
                                QFont &font, float xoff, float yoff ) {
    QDomElement element = docNodeOut.ownerDocument().createElement( "text" );
    element.setAttribute( "x", xoff );
    element.setAttribute( "y", yoff );
    addTextStyleAttribute( element, docNodeIn.namedItem( "gobject" ).toElement(), font );
    element.appendChild( docNodeOut.ownerDocument().createTextNode( text ) );
    docNodeOut.appendChild( element );
}

void SVGExport::addTextStyleAttribute( QDomElement &docNodeOut,
                                      const QDomElement &docNodeIn, QFont &font ) {

    QString s;
    Qt::PenStyle ostyle = (Qt::PenStyle) docNodeIn.attribute( "strokestyle" ).toInt();

    s += "font-family:" + font.family();
    s += "; font-size:" + QString::number( font.pointSize() );
    s += "; fill:";
    if( ostyle == Qt::NoPen )
        s += "none" ;
    else {
        s += docNodeIn.attribute( "strokecolor" ) + ";";
    }
    s += "font-style:" + QString( font.italic() ? "italic" : "normal" );
    
    docNodeOut.setAttribute( "style", s );
    exportTransform( docNodeOut, docNodeIn.namedItem( "matrix" ).toElement() );
}

#include <svgexport.moc>
