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

#include "text.h"
#include "GText.h"
#include "GGroup.h"
#include <qfontmetrics.h>


/********************* Text stuff *******************************/

void SVGTextElement::addContent( const QString &str ) {
    QFontMetrics fm( font );
    QString text = str.simplifyWhiteSpace();
    int xWidth = fm.width( text );
    Coord p( x + xpos, y + obj->cursorY() );
    xpos += xWidth;
    //ypos += fm.height( );
    //kdDebug() << "CursorX: " << obj->cursorX() <<  "xWidth:  " <<  xWidth << endl;
    //kdDebug() << "Textlen: " << text.length() << "Text: " << text.latin1() << endl;
    obj->setText( text );
    GText *newobj = new GText( *obj );
    newobj->setOrigin( p );
    group->addObject( newobj );
}


void SVGTextElement::setAttributes( const QXmlAttributes& atts ) {
    SVGComposite::setAttributes( atts );
    x = atts.value("x").toFloat();
    y = atts.value("y").toFloat();
    
    obj  = new GText( NULL );
    setStyleProperties( obj );
    
    if( parent ) parent->addObject( this );
}


GObject *SVGTextElement::build() {
    if( !transform.isEmpty() )
    {
        performTransformations( group );
        transform = "";
    } 
    return group;
}


void SVGTextElement::processSubStyle( QString &command, QString &param ) {
    if( command == "font-family" )
    {
        font.setFamily(param);
    }
    else if( command == "font-size" )
    {
//        font.setPointSize( (int) convertNumToPt( param ) );
    }
    else if( command == "font-weight" )
    {
        if( param == "bold" )
            font.setWeight( QFont::Bold );
        else if( param == "normal" )
            font.setWeight( QFont::Normal );

    }
    else if( command == "font-style" )
    {
        if( param == "italic" )
            font.setItalic( true );
    }
}


void SVGTextElement::setStyleProperties( GObject *obj ) {
    processStyle();

    if( obj )
    {
        obj->setOutlineStyle( Qt::SolidLine );

        //obj->setFillColor(styleProp->fillInfo.color);
        //obj->setFillStyle(styleProp->fillInfo.fstyle);
        obj->setOutlineColor( fillInfo.color );
        //obj->setOutlineWidth(styleProp->OutlineWidth);
        //obj->setOutlineInfo( styleProp->outlineInfo );
        //obj->setFillInfo( styleProp->fillInfo );

        ((GText *)obj)->setFont( font );
    }
}
