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


double ElementType::getSpaceBefore( const ContextStyle&,
                                    ContextStyle::TextStyle )
{
    return 0;
}

double ElementType::getSpaceAfter( MultiElementType*,
                                   const ContextStyle&,
                                   ContextStyle::TextStyle )
{
    return 0;
}

double ElementType::getSpaceAfter( OperatorType*,
                                   const ContextStyle&,
                                   ContextStyle::TextStyle )
{
    return 0;
}

double ElementType::getSpaceAfter( RelationType*,
                                   const ContextStyle&,
                                   ContextStyle::TextStyle )
{
    return 0;
}

double ElementType::getSpaceAfter( PunctuationType*,
                                   const ContextStyle&,
                                   ContextStyle::TextStyle )
{
    return 0;
}

double ElementType::getSpaceAfter( BracketType*,
                                   const ContextStyle&,
                                   ContextStyle::TextStyle )
{
    return 0;
}

double ElementType::getSpaceAfter( ComplexElementType*,
                                   const ContextStyle&,
                                   ContextStyle::TextStyle )
{
    return 0;
}

double ElementType::getSpaceAfter( InnerElementType*,
                                   const ContextStyle&,
                                   ContextStyle::TextStyle )
{
    return 0;
}

double ElementType::thinSpaceIfNotScript( const ContextStyle& context,
                                          ContextStyle::TextStyle tstyle )
{
    if ( !context.isScript( tstyle ) ) {
        return context.getThinSpace( tstyle );
    }
    return 0;
}

double ElementType::mediumSpaceIfNotScript( const ContextStyle& context,
                                            ContextStyle::TextStyle tstyle )
{
    if ( !context.isScript( tstyle ) ) {
        return context.getMediumSpace( tstyle );
    }
    return 0;
}

double ElementType::thickSpaceIfNotScript( const ContextStyle& context,
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

double MultiElementType::getSpaceBefore( const ContextStyle& context,
                                         ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

double MultiElementType::getSpaceAfter( OperatorType*,
                                        const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

double MultiElementType::getSpaceAfter( RelationType*,
                                        const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

double MultiElementType::getSpaceAfter( InnerElementType*,
                                        const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}


TextType::TextType( SequenceParser* parser )
    : MultiElementType( parser )
{
}


NameType::NameType( SequenceParser* parser, QString n )
    : MultiElementType( parser ), name( n )
{
}


bool NameType::isVisible( const TextElement& element ) const
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

double OperatorType::getSpaceBefore( const ContextStyle& context,
                                     ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

double OperatorType::getSpaceAfter( MultiElementType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

double OperatorType::getSpaceAfter( BracketType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

double OperatorType::getSpaceAfter( ComplexElementType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

double OperatorType::getSpaceAfter( InnerElementType*,
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

double RelationType::getSpaceBefore( const ContextStyle& context,
                                     ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

double RelationType::getSpaceAfter( MultiElementType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

double RelationType::getSpaceAfter( BracketType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

double RelationType::getSpaceAfter( ComplexElementType*,
                                    const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

double RelationType::getSpaceAfter( InnerElementType*,
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

double PunctuationType::getSpaceBefore( const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

double PunctuationType::getSpaceAfter( MultiElementType*,
                                       const ContextStyle& context,
                                       ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

double PunctuationType::getSpaceAfter( RelationType*,
                                       const ContextStyle& context,
                                       ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

double PunctuationType::getSpaceAfter( PunctuationType*,
                                       const ContextStyle& context,
                                       ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

double PunctuationType::getSpaceAfter( BracketType*,
                                       const ContextStyle& context,
                                       ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

double PunctuationType::getSpaceAfter( ComplexElementType*,
                                       const ContextStyle& context,
                                       ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

double PunctuationType::getSpaceAfter( InnerElementType*,
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

double BracketType::getSpaceBefore( const ContextStyle& context,
                                    ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

double BracketType::getSpaceAfter( OperatorType*,
                                   const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

double BracketType::getSpaceAfter( RelationType*,
                                   const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

double BracketType::getSpaceAfter( InnerElementType*,
                                   const ContextStyle& context,
                                   ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}


ComplexElementType::ComplexElementType( SequenceParser* parser )
    : SingleElementType( parser )
{
}

double ComplexElementType::getSpaceBefore( const ContextStyle& context,
                                           ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

double ComplexElementType::getSpaceAfter( OperatorType*,
                                          const ContextStyle& context,
                                          ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

double ComplexElementType::getSpaceAfter( RelationType*,
                                          const ContextStyle& context,
                                          ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

double ComplexElementType::getSpaceAfter( InnerElementType*,
                                          const ContextStyle& context,
                                          ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}


InnerElementType::InnerElementType( SequenceParser* parser )
    : SingleElementType( parser )
{
}

double InnerElementType::getSpaceBefore( const ContextStyle& context,
                                         ContextStyle::TextStyle tstyle )
{
    if ( getPrev() != 0 ) {
        return getPrev()->getSpaceAfter( this, context, tstyle );
    }
    return 0;
}

double InnerElementType::getSpaceAfter( MultiElementType*,
                                        const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

double InnerElementType::getSpaceAfter( OperatorType*,
                                        const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    return mediumSpaceIfNotScript( context, tstyle );
}

double InnerElementType::getSpaceAfter( RelationType*,
                                        const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    return thickSpaceIfNotScript( context, tstyle );
}

double InnerElementType::getSpaceAfter( PunctuationType*,
                                        const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

double InnerElementType::getSpaceAfter( BracketType*,
                                        const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

double InnerElementType::getSpaceAfter( ComplexElementType*,
                                        const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}

double InnerElementType::getSpaceAfter( InnerElementType*,
                                        const ContextStyle& context,
                                        ContextStyle::TextStyle tstyle )
{
    return thinSpaceIfNotScript( context, tstyle );
}


KFORMULA_NAMESPACE_END
