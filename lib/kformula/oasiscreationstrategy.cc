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

#include <qdom.h>

#include "bracketelement.h"
#include "elementtype.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "matrixelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "spaceelement.h"
#include "symbolelement.h"
#include "textelement.h"
#include "glyphelement.h"
#include "styleelement.h"
#include "tokenelement.h"
#include "identifierelement.h"
#include "operatorelement.h"
#include "stringelement.h"
#include "paddedelement.h"
#include "errorelement.h"
#include "phantomelement.h"
#include "actionelement.h"
#include "encloseelement.h"

#include "oasiscreationstrategy.h"

KFORMULA_NAMESPACE_BEGIN

BasicElement* OasisCreationStrategy::createElement( QString type, const QDomElement& element )
{
    
    // TODO
    // mlabeledtr
    // maligngroup
    // malignmark
    // Content elements
    // mtr and mtd are currently managed inside MatrixElement
    kdDebug( DEBUGID ) << type << endl;

    // Token Elements ( Section 3.1.6.1 )
    if      ( type == "mi" )               return new IdentifierElement();
    else if ( type == "mo" )               return createOperatorElement( element );
    else if ( type == "mn"
              || type == "mtext" )         return new TokenElement();
    else if ( type == "ms" )               return new StringElement();
    else if ( type == "mspace" )           return new SpaceElement();
    else if ( type == "mglyph" )           return new GlyphElement();

    // General Layout Schemata ( Section 3.1.6.2 )
    else if ( type == "mrow" )             return new SequenceElement();
    else if ( type == "mfrac" )            return new FractionElement();
    else if ( type == "msqrt"
              || type == "mroot" )         return new RootElement();
	else if ( type == "mstyle" )           return new StyleElement();
    else if ( type == "merror" )           return new ErrorElement();
    else if ( type == "mpadded" )          return new PaddedElement();
    else if ( type == "mphantom" )         return new PhantomElement();
    else if ( type == "mfenced" )          return new BracketElement();
    else if ( type == "menclose" )        return new EncloseElement();

    // Script and Limit Schemata ( Section 3.1.6.3 )
    else if ( type == "msub"
              || type == "msup"
              || type == "msubsup"
              || type == "munder"
              || type == "mover"
              || type == "munderover"
              || type == "mmultiscripts" ) return new IndexElement();

    // Tables and Matrices ( Section 3.1.6.4 )
    else if ( type == "mtable" )           return new MatrixElement();

    // Enlivening Expressions ( Section 3.1.6.5 )
    else if ( type == "maction" )          return new ActionElement();
    return 0;
}


TextElement* OasisCreationStrategy::createTextElement( const QChar& ch, bool symbol )
{
    return new TextElement( ch, symbol );
}

EmptyElement* OasisCreationStrategy::createEmptyElement()
{
    return new EmptyElement;
}

NameSequence* OasisCreationStrategy::createNameSequence()
{
    return new NameSequence;
}

BracketElement* OasisCreationStrategy::createBracketElement( SymbolType lhs, SymbolType rhs )
{
    return new BracketElement( lhs, rhs );
}

OverlineElement* OasisCreationStrategy::createOverlineElement()
{
    return new OverlineElement;
}

UnderlineElement* OasisCreationStrategy::createUnderlineElement()
{
    return new UnderlineElement;
}

MultilineElement* OasisCreationStrategy::createMultilineElement()
{
    return new MultilineElement;
}

SpaceElement* OasisCreationStrategy::createSpaceElement( SpaceWidth width )
{
    return new SpaceElement( width );
}

FractionElement* OasisCreationStrategy::createFractionElement()
{
    return new FractionElement;
}

RootElement* OasisCreationStrategy::createRootElement()
{
    return new RootElement;
}

SymbolElement* OasisCreationStrategy::createSymbolElement( SymbolType type )
{
    return new SymbolElement( type );
}

MatrixElement* OasisCreationStrategy::createMatrixElement( uint rows, uint columns )
{
    return new MatrixElement( rows, columns );
}

IndexElement* OasisCreationStrategy::createIndexElement()
{
    return new IndexElement;
}

BasicElement* OasisCreationStrategy::createOperatorElement( const QDomElement& element )
{
    QDomNode n = element.firstChild();
    if ( n.isNull() )
        return 0;
    if ( n.isEntityReference() ) {
        QString name = n.nodeName();
        if ( name == "CloseCurlyDoubleQuote"
             || name == "CloseCurlyQuote"
             || name == "LeftAngleBracket"
             || name == "LeftCeiling"
             || name == "LeftDoubleBracket"
             || name == "LeftFloor"
             || name == "OpenCurlyDoubleQuote"
             || name == "OpenCurlyQuote"
             || name == "RightAngleBracket"
             || name == "RightCeiling"
             || name == "RightDoubleBracket"
             || name == "RightFloor" ) {
            return new BracketElement();
        }
        return new OperatorElement();
    }
    if ( n.isText() ) {
        QString text = n.toText().data();
        if ( text.length() == 1 && QString("()[]{}").contains(text[0]) ) {
            return new BracketElement();
        }
    }
    return new OperatorElement();
}

IdentifierElement* OasisCreationStrategy::createIdentifierElement()
{
    return new IdentifierElement();
}

OperatorElement* OasisCreationStrategy::createOperatorElement()
{
    return new OperatorElement();
}

TokenElement* OasisCreationStrategy::createNumberElement()
{
    return new TokenElement();
}

KFORMULA_NAMESPACE_END
