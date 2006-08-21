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

#include <algorithm>

#include <qpainter.h>

#include "entities.h"
#include "elementtype.h"
#include "sequenceelement.h"
#include "textelement.h"
#include "glyphelement.h"
#include "tokenelement.h"

KFORMULA_NAMESPACE_BEGIN

TokenElement::TokenElement( BasicElement* parent ) : TokenStyleElement( parent ) {}

int TokenElement::buildChildrenFromMathMLDom(QPtrList<BasicElement>& list, QDomNode n) 
{
    while ( ! n.isNull() ) {
        if ( n.isText() ) {
            QString textelements = n.toText().data();
            textelements = textelements.stripWhiteSpace();
                
            for (uint i = 0; i < textelements.length(); i++) {
                TextElement* child = new TextElement(textelements[i]);
                child->setParent(this);
                child->setCharFamily( charFamily() );
                child->setCharStyle( charStyle() );
                list.append(child);
            }
        }
        else if ( n.isEntityReference() ) {
            QString entity = n.toEntityReference().nodeName();
            const entityMap* begin = entities;
            const entityMap* end = entities + entityMap::size() / sizeof( entityMap );
            const entityMap* pos = std::lower_bound( begin, end, entity.ascii() );
            if ( pos == end || QString( pos->name ) != entity ) {
                kdWarning() << "Invalid entity refererence: " << entity << endl;
            }
            else {
                TextElement* child = new TextElement( QChar( pos->unicode ) );
                child->setParent(this);
                child->setCharFamily( charFamily() );
                child->setCharStyle( charStyle() );
                list.append(child);
            }
        }
        else if ( n.isElement() ) {
            // Only mglyph element is allowed
            QDomElement e = n.toElement();
            if ( e.tagName().lower() != "mglyph" ) {
                kdWarning( DEBUGID ) << "Invalid element inside Token Element\n";
                return -1;
            }
            GlyphElement* child = new GlyphElement();
            child->setParent(this);
            child->setCharFamily( charFamily() );
            child->setCharStyle( charStyle() );
            if ( child->buildFromMathMLDom( e ) == -1 ) {
                return -1;
            }
            list.append( child );
        }
        else {
            kdWarning() << "Invalid content in TokenElement\n";
        }
        n = n.nextSibling();
    }
//	parse();
	kdWarning() << "Num of children " << list.count() << endl;
    return 1;
}


KFORMULA_NAMESPACE_END
