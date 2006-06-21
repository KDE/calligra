/* This file is part of the KDE project
   Copyright (C) 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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

#include "basicelement.h"
#include "styleelement.h"

KFORMULA_NAMESPACE_BEGIN

StyleElement::StyleElement( BasicElement* parent ) : SequenceElement( parent ), 
                                                     ownSize( false ),
                                                     absoluteSize( true ),
                                                     style( anyChar ),
                                                     custom_style( false ),
                                                     family( anyFamily ),
                                                     custom_family( false ),
                                                     color( "#000000" ),
                                                     custom_color( false ),
                                                     background( "#FFFFFF" ),
                                                     custom_background( false )
{
}

void StyleElement::setAbsoluteSize( double s )
{ 
        kdDebug( DEBUGID) << "Setting size: " << s << endl;
        ownSize = true;
        absoluteSize = true;
        size = s; 
}

void StyleElement::setRelativeSize( double f )
{ 
        kdDebug( DEBUGID) << "Setting factor: " << f << endl;
        ownSize = true;
        absoluteSize = false;
        factor = f;
}

// TODO: Support for deprecated attributes
bool StyleElement::readAttributesFromMathMLDom( const QDomElement& element )
{
    kdDebug( DEBUGID ) << "StyleElement::readAttributesFromMathMLDom\n";
    if ( !BasicElement::readAttributesFromDom( element ) ) {
        return false;
    }

    // MathML Section 3.2.2
    QString variantStr = element.attribute( "mathvariant" );
    if ( !variantStr.isNull() ) {
        if ( variantStr == "normal" ) {
            setCharStyle( normalChar );
            setCharFamily( normalFamily );
        }
        else if ( variantStr == "bold" ) {
            setCharStyle( boldChar );
            setCharFamily( normalFamily );
        }
        else if ( variantStr == "italic" ) {
            setCharStyle( italicChar );
            setCharFamily( normalFamily );
        }
        else if ( variantStr == "bold-italic" ) {
            setCharStyle( boldItalicChar );
            setCharFamily( normalFamily );
        }
        else if ( variantStr == "double-struck" ) {
            setCharStyle( normalChar );
            setCharFamily( doubleStruckFamily );
        }
        else if ( variantStr == "bold-fraktur" ) {
            setCharStyle( boldChar );
            setCharFamily( frakturFamily );
        }
        else if ( variantStr == "script" ) {
            setCharStyle( normalChar );
            setCharFamily( scriptFamily );
        }
        else if ( variantStr == "bold-script" ) {
            setCharStyle( boldChar );
            setCharFamily( scriptFamily );
        }
        else if ( variantStr == "fraktur" ) {
            setCharStyle( boldChar );
            setCharFamily( frakturFamily );
        }
        else if ( variantStr == "sans-serif" ) {
            setCharStyle( normalChar );
            setCharFamily( sansSerifFamily );
        }
        else if ( variantStr == "bold-sans-serif" ) {
            setCharStyle( boldChar );
            setCharFamily( sansSerifFamily );
        }
        else if ( variantStr == "sans-serif-italic" ) {
            setCharStyle( italicChar );
            setCharFamily( sansSerifFamily );
        }
        else if ( variantStr == "sans-serif-bold-italic" ) {
            setCharStyle( boldItalicChar );
            setCharFamily( sansSerifFamily );
        }
        else if ( variantStr == "monospace" ) {
            setCharStyle( normalChar );
            setCharFamily( monospaceFamily );
        }
    }

    QString sizeStr = element.attribute( "mathsize" );
    if ( !sizeStr.isNull() ) {
        // FIXME: This is broken
        kdDebug( DEBUGID ) << "MathSize attribute " << sizeStr << endl;
        bool ok;
        float s = sizeStr.toFloat( &ok );
        if ( ok ) {
            setAbsoluteSize( s );
        }
    }

    QString colorStr = element.attribute( "mathcolor" );
    if ( !colorStr.isNull() ) {
        color.setNamedColor( colorStr );
        custom_color = true;
	}
    QString backgroundStr = element.attribute( "mathbackground" );
    if ( !backgroundStr.isNull() ) {
        background.setNamedColor( backgroundStr );
        custom_background = true;
	}
    return true;
}

void StyleElement::setCharStyle( CharStyle cs )
{
    style = cs;
    custom_style = true;
}

void StyleElement::setCharFamily( CharFamily cf )
{
    family = cf;
    custom_family = true;
}

void StyleElement::calcSizes( const ContextStyle& context,
                              ContextStyle::TextStyle tstyle,
                              ContextStyle::IndexStyle istyle,
                              StyleAttributes& style )
{
    style.setSizeFactor( getSizeFactor( context, style.getSizeFactor() ) );
    inherited::calcSizes( context, tstyle, istyle, style );
    style.resetSizeFactor();
}

void StyleElement::draw( QPainter& painter, const LuPixelRect& r,
                         const ContextStyle& context,
                         ContextStyle::TextStyle tstyle,
                         ContextStyle::IndexStyle istyle,
                         StyleAttributes& style,
                         const LuPixelPoint& parentOrigin )
{
    style.setSizeFactor( getSizeFactor( context, style.getSizeFactor() ) );
    inherited::draw( painter, r, context, tstyle, istyle, style, parentOrigin );
    style.resetSizeFactor();
}

double StyleElement::getSizeFactor( const ContextStyle& context, double factor )
{
    if ( ! ownSize )
        return factor;
    if ( absoluteSize )
        return factor * size / context.baseSize();
    return factor * factor;
}

KFORMULA_NAMESPACE_END
