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
void TextElement::calcSizes(const ContextStyle& context, int parentSize)
{
    int mySize = QMAX(parentSize, context.getMinimumSize());

    QFont font = getFont(context);
    font.setPointSize(mySize);
    int spaceBefore = getSpaceBefore(context, mySize);
    int spaceAfter = getSpaceAfter(context, mySize);

    QFontMetrics fm(font);
    QRect bound = fm.boundingRect(character);

    setWidth(fm.width(character) + spaceBefore + spaceAfter);
    setHeight(bound.height());
    setBaseline(-bound.top());
    setMidline(getBaseline() - fm.strikeOutPos());
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void TextElement::draw(QPainter& painter, const ContextStyle& context,
                       int parentSize, const QPoint& parentOrigin)
{
    int mySize = QMAX(parentSize, context.getMinimumSize());

    QFont font = getFont(context);
    font.setPointSize(mySize);
    setUpPainter(context, painter);
    int spaceBefore = getSpaceBefore(context, mySize);
    //int spaceAfter = getSpaceAfter(context, mySize);
    
    painter.setFont(font);
    painter.drawText(parentOrigin.x()+getX()+spaceBefore,
                     parentOrigin.y()+getY()+getBaseline(), character);
}


QFont TextElement::getFont(const ContextStyle& context)
{
    if (getElementType() != 0) {
        return getElementType()->getFont(context);
    }
    else {
        return context.getDefaultFont();
    }
}

int TextElement::getSpaceBefore(const ContextStyle& context, int size)
{
    if (getElementType() != 0) {
        return getElementType()->getSpaceBefore(context, size);
    }
    else {
        return 0;
    }
}

int TextElement::getSpaceAfter(const ContextStyle& context, int size)
{
    if (getElementType() != 0) {
        return getElementType()->getSpaceAfter(context, size);
    }
    else {
        return 0;
    }
}

void TextElement::setUpPainter(const ContextStyle& context, QPainter& painter)
{
    if (getElementType() != 0) {
        getElementType()->setUpPainter(context, painter);
    }
    else {
        painter.setPen(Qt::red);
    }
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
