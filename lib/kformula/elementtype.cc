/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <iostream.h>

#include <qfont.h>
#include <qfontmetrics.h>
#include <qpainter.h>

#include <kdebug.h>

#include "basicelement.h"
#include "contextstyle.h"
#include "elementtype.h"
#include "sequenceelement.h"
#include "sequenceparser.h"
#include "textelement.h"


KFORMULA_NAMESPACE_BEGIN

int ElementType::evilDestructionCount = 0;


ElementType::ElementType( SequenceParser* parser )
    : from( parser->getStart() ), to( parser->getEnd() ), prev( 0 )
{
    evilDestructionCount++;
}

ElementType::~ElementType()
{
    delete prev;
    evilDestructionCount--;
}


lu ElementType::getSpaceBefore( const ContextStyle&,
                                ContextStyle::TextStyle )
{
    return 0;
}

lu ElementType::getSpaceAfter( MultiElementType*,
                               const ContextStyle&,
                               ContextStyle::TextStyle )
{
    return 0;
}

lu ElementType::getSpaceAfter( OperatorType*,
                               const ContextStyle&,
                               ContextStyle::TextStyle )
{
    return 0;
}

lu ElementType::getSpaceAfter( RelationType*,
                               const ContextStyle&,
                               ContextStyle::TextStyle )
{
    return 0;
}

lu ElementType::getSpaceAfter( PunctuationType*,
                               const ContextStyle&,
                               ContextStyle::TextStyle )
{
    return 0;
}

lu ElementType::getSpaceAfter( BracketType*,
                               const ContextStyle&,
                               ContextStyle::TextStyle )
{
    return 0;
}

lu ElementType::getSpaceAfter( ComplexElementType*,
                               const ContextStyle&,
                               ContextStyle::TextStyle )
{
    return 0;
}

lu ElementType::getSpaceAfter( InnerElementType*,
                               const ContextStyle&,
                               ContextStyle::TextStyle )
{
    return 0;
}

lu ElementType::thinSpaceIfNotScript( const ContextStyle& context,
                                      ContextStyle::TextStyle tstyle )
{
    if ( !context.isScript( tstyle ) ) {
        return context.getThinSpace( tstyle );
    }
    return 0;
}

lu ElementType::mediumSpaceIfNotScript( const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    if ( !context.isScript( tstyle ) ) {
        return context.getMediumSpace( tstyle );
    }
    return 0;
}

lu ElementType::thickSpaceIfNotScript( const ContextStyle& context,
                                       ContextStyle::TextStyle tstyle )
{
    if ( !context.isScript( tstyle ) ) {
        return context.getThickSpace( tstyle );
    }
    return 0;
}


QFont ElementType::getFont(const ContextStyle& context)
{
    return context.getDefaultFont();
}

void ElementType::setUpPainter(const ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getDefaultColor());
}

void ElementType::append( ElementType* element )
{
    element->prev = this;
}

void ElementType::output()
{
    cerr << start() << " - " << end() << endl;
}


SequenceType::SequenceType( SequenceParser* parser )
    : ElementType( parser ), last( 0 )
{
    while ( true ) {
        parser->nextToken();
        //cerr << "SequenceType::SequenceType(): " << parser->getTokenType() << " "
        //     << parser->getStart() << " " << parser->getEnd() << endl;
        if ( parser->getTokenType() == END ) {
            break;
        }
        ElementType* nextType = parser->getPrimitive();
        if ( nextType == 0 ) {
            break;
        }
        if ( last != 0 ) {
            last->append( nextType );
        }
        last = nextType;
    }
}

SequenceType::~SequenceType()
{
    delete last;
}


void SequenceType::output()
{
}


MultiElementType::MultiElementType( SequenceParser* parser )
    : ElementType( parser )
{
    for ( uint i = start(); i < end(); i++ ) {
        parser->setElementType( i, this );
    }
}

lu MultiElementType::getSpaceBefore( const ContextStyle& context,
                                     ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

lu MultiElementType::getSpaceAfter( OperatorType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

lu MultiElementType::getSpaceAfter( RelationType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

lu MultiElementType::getSpaceAfter( InnerElementType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}


TextType::TextType( SequenceParser* parser )
    : MultiElementType( parser )
{
}


NameType::NameType( SequenceParser* parser )
    : MultiElementType( parser )
{
}


bool NameType::isInvisible( const TextElement& element ) const
{
    // Only this first char of a name might be a backslash.
    return element.getCharacter() == QChar( '\\' );
}


QFont NameType::getFont(const ContextStyle& context)
{
    return context.getNameFont();
}

NumberType::NumberType( SequenceParser* parser )
    : MultiElementType( parser )
{
}

QFont NumberType::getFont(const ContextStyle& context)
{
    return context.getNumberFont();
}

void NumberType::setUpPainter(const ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getNumberColor());
}


SingleElementType::SingleElementType( SequenceParser* parser )
    : ElementType( parser )
{
    parser->setElementType( start(), this );
}

OperatorType::OperatorType( SequenceParser* parser )
    : SingleElementType( parser )
{
}

lu OperatorType::getSpaceBefore( const ContextStyle& context,
                                 ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

lu OperatorType::getSpaceAfter( MultiElementType*,
                                const ContextStyle& context,
                                ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

lu OperatorType::getSpaceAfter( BracketType*,
                                const ContextStyle& context,
                                ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

lu OperatorType::getSpaceAfter( ComplexElementType*,
                                const ContextStyle& context,
                                ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

lu OperatorType::getSpaceAfter( InnerElementType*,
                                const ContextStyle& context,
                                ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}


QFont OperatorType::getFont(const ContextStyle& context)
{
    return context.getOperatorFont();
}

void OperatorType::setUpPainter(const ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getOperatorColor());
}


RelationType::RelationType( SequenceParser* parser )
    : SingleElementType( parser )
{
}

lu RelationType::getSpaceBefore( const ContextStyle& context,
                                 ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

lu RelationType::getSpaceAfter( MultiElementType*,
                                const ContextStyle& context,
                                ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

lu RelationType::getSpaceAfter( BracketType*,
                                const ContextStyle& context,
                                ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

lu RelationType::getSpaceAfter( ComplexElementType*,
                                const ContextStyle& context,
                                ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

lu RelationType::getSpaceAfter( InnerElementType*,
                                const ContextStyle& context,
                                ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

QFont RelationType::getFont( const ContextStyle& context )
{
    return context.getOperatorFont();
}

void RelationType::setUpPainter( const ContextStyle& context, QPainter& painter )
{
    painter.setPen(context.getOperatorColor());
}



PunctuationType::PunctuationType( SequenceParser* parser )
    : SingleElementType( parser )
{
}

lu PunctuationType::getSpaceBefore( const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

lu PunctuationType::getSpaceAfter( MultiElementType*,
                                   const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

lu PunctuationType::getSpaceAfter( RelationType*,
                                   const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

lu PunctuationType::getSpaceAfter( PunctuationType*,
                                   const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

lu PunctuationType::getSpaceAfter( BracketType*,
                                   const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

lu PunctuationType::getSpaceAfter( ComplexElementType*,
                                   const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

lu PunctuationType::getSpaceAfter( InnerElementType*,
                                   const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

QFont PunctuationType::getFont( const ContextStyle& context )
{
    return context.getOperatorFont();
}

void PunctuationType::setUpPainter( const ContextStyle& context, QPainter& painter )
{
    painter.setPen( context.getDefaultColor() );
}


BracketType::BracketType( SequenceParser* parser )
    : SingleElementType( parser )
{
}

lu BracketType::getSpaceBefore( const ContextStyle& context,
                                ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

lu BracketType::getSpaceAfter( OperatorType*,
                               const ContextStyle& context,
                               ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

lu BracketType::getSpaceAfter( RelationType*,
                               const ContextStyle& context,
                               ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

lu BracketType::getSpaceAfter( InnerElementType*,
                               const ContextStyle& context,
                               ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}


ComplexElementType::ComplexElementType( SequenceParser* parser )
    : SingleElementType( parser )
{
}

lu ComplexElementType::getSpaceBefore( const ContextStyle& context,
                                       ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

lu ComplexElementType::getSpaceAfter( OperatorType*,
                                      const ContextStyle& context,
                                      ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

lu ComplexElementType::getSpaceAfter( RelationType*,
                                      const ContextStyle& context,
                                      ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

lu ComplexElementType::getSpaceAfter( InnerElementType*,
                                      const ContextStyle& context,
                                      ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}


InnerElementType::InnerElementType( SequenceParser* parser )
    : SingleElementType( parser )
{
}

lu InnerElementType::getSpaceBefore( const ContextStyle& context,
                                     ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

lu InnerElementType::getSpaceAfter( MultiElementType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

lu InnerElementType::getSpaceAfter( OperatorType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

lu InnerElementType::getSpaceAfter( RelationType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

lu InnerElementType::getSpaceAfter( PunctuationType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

lu InnerElementType::getSpaceAfter( BracketType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

lu InnerElementType::getSpaceAfter( ComplexElementType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

lu InnerElementType::getSpaceAfter( InnerElementType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}


KFORMULA_NAMESPACE_END
