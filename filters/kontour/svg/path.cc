/*
    Copyright (C) 2001, Rob Buis <rwlbuis@wanadoo.nl>.
    This file is part of the KDE project

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

DESCRIPTION
    Filter to convert a svg file to a killustrator document using koDocument
*/

#include "path.h"
#include "GCurve.h"
#include "GOval.h"
#include "style.h"
#include <kdebug.h>

/********************* Path Shape ********************************/

void SVGPathElement::setAttributes( const QXmlAttributes& atts ) {
    SVGGraphicElement::setAttributes( atts );

    d = atts.value( "d" );
}


GObject *SVGPathElement::build() {
    GCurve *curve = new GCurve( NULL );
    GOval  *oval  = new GOval( NULL );
    enum { mAbs, mRel,
           lAbs, lRel,
	   zAbs, zRel,
	   hAbs, hRel,
	   vAbs, vRel,
	   qAbs, qRel,
	   tAbs, tRel,
	   cAbs, cRel,
	   sAbs, sRel,
	   aAbs, aRel,
 	   nrCommands };

    QChar commands[] = { 'M', 'm', 'L', 'l', 'Z', 'z', 'H', 'h', 'V', 'v',
                         'Q', 'q', 'T', 't', 'C', 'c', 'S', 's', 'A', 'a' };

    QString seperator( "|" );
    unsigned int i = 0;
    for( i = 0; i < nrCommands; i++ )
        d.replace( QRegExp( commands[i] ), seperator + commands[i] );

    QValueList<Coord> points;
    float curx = 0,    cury = 0;
    float startx = -1, starty = -1;
    QRegExp reg("[a-zA-Z,() ]");
    QStringList coords;
    Coord p, p2, p3, p4;
    bool bRelative;
    float tox, toy;
    char lastCommand = ' ', curCommand;
    QStringList::Iterator it2;
    QStringList path = QStringList::split( seperator, d );
    for( QStringList::Iterator it = path.begin(); it != path.end(); it++ ) {
  
	(*it).simplifyWhiteSpace();
	(*it).replace( QRegExp("-"), " -" );
	(*it).replace( QRegExp("+"), " +" );
        kdDebug() << "Subpath : " << (*it).local8Bit() << endl;
      
	bRelative = false;
	coords = QStringList::split( reg, (*it) );
	switch( curCommand = ((*it)[0]).latin1() ) {
	case 'm':    bRelative = true;
	case 'M':    for( it2 = coords.begin(); it2 != coords.end(); ) {
			 // first time, move current x, y
			 if( it2 == coords.begin() ) {
  		             startx = curx = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		             starty = cury = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         }
			 else { // not the first time, define line between
			        // current x, y and new x, y 
		             p.x( curx ); p.y( cury );
			     curx = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		             cury = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		             p2.x( curx ); p2.y( cury );
			     
			     curve->addLineSegment( p, p2 );
			 }
		     }
	             break;
	case 'l':    bRelative = true;
	case 'L':    for( it2 = coords.begin(); it2 != coords.end(); ) {
	                 p.x( curx ); p.y( cury );
			 curx = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         cury = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
         	         p2.x( curx ); p2.y( cury );
			 
			 curve->addLineSegment( p, p2 );
		     }
	             break;
	case 'h':    bRelative = true;
	case 'H':    if( coords.begin() == coords.end() ) break;
 
		     it2 = coords.begin();
		     p.x( curx ); p.y( cury );
		     curx = bRelative ? curx + (*it2).toFloat() : (*it2).toFloat();	     
         	     p2.x( curx ); p2.y( cury );

		     curve->addLineSegment( p, p2 );
	             break;
	case 'v':    bRelative = true;
	case 'V':    if( coords.begin() == coords.end() ) break;

		     it2 = coords.begin();
		     p.x( curx ); p.y( cury );
		     cury = bRelative ? cury + (*it2).toFloat() : (*it2).toFloat();
         	     p2.x( curx ); p2.y( cury );

		     curve->addLineSegment( p, p2 );
	             break;
        case 'c':    bRelative = true;
	case 'C':    for( it2 = coords.begin(); it2 != coords.end(); ) {
                         p.x( curx ); p.y( cury );
			 tox = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         toy = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
         	         p2.x( tox ); p2.y( toy );

			 tox = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         toy = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
         	         p3.x( tox ); p3.y( toy );

			 curx = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         cury = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
         	         p4.x( curx ); p4.y( cury );
			 
			 curve->addBezierSegment( p, p2, p3, p4 );
		     }
	             break;
	case 's':    bRelative = true;
	case 'S':    for( it2 = coords.begin(); it2 != coords.end(); ) {
		         p.x( curx );
		         p.y( cury );
		         if( (lastCommand == 'c' || lastCommand == 'C') ||
			     (lastCommand == 's' || lastCommand == 'S') ) {
			     tox = bRelative ? curx + (p.x() - p3.x()) : p.x() + (p.x() - p3.x());
		             toy = bRelative ? cury + (p.y() - p3.y()) : p.y() + (p.y() - p3.y());
         	             p2.x( tox ); p2.y( toy );
			 }
			 else {
			     p2.x( p.x() ); p2.y( p.y() );
			 }
			 tox = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         toy = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
         	         p3.x( tox ); p3.y( toy );
			 
			 curx = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         cury = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
         	         p4.x( curx ); p4.y( cury );
			 
			 curve->addBezierSegment( p, p2, p3, p4 );
		     }
	             break;
        case 'q':    bRelative = true;
	case 'Q':    for( it2 = coords.begin(); it2 != coords.end(); ) {
		         
                         p.x( curx ); p.y( cury );
			 tox = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         toy = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
         	         p2.x( tox );    p2.y( toy ); 		
       	                 p3.x( p2.x() ); p3.y( p2.y() );
			 
			 curx = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         cury = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
         	         p4.x( curx ); p4.y( cury );
			 
			 curve->addBezierSegment( p, p2, p3, p4 );
		     }
	             break;
	case 't':    bRelative = true;
	case 'T':    for( it2 = coords.begin(); it2 != coords.end(); ) {
		         p.x( curx ); p.y( cury );
		         if( (lastCommand == 'q' || lastCommand == 'Q') ||
			     (lastCommand == 't' || lastCommand == 'T') ) {
			     tox = bRelative ? curx + p.x() + (p.x() - p3.x()) : p.x() + (p.x() - p3.x());
		             toy = bRelative ? cury + p.y() + (p.y() - p3.y()) : p.y() + (p.y() - p3.y());
         	             p2.x( tox ); p2.y( toy );
			 }
			 else {
			     p2.x( p.x() ); p2.y( p.y() );
			 }
         	         p3.x( p2.x() ); p3.y( p2.y() );
			 
			 curx = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         cury = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
         	         p4.x( curx ); p4.y( cury );
			 
			 curve->addBezierSegment( p, p2, p3, p4 );
		     }
	             break;
     	case 'z':
	case 'Z':    if( startx != -1 && starty != -1 ) {
	                 p.x( curx );    p.y( cury );
         	         p2.x( startx ); p2.y( starty );
			     			 
			 curve->addLineSegment( p, p2 );
			 curve->setClosed( true );
	             }
		     startx = -1;
		     starty = -1;
		     break;
        case 'a':    bRelative = true;
	case 'A':    for( it2 = coords.begin(); it2 != coords.end(); ) {
                         p.x( curx ); p.y( cury );
			 oval->setStartPoint( p );
			 outlineInfo.shape = GObject::OutlineInfo::PieShape;
			 setStyleProperties( oval );

                         // rx, ry, ignore
			 tox = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         toy = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
                         oval->setAngles( 180, 270 );
                         
			 tox = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         toy = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
			 tox = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();

                         // x, y
			 curx = bRelative ? curx + (*(it2++)).toFloat() : (*(it2++)).toFloat();
		         cury = bRelative ? cury + (*(it2++)).toFloat() : (*(it2++)).toFloat();
         	         p4.x( curx ); p4.y( cury );
			 oval->setEndPoint( p4 );

                         
                         oval->getPath( points );
			 for( i = 0; i < points.count() - 1; i += 2 )
			 {
  			     curve->addLineSegment( points[ i ],
			                            points[ i + 1 ] );
			 }
		     }
	             break;

	}
	lastCommand = curCommand;
    }
    delete oval;

    setStyleProperties( curve );
    performTransformations( curve );
    return curve;
}
