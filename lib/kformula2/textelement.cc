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
#include "elementtype.h"
#include "textelement.h"
#include "contextstyle.h"


TextElement::TextElement(QChar ch, BasicElement* parent)
    : BasicElement(parent), character(ch)
{
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void TextElement::calcSizes(ContextStyle& context, int parentSize)
{
    int mySize = QMAX(parentSize + getRelativeSize(), 8);
    QFont font;
    
    ElementType* type = getElementType();
    if (type != 0) {
        spaceWidth = type->getSpace(context, mySize);
        font = type->getFont(context);
    }
    else {
        spaceWidth = 0;
        font = context.getDefaultFont();
    }
    font.setPointSize(mySize);

    QFontMetrics fm(font);
    QRect bound=fm.boundingRect(character);
    bound.moveBy(0,fm.strikeOutPos());

    setWidth(fm.width(character) + spaceWidth*2);
    setHeight(bound.height());

    baseline = -bound.top()+fm.strikeOutPos();
    setMidline(-bound.top());
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void TextElement::draw(QPainter& painter, ContextStyle& context,
                       int parentSize, const QPoint& parentOrigin)
{
    int mySize = QMAX(parentSize + getRelativeSize(), 10);
    QFont font;
    
    ElementType* type = getElementType();
    if (type != 0) {
        font = type->getFont(context);
        type->setUpPainter(context, painter);
    }
    else {
        font = context.getDefaultFont();
        //painter.setPen(context.getDefaultColor());
        painter.setPen(Qt::red);
    }
    font.setPointSize(mySize);
    painter.setFont(font);
    painter.drawText(parentOrigin.x()+getX()+spaceWidth,
                     parentOrigin.y()+getY()+baseline, character);
}


QFont TextElement::getFont(ContextStyle& context)
{
    return context.getDefaultFont();
}


/**
 * Appends our attributes to the dom element.
 */
void TextElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);
    element.setAttribute("CHAR", QString(character));
}
    
/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool TextElement::readAttributesFromDom(QDomElement& element)
{
    if (!BasicElement::readAttributesFromDom(element)) {
        return false;
    }
    QString charStr = element.attribute("CHAR");
    if(!charStr.isNull()) {
        character = charStr.at(0);
    }
    return true;
}

/**
 * Reads our content from the node. Sets the node to the next node
 * that needs to be read.
 * Returns false if it failed.
 */
bool TextElement::readContentFromDom(QDomNode& node)
{
    return BasicElement::readContentFromDom(node);
}
