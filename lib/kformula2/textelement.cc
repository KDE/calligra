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

#include "basicelement.h"
#include "textelement.h"


TextElement::TextElement(SequenceElement* parent, QChar ch)
    : BasicElement(parent)
{
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void TextElement::calcSizes(ContextStyle& context, int parentSize)
{
    QFontMetrics fm = context.fontMetrics();
    setWidth(fm.width(ch));
    setHeight(fm.height());
    setMidline(height() / 2);
    baseline = fm.ascent();
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void TextElement::draw(QPainter& painter, ContextStyle& context,
                       int parentSize, const QPoint& parentOrigin)
{
    context.setupPainter(painter);
    painter.drawText(parentOrigin.x()+getX(), parentOrigin.y()+getY()+baseline, ch);
}

    
// navigation
// 
// The elements are responsible to handle cursor movement themselves.
// To do this they need to know the direction the cursor moves and
// the element it comes from.
//
// The cursor might be in normal or in selection mode.

/**
 * Enters this element while moving to the left starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the left of it.
 */
void TextElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    getParent()->moveLeft(cursor, this);
}

/**
 * Enters this element while moving to the right starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the right of it.
 */
void TextElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    getParent()->moveRight(cursor, this);
}
