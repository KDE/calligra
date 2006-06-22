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

#include "kformuladefs.h"
#include "textelement.h"
#include "identifierelement.h"

KFORMULA_NAMESPACE_BEGIN

IdentifierElement::IdentifierElement( BasicElement* parent ) : TokenElement( parent ) {
}

void IdentifierElement::calcSizes( const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle,
                                   ContextStyle::IndexStyle istyle,
                                   StyleAttributes& style ) {
    // Identifiers, by default, use italic style when length == 1
    if ( countChildren() == 1 ) {
        TextElement *e = dynamic_cast<TextElement*>( getChild( (uint) 0 ) );
        if (e != 0) {
            if ( e->getCharStyle() == anyChar ) { // This means no specified variant
                e->setCharStyle( italicChar );
                inherited::calcSizes( context, tstyle, istyle, style );
                e->setCharStyle( anyChar );
            } else {
                inherited::calcSizes( context, tstyle, istyle, style );
            }
        } else {
            inherited::calcSizes( context, tstyle, istyle, style );
        }
    } else {
        inherited::calcSizes( context, tstyle, istyle, style );
    }
}

void IdentifierElement::draw( QPainter& painter, const LuPixelRect& r,
                              const ContextStyle& context,
                              ContextStyle::TextStyle tstyle,
                              ContextStyle::IndexStyle istyle,
                              StyleAttributes& style,
                              const LuPixelPoint& parentOrigin ) 
{
    if ( customCharStyle() ) {
        style.setCharStyle( getCharStyle() );
    }
    else if ( countChildren() == 1 ) {
        style.setCharStyle( italicChar );
    }
    else {
        style.setCharStyle( normalChar );
    }

    StyleElement::draw( painter, r, context, tstyle, istyle, style, parentOrigin );
    style.reset();
}

KFORMULA_NAMESPACE_END
