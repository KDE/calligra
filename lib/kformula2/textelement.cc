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
#include "contextstyle.h"
#include "elementtype.h"
#include "formulaelement.h"
#include "sequenceelement.h"
#include "textelement.h"


TextElement::TextElement(QChar ch, BasicElement* parent)
        : BasicElement(parent), character(ch), symbol(false)
{
}


TokenType TextElement::getTokenType() const
{
    if (isSymbol())
        return SYMBOL;

    char latin1 = character.latin1();
    switch (latin1) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
        case '<':
        case '>':
        case '\\':
        case ',':
        case ';':
        case ':':
            return TokenType(latin1);
        case '\0':
            return ELEMENT;
        default:
            if (character.isNumber()) {
                return NUMBER;
            }
            else {
                return TEXT;
            }
    }
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void TextElement::calcSizes(const ContextStyle& context, ContextStyle::TextStyle tstyle, ContextStyle::IndexStyle /*istyle*/)
{
    int mySize = context.getAdjustedSize( tstyle );

    QFont font = getFont(context);
    font.setPointSize(mySize);
    int spaceBefore = getSpaceBefore(context, tstyle);
    int spaceAfter = getSpaceAfter(context, tstyle);

    QFontMetrics fm(font);
    QRect bound = fm.boundingRect(character);

    setWidth(fm.width(character) + spaceBefore + spaceAfter);

    setHeight(bound.height());
    setBaseline(-bound.top());

    // Or this way. But this doesn't seem to look better...
    //setHeight(fm.height());
    //setBaseline(fm.ascent()+1);

    setMidline(getBaseline() - fm.strikeOutPos());
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void TextElement::draw(QPainter& painter, const QRect& r,
                       const ContextStyle& context,
		       ContextStyle::TextStyle tstyle,
		       ContextStyle::IndexStyle /*istyle*/,
		       const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    int mySize = context.getAdjustedSize( tstyle );
    if (!QRect(myPos, getSize()).intersects(r))
        return;

    QFont font = getFont(context);
    font.setPointSize(mySize);
    setUpPainter(context, painter);
    int spaceBefore = getSpaceBefore(context, tstyle);
    //int spaceAfter = getSpaceAfter(context, tstyle);

    painter.setFont(font);
    painter.drawText(parentOrigin.x()+getX()+spaceBefore,
                     parentOrigin.y()+getY()+getBaseline(), character);

    // Debug
    //painter.setBrush(Qt::NoBrush);
    //painter.setPen(Qt::red);
    //painter.drawRect(myPos.x(), myPos.y(), getWidth(), getHeight());
    //painter.setPen(Qt::green);
    //painter.drawLine(myPos.x(), myPos.y()+getMidline(),
    //                 myPos.x()+getWidth(), myPos.y()+getMidline());
}


void TextElement::setSymbol(bool s)
{
    symbol = s;
    SequenceElement* sequence = dynamic_cast<SequenceElement*>(getParent());
    if (sequence != 0) {
        sequence->parse();
    }
    if (getParent() != 0) {
        formula()->changed();
    }
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

int TextElement::getSpaceBefore(const ContextStyle& context, ContextStyle::TextStyle tstyle)
{
    if (getElementType() != 0) {
        return getElementType()->getSpaceBefore(context, tstyle);
    }
    else {
        return 0;
    }
}

int TextElement::getSpaceAfter(const ContextStyle& context, ContextStyle::TextStyle tstyle)
{
    if (getElementType() != 0) {
        return getElementType()->getSpaceAfter(context, tstyle);
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
    if (symbol) element.setAttribute("SYMBOL", "1");
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
    QString symbolStr = element.attribute("SYMBOL");
    if(!symbolStr.isNull()) {
        symbol = symbolStr.toInt() != 0;
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

QString TextElement::toLatex()
{
    if(isSymbol()) {
	char latin1 = character.latin1();
	switch (latin1) {
	    case 'a': return "\\alpha";	
	    case 'b': return "\\beta";	
	    case 'g': return "\\gamma";	
	    case 'd': return "\\delta";	
	    case 'e': return "\\varepsilon";	
	    case 'z': return "\\zeta";	
	    case 'h': return "\\eta";	
	}
    }
     else
    {

	char latin1 = character.latin1();
	switch (latin1) {
	    case '\\': return "\\backslash";

            default:
                return QChar(latin1);

	}

    }

}
