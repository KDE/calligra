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

#include "svgmain.h"
#include <kdebug.h>
#include "GGroup.h"
#include "GPixmap.h"

/********************* Element && Composite *******************************/


SVGGraphicElement::SVGGraphicElement( SVGComposite *par )// : parent( par )
{
   SVGElement::setParent( par );
   if( parent )
        styleCopy( *parent );
};


SVGGraphicElement::~SVGGraphicElement()
{
    kdDebug() << "Deleting SVGElement with id : " << id().local8Bit() << endl;
}


void SVGElement::setParent( SVGComposite *par) { 
   parent = par;
   if( parent )
       _ownerSVGElement = ( parent->type() == "svg" ) ? (SVGSVGElement *)parent : NULL;    
};


void SVGElement::setAttributes( const QXmlAttributes& atts ) { 
    setId( atts.value( "id" ) );
};


void SVGGraphicElement::setParent( SVGComposite *par) { 
   SVGElement::setParent( par );
   if( parent )
        styleCopy( *parent );
};


void SVGGraphicElement::setAttributes( const QXmlAttributes& atts ) { 
    SVGElement::setAttributes( atts );
    transform = atts.value( "transform" );
    style = atts.value("style");
};

    
SVGComposite::SVGComposite( SVGComposite *par ) //: SVGElement( par )
{
    setParent( par );
    group = new GGroup( NULL );
    //if( parent ) parent->addObject( this );
};


SVGComposite::~SVGComposite() {
    QListIterator<SVGElement> it( children );
    for( ;it.current() ; ++it ) {
        delete it.current();
    }
}


void SVGComposite::setAttributes( const QXmlAttributes& atts ) {
    SVGElement::setAttributes( atts );
    transform = atts.value( "transform" );
    style = atts.value( "style" );

    setStyleProperties( 0 );
    
    if( parent ) parent->addObject( this );
}


GObject *SVGComposite::build() {
    kdDebug() << " Doing : " << children.count() << " children." << endl;
    QListIterator<SVGElement> it( children );
    GObject *obj;
    for( ;it.current() ; ++it ) {
        obj = it.current()->build();
        if( obj ) group->addObject( obj );
    }

    if( !hasIdentifier() )
    {
        QListIterator<SVGElement> it( children );
        for( ;it.current() ; ++it ) {
            if( !it.current()->hasIdentifier() )
	        delete it.current();
        }
    }
    children.clear();
    if( !transform.isEmpty() )
    {
        performTransformations( group );
        transform = "";
    }    
    return group;
}


SVGSymbolElement::SVGSymbolElement( SVGComposite *par ) : SVGComposite( par )
{
    if( parent && parent->type() != "defs" ) parent = NULL;
};


void SVGSVGElement::setAttributes( const QXmlAttributes& atts ) {
    SVGComposite::setAttributes( atts );

    _width.setValueAsString( atts.value("width") );
    _height.setValueAsString( atts.value("height") );
}

void SVGImageElement::setAttributes( const QXmlAttributes& atts ) {
    SVGGraphicElement::setAttributes( atts );

    uri = atts.value( "xlink:href" );
    x.setValueAsString( atts.value("x") );
    //x.newValueSpecifiedUnits( SVGLength::SVG_LENGTHTYPE_CM, 0 );
    y.setValueAsString( atts.value("y") );
    width.setValueAsString( atts.value("width") );
    height.setValueAsString( atts.value("height") );
    //height.newValueSpecifiedUnits( SVGLength::SVG_LENGTHTYPE_CM, 25 );
}


GObject *SVGImageElement::build() {
    obj = new GPixmap( NULL, uri );
    /*Coord p( x.value(), y.value() );   obj->_addPoint( 0, p );
    p.x( x.value() + width.value() );  obj->_addPoint( 1, p );
    p.y( y.value() + height.value() ); obj->_addPoint( 2, p );
    p.x( x.value() );                  obj->_addPoint( 3, p );*/

    performTransformations( obj );
    setStyleProperties( obj );
    return obj;
}
