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


#ifndef OASISCREATIONSTRATEGY_H
#define OASISCREATIONSTRATEGY_H


#include <qstring.h>

#include "kformuladefs.h"
#include "creationstrategy.h"

KFORMULA_NAMESPACE_BEGIN

class BasicElement;
class BracketElement;
class EmptyElement;
class FractionElement;
class IndexElement;
class MatrixElement;
class MultilineElement;
class NameSequence;
class OverlineElement;
class RootElement;
class SpaceElement;
class SymbolElement;
class TextElement;
class UnderlineElement;

/**
 * The strategy to be used for OASIS OpenDocument / MathML.
 */
class OasisCreationStrategy : public ElementCreationStrategy {
public:
    virtual BasicElement* createElement( QString type );

    virtual TextElement* createTextElement( const QChar& ch, bool symbol=false );
    virtual EmptyElement* createEmptyElement();
    virtual NameSequence* createNameSequence();
    virtual BracketElement* createBracketElement( SymbolType lhs, SymbolType rhs );
    virtual OverlineElement* createOverlineElement();
    virtual UnderlineElement* createUnderlineElement();
    virtual MultilineElement* createMultilineElement();
    virtual SpaceElement* createSpaceElement( SpaceWidth width );
    virtual FractionElement* createFractionElement();
    virtual RootElement* createRootElement();
    virtual SymbolElement* createSymbolElement( SymbolType type );
    virtual MatrixElement* createMatrixElement( uint rows, uint columns );
    virtual IndexElement* createIndexElement();
	virtual QString type() const { return "Oasis"; }
};

KFORMULA_NAMESPACE_END

#endif // OASISCREATIONSTRATEGY_H
