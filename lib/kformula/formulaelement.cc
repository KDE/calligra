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

#include <iostream>
#include <qpainter.h>

#include "contextstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"

KFORMULA_NAMESPACE_BEGIN

FormulaElement::FormulaElement(FormulaDocument* container)
    : document( container ), baseSize( 18 )
{
}

/**
 * Returns the element the point is in.
 */
BasicElement* FormulaElement::goToPos(FormulaCursor* cursor, const KoPoint& point)
{
    bool handled = false;
    BasicElement* element = inherited::goToPos(cursor, handled, point, KoPoint());
    if (element == 0) {
        //if ((point.x() > getWidth()) || (point.y() > getHeight())) {
            cursor->setTo(this, countChildren());
            //}
            return this;
    }
    return element;
}

void FormulaElement::elementRemoval(BasicElement* child)
{
    document->elementRemoval(child);
}

void FormulaElement::changed()
{
    document->changed();
}


/**
 * Calculates the formulas sizes and positions.
 */
void FormulaElement::calcSizes( ContextStyle& context )
{
    context.setUnzoomedBaseSize( getBaseSize() );
    inherited::calcSizes( context, context.getBaseTextStyle(),
                          ContextStyle::normal );
}

/**
 * Draws the whole thing.
 */
void FormulaElement::draw( QPainter& painter, const QRect& r,
                           ContextStyle& context )
{
    context.setUnzoomedBaseSize( getBaseSize() );
    inherited::draw( painter, r, context, context.getBaseTextStyle(),
                     ContextStyle::normal, KoPoint() );
}


const SymbolTable& FormulaElement::getSymbolTable() const
{
    return document->getSymbolTable();
}


/**
 * Appends our attributes to the dom element.
 */
void FormulaElement::writeDom(QDomElement& element)
{
    inherited::writeDom(element);
}

/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool FormulaElement::readAttributesFromDom(QDomElement& element)
{
    if (!inherited::readAttributesFromDom(element)) {
        return false;
    }
    return true;
}

/**
 * Reads our content from the node. Sets the node to the next node
 * that needs to be read.
 * Returns false if it failed.
 */
bool FormulaElement::readContentFromDom(QDomNode& node)
{
    return inherited::readContentFromDom(node);
}

QString FormulaElement::toLatex()
{
    return inherited::toLatex();   //Consider $$ sorround
}

KFORMULA_NAMESPACE_END
