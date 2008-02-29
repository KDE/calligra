/* This file is part of the KDE project
   Copyright (C) 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#include "OperatorElement.h"
#include "RowElement.h"
#include "AttributeManager.h"

OperatorElement::OperatorElement( BasicElement* parent ) : TokenElement( parent )
{}

QPainterPath OperatorElement::renderForFence( const QString& raw, Form form )
{
    return QPainterPath();
}

void OperatorElement::renderToPath( const QString& raw, QPainterPath& path )
{
    Dictionary dict;
    dict.queryOperator( raw, determineOperatorForm() );
    
    AttributeManager manager;
    path.addText( path.currentPosition(), manager.font( this ), raw );

//    path.moveTo( path.currentPosition() + QPointF( lspace, 0.0 ) );
//    path.moveTo( path.currentPosition() + QPointF( rspace, 0.0 ) );
}

Form OperatorElement::determineOperatorForm() const
{
    // a bit of a hack - determine the operator's form with its position inside the
    // parent's element list. This is with the assumption that the parent is an 
    // ( inferred ) row element. If that is not the case return standart Prefix ( ? )
 
    if( dynamic_cast<RowElement*>( parentElement() ) == 0 )
        return Prefix;
    else if( parentElement()->childElements().first() == this )
        return Prefix;
    else if( parentElement()->childElements().last() == this )
        return Postfix;
    else
        return Infix;
}

double OperatorElement::parseMathSpace( const QString& value )  const
{
/*    if( value == "negativeveryverythinmathspace" )
        return -1*calculateEmExUnits( 0.055556, true );
    else if( value == "negativeverythinmathspace" )
        return -1*calculateEmExUnits( 0.111111, true );
    else if( value == "negativethinmathspace" )
        return -1*calculateEmExUnits( 0.166667, true );
    else if( value == "negativemediummathspace" )
        return -1*calculateEmExUnits( 0.222222, true );
    else if( value == "negativethickmathspace" )
        return -1*calculateEmExUnits( 0.277778, true );
    else if( value == "negativeverythickmathspace" )
        return -1*calculateEmExUnits( 0.333333, true );
    else if( value == "negativeveryverythickmathspace" )
        return -1*calculateEmExUnits( 0.388889, true );
    else if( value == "veryverythinmathspace" )
        return calculateEmExUnits( 0.055556, true );
    else if( value == "verythinmathspace" )
        return calculateEmExUnits( 0.111111, true );
    else if( value == "thinmathspace" )
        return calculateEmExUnits( 0.166667, true );
    else if( value == "mediummathspace" )
        return calculateEmExUnits( 0.222222, true );
    else if( value == "thickmathspace" )
        return calculateEmExUnits( 0.277778, true );
    else if( value == "verythickmathspace" )
        return calculateEmExUnits( 0.333333, true );
    else if( value == "veryverythickmathspace" )
        return calculateEmExUnits( 0.388889, true );
    else*/
        return -1.0;
}

Form OperatorElement::parseForm( const QString& value ) const
{
    if( value == "prefix" )
        return Prefix;
    else if( value == "infix" )
        return Infix;
    else if( value == "postfix" )
        return Postfix;
    else
        return InvalidForm;
}

ElementType OperatorElement::elementType() const
{
    return Operator;
}
