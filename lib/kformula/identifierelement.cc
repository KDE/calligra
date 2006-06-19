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

void IdentifierElement::calcSizes( const ContextStyle& style,
                                   ContextStyle::TextStyle tstyle,
                                   ContextStyle::IndexStyle istyle,
                                   double factor ) {
    // Identifiers, by default, use italic style when length == 1
    if ( countChildren() == 1 ) {
        TextElement *e = dynamic_cast<TextElement*>( getChild( (uint) 0 ) );
        if (e != 0) {
            if ( e->getCharStyle() == anyChar ) { // This means no specified variant
                e->setCharStyle( italicChar );
                inherited::calcSizes( style, tstyle, istyle, factor );
                e->setCharStyle( anyChar );
            } else {
                inherited::calcSizes( style, tstyle, istyle, factor );
            }
        } else {
            inherited::calcSizes( style, tstyle, istyle, factor );
        }
    } else {
        inherited::calcSizes( style, tstyle, istyle, factor );
    }
}

void IdentifierElement::draw( QPainter& painter, const LuPixelRect& r,
                       const ContextStyle& context,
                       ContextStyle::TextStyle tstyle,
                       ContextStyle::IndexStyle istyle,
                       double factor,
                       const LuPixelPoint& parentOrigin ) {
    // Identifiers, by default, use italic style when length == 1
    if ( countChildren() == 1 ) {
        TextElement *e = dynamic_cast<TextElement*>( getChild( (uint) 0 ) );
        if (e != 0) {
            if ( e->getCharStyle() == anyChar ) { // This means no specified variant
                e->setCharStyle( italicChar );
                inherited::draw( painter, r, context, tstyle, istyle, factor, parentOrigin );
                e->setCharStyle( anyChar );
            } else {
                inherited::draw( painter, r, context, tstyle, istyle, factor, parentOrigin );
            }
        } else {
            inherited::draw( painter, r, context, tstyle, istyle, factor, parentOrigin );
        }
    } else {
        inherited::draw( painter, r, context, tstyle, istyle, factor, parentOrigin );
    }
}

KFORMULA_NAMESPACE_END
