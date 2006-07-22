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

#include <qpainter.h>

#include "elementtype.h"
#include "sequenceelement.h"
#include "textelement.h"
#include "tokenelement.h"

KFORMULA_NAMESPACE_BEGIN

TokenElement::TokenElement( BasicElement* parent ) : StyleElement( parent ) {}

bool TokenElement::buildChildrenFromMathMLDom(QPtrList<BasicElement>& list, QDomNode n) {
    if (!n.isText())
        return false;
    QString textelements = n.toText().data().stripWhiteSpace();
    for (uint i = 0; i < textelements.length(); i++) {
        TextElement* child = new TextElement(textelements[i]);
        if (child != 0) {
            child->setParent(this);
            child->setCharFamily( charFamily() );
            child->setCharStyle( charStyle() );
			list.append(child);
        }
    }
//	parse();
	kdWarning() << "Num of children " << list.count() << endl;
    return true;
}

void TokenElement::writeMathMLText( QDomDocument& doc, QDomElement& element )
{
    QString value;
    for ( iterator it = begin(); it != end(); it++ ) {
        value += it->getCharacter();
    }
    element.appendChild( doc.createTextNode( value ) );
}

KFORMULA_NAMESPACE_END
