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

#include "GPolyline.h"
#include "GOval.h"
#include "GPolygon.h"
#include "GBezier.h"
#include "GCurve.h"
//#include "GGroup.h"
#include "basicshapes.h"

/********************* Basic Shapes *******************************/


void SVGRectElement::setAttributes( const QXmlAttributes& atts ) {
    SVGGraphicElement::setAttributes( atts );

    x.setValueAsString( atts.value("x") );
    //x.newValueSpecifiedUnits( SVGLength::SVG_LENGTHTYPE_CM, 0 );
    y.setValueAsString( atts.value("y") );
    rx.setValueAsString( atts.value("rx") );
    ry.setValueAsString( atts.value("ry") );
    width.setContext( this, (StyleProperty *)this );
    width.setValueAsString( atts.value("width") );
    height.setContext( this, (StyleProperty *)this );
    height.setValueAsString( atts.value("height") );
    //height.newValueSpecifiedUnits( SVGLength::SVG_LENGTHTYPE_CM, 25 );
}


GObject *SVGRectElement::build() {
    obj = new GPolygon( NULL, GPolygon::PK_Rectangle );
    Coord p( x.value(), y.value() );   obj->_addPoint( 0, p );
    p.x( x.value() + width.value() );  obj->_addPoint( 1, p );
    p.y( y.value() + height.value() ); obj->_addPoint( 2, p );
    p.x( x.value() );                  obj->_addPoint( 3, p );

    performTransformations( obj );
    // Note : killu can only handle one of rx and ry
    outlineInfo.roundness = rx.value() < ry.value() ? ry.value() : rx.value();
    setStyleProperties( obj );
    return obj;
}


void SVGCircleElement::setAttributes( const QXmlAttributes& atts ) {
    SVGGraphicElement::setAttributes( atts );

    cx.setValueAsString( atts.value("cx") );
    cy.setValueAsString( atts.value("cy") );
    r.setValueAsString( atts.value("r") );
}


GObject *SVGCircleElement::build() {
    obj = new GOval( NULL );
    Coord p1( cx.value() - r.value(), cy.value() - r.value() );
    Coord p2( cx.value() + r.value(), cy.value() + r.value() );

    obj->setStartPoint( p1 );
    obj->setEndPoint( p2 );

    outlineInfo.shape = GObject::OutlineInfo::DefaultShape;
    setStyleProperties( obj );
    performTransformations( obj );
    return obj;
}


void SVGEllipseElement::setAttributes( const QXmlAttributes& atts ) {
    SVGGraphicElement::setAttributes( atts );

    cx.setValueAsString( atts.value("cx") );
    cy.setValueAsString( atts.value("cy") );
    rx.setValueAsString( atts.value("rx") );
    ry.setValueAsString( atts.value("ry") );
}


GObject *SVGEllipseElement::build() {
    obj = new GOval( NULL );
    Coord p1( cx.value() - rx.value(), cy.value() - ry.value() );
    Coord p2( cx.value() + rx.value(), cy.value() + ry.value() );

    obj->setStartPoint( p1 );
    obj->setEndPoint( p2 );

    outlineInfo.shape = GObject::OutlineInfo::DefaultShape;
    setStyleProperties( obj );
    performTransformations( obj );
    return obj;
}


void SVGLineElement::setAttributes( const QXmlAttributes& atts ) {
    SVGGraphicElement::setAttributes( atts );

    x1.setValueAsString( atts.value("x1") );
    y1.setValueAsString( atts.value("y1") );
    x2.setValueAsString( atts.value("x2") );
    y2.setValueAsString( atts.value("y2") );
}


GObject *SVGLineElement::build() {
    obj = new GPolyline( NULL );
    Coord p( x1.value(), y1.value() );    obj->_addPoint( 0, p );
    p.x( x2.value() ); p.y( y2.value() ); obj->_addPoint( 1, p );

    performTransformations( obj );
    setStyleProperties( obj );
    return obj;
}


void SVGPolylineElement::setAttributes( const QXmlAttributes& atts ) {
    SVGGraphicElement::setAttributes( atts );

    points = atts.value( "points" );
}


GObject *SVGPolylineElement::build() {
    GObject *obj = processPoints();

    setStyleProperties( obj );
    performTransformations( obj );    
    return obj;
}


GObject *SVGPolygonElement::build() {
    GObject *obj = processPoints( true );

    setStyleProperties( obj );
    performTransformations( obj );    
    return obj;
}


GObject *SVGPolylineElement::processPoints( bool bPolygon ) {
    // Make coordinates from the points string and put them in a list
    points = points.simplifyWhiteSpace();
    QRegExp reg("[, ]");
    QStringList lijst = QStringList::split( reg, points );
    unsigned int i = 0;
    for( ; i < lijst.count(); i = i + 2 )
	appendItem( new SVGPoint( lijst[i].toFloat(), lijst[i + 1].toFloat() ) );

    // Is the poly closed, then it is a polygon, else a polyline
    GPolyline *obj;
    if( bPolygon || ( lijst[i - 2].toFloat() == lijst[0].toFloat() && 
                      lijst[i - 1].toFloat() == lijst[1].toFloat() ) )
        obj = new GPolygon( NULL );
    else
        obj = new GPolyline( NULL );

    // Append coordinates to polygon/polyline 
    unsigned long index = 0;
    //kdDebug() << "NumberOfItems : " << numberOfItems() << endl;
    for( ; index < numberOfItems() ; )
	obj->_addPoint( index++, Coord( ((SVGPoint *) getItem( index ))->x(),
	                                ((SVGPoint *) getItem( index ))->y() ) );
    return obj;
}
