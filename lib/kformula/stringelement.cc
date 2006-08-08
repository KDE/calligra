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

#include "textelement.h"
#include "stringelement.h"

KFORMULA_NAMESPACE_BEGIN

StringElement::StringElement( BasicElement* parent ) : TokenElement( parent ) {
}

int StringElement::buildChildrenFromMathMLDom(QPtrList<BasicElement>& list, QDomNode n) 
{
    int count = inherited::buildChildrenFromMathMLDom( list, n );
    if ( count == -1 )
        return -1;
    TextElement* child = new TextElement( '"' );
    child->setParent( this );
    child->setCharFamily( charFamily() );
    child->setCharStyle( charStyle() );
    insert( 0, child );
    child = new TextElement( '"' );
    child->setParent( this );
    child->setCharFamily( charFamily() );
    child->setCharStyle( charStyle() );
    insert( countChildren(), child );
    return count;
}

void StringElement::writeMathML( QDomDocument& doc, QDomNode& parent, bool oasisFormat )
{
    QDomElement de = doc.createElement( oasisFormat ? "math:ms" : "ms" );
    writeMathMLAttributes( de );
    writeMathMLContent( doc, de, oasisFormat );
    parent.appendChild( de );
}

void StringElement::writeMathMLContent( QDomDocument& doc, QDomElement& element, bool oasisFormat )
{
    iterator it = begin();
    iterator itEnd = end();
    for ( it++, itEnd--; it != itEnd; it++ ) {
        it->writeMathML( doc, element, oasisFormat );
    }
}

KFORMULA_NAMESPACE_END
