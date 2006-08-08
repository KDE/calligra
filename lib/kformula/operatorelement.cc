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
#include "fontstyle.h"
#include "operatorelement.h"

KFORMULA_NAMESPACE_BEGIN

OperatorElement::OperatorElement( BasicElement* parent ) : TokenElement( parent ) {}

int OperatorElement::buildChildrenFromMathMLDom(QPtrList<BasicElement>& list, QDomNode n) {
    QString textelements;
    if ( n.isEntityReference() ) {
        QString entity = n.toEntityReference().nodeName();
        if ( entity == "ApplyFunction" ) {
            textelements = applyFunctionChar;
        }
        else if ( entity == "InvisibleTimes" ) {
            textelements = invisibleTimes;
        }
        else if ( entity == "InvisibleComma" ) {
            textelements = invisibleComma;
        }
        else if ( entity == "CapitalDifferentialD" ) {
            textelements = QChar(0x2145);
        }
        else if ( entity == "DifferentialD" ) {
            textelements = QChar(0x2146);
        }
        else if ( entity == "ExponentialE" ) {
            textelements = QChar(0x2147);
        }
        else if ( entity == "ImaginaryI" ) {
            textelements = QChar(0x2148);
        }
        else if ( entity.lower() == "sum" ) {
            textelements = QChar(0x2211);
        }
        else if ( entity == "int" || entity == "Integral" ) {
            textelements = QChar(0x222B);
        }
        else if ( entity == "Int" ) {
            textelements = QChar(0x222C);
        }
        else if ( entity == "prod" || entity == "Product" ) {
            textelements = QChar(0x0220F);
        }
        else {
            kdWarning() << "Unknown entity " << entity << endl;
            return -1;
        }
    }
    else if ( n.isText() ) {
        textelements = n.toText().data().stripWhiteSpace();
    }
    else {
        return -1;
    }
    
    for (uint i = 0; i < textelements.length(); i++) {
        TextElement* child = new TextElement(textelements[i], true);
        if (child != 0) {
            child->setParent(this);
            child->setCharFamily( charFamily() );
            child->setCharStyle( charStyle() );
			list.append(child);
        }
    }
	parse();
	kdWarning() << "Num of children " << list.count() << endl;
    return 1;
}

KFORMULA_NAMESPACE_END
