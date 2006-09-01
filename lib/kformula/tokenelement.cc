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

#include <klocale.h>

#include "entities.h"
#include "elementtype.h"
#include "sequenceelement.h"
#include "textelement.h"
#include "glyphelement.h"
#include "tokenelement.h"
#include "kformulacommand.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "formulaelement.h"
#include "creationstrategy.h"

KFORMULA_NAMESPACE_BEGIN

TokenElement::TokenElement( BasicElement* parent ) : TokenStyleElement( parent ),
                                                     m_textOnly( true )
{
}

TokenElement::TokenElement( QChar ch, BasicElement* parent ) : TokenStyleElement( parent ),
                                                               m_textOnly( true )
{
    TextElement* child = new TextElement( ch );
    child->setParent( this );
    insert( 0, child );
}

/*
 * Token elements' content has to be of homogeneous type. Every token element
 * must (TODO: check this) appear inside a non-token sequence, and thus, if
 * the command asks for a different content, a new element has to be created in
 * parent sequence.
 */
KCommand* TokenElement::buildCommand( Container* container, Request* request )
{
    FormulaCursor* cursor = container->activeCursor();
    if ( cursor->isReadOnly() ) {
        formula()->tell( i18n( "write protection" ) );
        return 0;
    }

    switch ( *request ) {
    case req_addNumber: {
        KFCReplace* command = new KFCReplace( i18n("Add Number"), container );
        NumberRequest* nr = static_cast<NumberRequest*>( request );
        TextElement* element = creationStrategy->createTextElement( nr->ch(), false );
        command->addElement( element );
        return command;
    }

    case req_addText:
    case req_addTextChar:
    case req_makeGreek: 
    case req_addOperator:
    case req_addEmptyBox:
    case req_addNameSequence:
    case req_addBracket:
    case req_addSpace:
    case req_addFraction:
    case req_addRoot:
    case req_addSymbol:
    case req_addOneByTwoMatrix:
    case req_addMatrix:
        return getParent()->buildCommand( container, request );

    default:
        return SequenceElement::buildCommand( container, request );
    }
    return 0;
}

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
            const entityMap* end = entities + entityMap::size();
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
            m_textOnly = false;
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

luPt TokenElement::getSpaceBefore( const ContextStyle& context, 
                                   ContextStyle::TextStyle tstyle,
                                   double factor )
{
    if ( !context.isScript( tstyle ) ) {
        return context.getMediumSpace( tstyle, factor );
    }
    return 0;
}

luPt TokenElement::getSpaceAfter( const ContextStyle& context, 
                                  ContextStyle::TextStyle tstyle,
                                  double factor )
{
    if ( !context.isScript( tstyle ) ) {
        return context.getThinSpace( tstyle, factor );
    }
    return 0;
}

KFORMULA_NAMESPACE_END
