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

#include <qfontmetrics.h>
#include <qpainter.h>

#include <kdebug.h>

#include "basicelement.h"
#include "contextstyle.h"
#include "elementtype.h"
#include "formulaelement.h"
#include "sequenceelement.h"
#include "textelement.h"
#include "symboltable.h"

KFORMULA_NAMESPACE_BEGIN

TextElement::TextElement(QChar ch, bool beSymbol, BasicElement* parent)
        : BasicElement(parent), character(ch), symbol(beSymbol)
{
}


TokenType TextElement::getTokenType() const
{
    if (isSymbol()) {
        // Hack
        switch ( getSymbolTable().charClass(character) ) {
        case ORDINARY:
            return TEXT;
        case BINOP:
            return MUL;
        case RELATION:
            return ASSIGN;
        case PUNCTUATION:
            return COMMA;
        case UNKNOWN:
            return ERROR;
        }
    }

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


bool TextElement::isPhantom() const
{
    if (getElementType() != 0) {
        return getElementType()->isPhantom(*this);
    }
    return false;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void TextElement::calcSizes(const ContextStyle& context, ContextStyle::TextStyle tstyle, ContextStyle::IndexStyle /*istyle*/)
{
    double mySize = context.getAdjustedSize( tstyle );

    QFont font = getFont( context );
    font.setPointSizeFloat( mySize );
    double spaceBefore = getSpaceBefore( context, tstyle );
    double spaceAfter = getSpaceAfter( context, tstyle );

    QFontMetrics fm(font);
    //QRect bound = fm.boundingRect(character);
    QChar ch;
    if ( !isSymbol() ) {
        ch = character;
    }
    else {
        ch = getSymbolTable().character(character);
    }
    QRect bound = fm.boundingRect(QString(ch));

    setWidth(fm.width(ch) + spaceBefore + spaceAfter);
    setHeight(bound.height());
    setBaseline(-bound.top());
    setMidline(getBaseline() - fm.strikeOutPos());

    //kdDebug() << "bound.height(): " << bound.height() << endl;
    //kdDebug() << "bound.top(): " << bound.top() << endl;
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
		       const KoPoint& parentOrigin)
{
    KoPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    double mySize = context.getAdjustedSize( tstyle );
    if (!QRect(myPos.x(), myPos.y(), getWidth(), getHeight()).intersects(r))
        return;

    QFont font = getFont(context);
    font.setPointSizeFloat(mySize);
    setUpPainter(context, painter);
    int spaceBefore = static_cast<int>( getSpaceBefore(context, tstyle) );
    //int spaceAfter = getSpaceAfter(context, tstyle);

    painter.setFont(font);

    QChar ch;
    if ( !isSymbol() ) {
        ch = character;
    }
    else {
        ch = getSymbolTable().character(character);
    }
    painter.drawText(myPos.x()+spaceBefore,
                     myPos.y()+getBaseline(), ch);

    // Debug
    //painter.setBrush(Qt::NoBrush);
    //painter.setPen(Qt::red);
    //painter.drawRect(myPos.x(), myPos.y(), getWidth(), getHeight());
    //painter.setPen(Qt::green);
    //painter.drawLine(myPos.x(), myPos.y()+getMidline(),
    //                 myPos.x()+getWidth(), myPos.y()+getMidline());
}


QFont TextElement::getFont(const ContextStyle& context)
{
    if ( !isSymbol() ) {
        if (getElementType() != 0) {
            return getElementType()->getFont(context);
        }
        else {
            return context.getDefaultFont();
        }
    }
    return getSymbolTable().font(character);
}

double TextElement::getSpaceBefore(const ContextStyle& context, ContextStyle::TextStyle tstyle)
{
    if (getElementType() != 0) {
        return getElementType()->getSpaceBefore(context, tstyle);
    }
    else {
        return 0;
    }
}

double TextElement::getSpaceAfter(const ContextStyle& context, ContextStyle::TextStyle tstyle)
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

const SymbolTable& TextElement::getSymbolTable() const
{
    return formula()->getSymbolTable();
}


/**
 * Appends our attributes to the dom element.
 */
void TextElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);
    element.setAttribute("CHAR", QString(character));
    if (symbol) element.setAttribute("SYMBOL", "2");
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
        int symbolInt = symbolStr.toInt();
        if ( symbolInt == 1 ) {
            character = getSymbolTable().unicodeFromSymbolFont(character);
        }
        symbol = symbolInt != 0;
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
    if (isSymbol()) {
        return getSymbolTable().name(character);
    }
    else {
        return character;
    }
}

KFORMULA_NAMESPACE_END
