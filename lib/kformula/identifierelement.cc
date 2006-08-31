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

#include <klocale.h>

#include "kformuladefs.h"
#include "textelement.h"
#include "identifierelement.h"
#include "kformulacommand.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "formulaelement.h"
#include "creationstrategy.h"

KFORMULA_NAMESPACE_BEGIN

IdentifierElement::IdentifierElement( QChar ch, BasicElement* parent ) : TokenElement( ch, parent ) {
}

/*
 * Token elements' content has to be of homogeneous type. Every token element
 * must (TODO: check this) appear inside a non-token sequence, and thus, if
 * the command asks for a different content, a new element has to be created in
 * parent sequence.
 */
KCommand* IdentifierElement::buildCommand( Container* container, Request* request )
{
    FormulaCursor* cursor = container->activeCursor();
    if ( cursor->isReadOnly() ) {
        formula()->tell( i18n( "write protection" ) );
        return 0;
    }

    switch ( *request ) {
    case req_addText: {
        KFCReplace* command = new KFCReplace( i18n("Add Text"), container );
        TextRequest* tr = static_cast<TextRequest*>( request );
        for ( uint i = 0; i < tr->text().length(); i++ ) {
            command->addElement( creationStrategy->createTextElement( tr->text()[i] ) );
        }
        return command;
    }
    case req_addTextChar: {
        KFCReplace* command = new KFCReplace( i18n("Add Text"), container );
        TextCharRequest* tr = static_cast<TextCharRequest*>( request );
        TextElement* element = creationStrategy->createTextElement( tr->ch(), tr->isSymbol() );
        command->addElement( element );
        return command;
    }

    case req_addOperator:
    case req_addNumber:
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

void IdentifierElement::setStyleVariant( StyleAttributes& style )
{
    if ( customMathVariant() ) {
        style.setCustomMathVariant ( true );
        style.setCustomFontWeight( false );
        style.setCustomFontStyle( false );
        style.setCustomFont( false );
        if ( customMathVariant() ) {
            style.setCharFamily ( charFamily() );
            style.setCharStyle( charStyle() );
        }
        else {
            style.setCharFamily( style.charFamily() );
            style.setCharStyle( style.charStyle() );
        }
    }
    else {
        style.setCustomMathVariant( false );
        if ( customFontFamily() ) {
            style.setCustomFont( true );
            style.setFont( QFont(fontFamily()) );
        }

        bool fontweight = false;
        if ( customFontWeight() || style.customFontWeight() ) {
            style.setCustomFontWeight( true );
            if ( customFontWeight() ) {
                fontweight = fontWeight();
            }
            else {
                fontweight = style.customFontWeight();
            }
            style.setFontWeight( fontweight );
        }
        else {
            style.setCustomFontWeight( false );
        }

        bool fontstyle;
        if ( customFontStyle() ) {
            fontstyle = fontStyle();
        }
        else if ( countChildren() == 1 ) {
            fontstyle = true;
        }
        else {
            fontstyle = false;
        }

        if ( fontweight && fontstyle ) {
            style.setCharStyle( boldItalicChar );
        }
        else if ( fontweight && ! fontstyle ) {
            style.setCharStyle( boldChar );
        }
        else if ( ! fontweight && fontstyle ) {
            style.setCharStyle( italicChar );
        }
        else {
            style.setCharStyle( normalChar );
        }
    }
}

KFORMULA_NAMESPACE_END
