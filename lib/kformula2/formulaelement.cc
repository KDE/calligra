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

#include <qpainter.h>

#include "contextstyle.h"
#include "formulaelement.h"
#include "kformulacontainer.h"


FormulaElement::FormulaElement(KFormulaContainer* container)
    : document(container)
{
    size = 20;
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
void FormulaElement::calcSizes(ContextStyle& context)
{
    SequenceElement::calcSizes(context, size);
}

/**
 * Draws the whole thing.
 */
void FormulaElement::draw(QPainter& painter, ContextStyle& context)
{
    SequenceElement::draw(painter, context, size, QPoint());
}
