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

// Debug
#include <iostream>

#include <qfontmetrics.h>
#include <qpainter.h>

#include "basicelement.h"
#include "textelement.h"
#include "contextstyle.h"


TextElement::TextElement(QChar ch, BasicElement* parent)
    : BasicElement(parent), character(ch)
{
}


ostream& TextElement::output(ostream& stream)
{
    stream << "TextElement: " << character << ", ";
    BasicElement::output(stream);
    return stream;
}

/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void TextElement::calcSizes(ContextStyle& context, int parentSize)
{
    int mySize = parentSize + getRelativeSize();
    //QFontMetrics fm = context.fontMetrics();
    QFont font = context.getDefaultFont();
    font.setPointSize(mySize);
    QFontMetrics fm(font);
    setWidth(fm.width(character));
    setHeight(fm.height());
    setMidline(getHeight() / 2);
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
    int mySize = parentSize + getRelativeSize();
    //context.setupPainter(painter);
    //cerr << "TextElement::draw: " << parentOrigin.x()+getX() << " " << parentOrigin.y()+getY()+baseline << " " << character << "\n";
    QFont font = context.getDefaultFont();
    font.setPointSize(mySize);
    painter.setFont(font);
    painter.setPen(context.getDefaultColor());
    painter.drawText(parentOrigin.x()+getX(),
                     parentOrigin.y()+getY()+baseline, character);
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
void TextElement::moveLeft(FormulaCursor* cursor, BasicElement*)
{
    getParent()->moveLeft(cursor, this);
}

/**
 * Enters this element while moving to the right starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the right of it.
 */
void TextElement::moveRight(FormulaCursor* cursor, BasicElement*)
{
    getParent()->moveRight(cursor, this);
}


QDomElement TextElement::getElementDom(QDomDocument *doc)
{
    
    QDomElement de=doc->createElement("TEXT");
    int sz=getRelativeSize();
    if(sz!=0) {
        de.setAttribute("SIZE",sz);
    }
    //May be this is wrong
    de.setAttribute("CHAR",QString(character));
    return de;
}


