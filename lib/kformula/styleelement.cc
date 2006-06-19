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
                                                     family( anyFamily )
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
            family = normalFamily;
            style = normalChar;
        }
        else if ( variantStr == "bold" ) {
            family = normalFamily;
            style = boldChar;
        }
        else if ( variantStr == "italic" ) {
            family = normalFamily;
            style = italicChar;
        }
        else if ( variantStr == "bold-italic" ) {
            family = normalFamily;
            style = boldItalicChar;
        }
        else if ( variantStr == "double-struck" ) {
            family = doubleStruckFamily;
            style = normalChar;
        }
        else if ( variantStr == "bold-fraktur" ) {
            family = frakturFamily;
            style = boldChar;
        }
        else if ( variantStr == "script" ) {
            family = scriptFamily;
            style = normalChar;
        }
        else if ( variantStr == "bold-script" ) {
            family = scriptFamily;
            style = boldChar;
        }
        else if ( variantStr == "fraktur" ) {
            family = frakturFamily;
            style = boldChar;
        }
        else if ( variantStr == "sans-serif" ) {
            family = sansSerifFamily;
            style = normalChar;
        }
        else if ( variantStr == "bold-sans-serif" ) {
            family = sansSerifFamily;
            style = boldChar;
        }
        else if ( variantStr == "sans-serif-italic" ) {
            family = sansSerifFamily;
            style = italicChar;
        }
        else if ( variantStr == "sans-serif-bold-italic" ) {
            family = sansSerifFamily;
            style = boldItalicChar;
        }
        else if ( variantStr == "monospace" ) {
            family = monospaceFamily;
            style = normalChar;
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
        // TODO
	}
    QString backgroundStr = element.attribute( "mathbackground" );
    if ( !backgroundStr.isNull() ) {
        // TODO
	}
    return true;
}

void StyleElement::calcSizes( const ContextStyle& style,
                              ContextStyle::TextStyle tstyle,
                              ContextStyle::IndexStyle istyle,
                              double factor )
{
    inherited::calcSizes( style, tstyle, istyle,
                          getProportion( style, factor ) );
}

void StyleElement::draw( QPainter& painter, const LuPixelRect& r,
                         const ContextStyle& context,
                         ContextStyle::TextStyle tstyle,
                         ContextStyle::IndexStyle istyle,
                         double factor,
                         const LuPixelPoint& parentOrigin )
{
    inherited::draw( painter, r, context, tstyle, istyle, 
                     getProportion( context, factor ), parentOrigin );
}

double StyleElement::getProportion( const ContextStyle& context, double factor )
{
    if ( ! ownSize )
        return factor;
    if ( absoluteSize )
        return factor * size / context.baseSize();
    return factor * factor;
}


KFORMULA_NAMESPACE_END
