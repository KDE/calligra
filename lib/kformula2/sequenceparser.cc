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
#include "elementtype.h"
#include "sequenceparser.h"
#include "symboltable.h"

SequenceParser::SequenceParser(const SymbolTable& t)
        : tokenStart(0), tokenEnd(0), table(t)
{
}


void SequenceParser::setElementType(uint pos, ElementType* type)
{
    list.at(pos)->setElementType(type);
}


ElementType* SequenceParser::parse(QList<BasicElement>& elements)
{
    list = elements;
    nextToken();
    return new Assignment(this);
}


void SequenceParser::nextToken()
{
    tokenStart = tokenEnd;
    if (tokenStart >= list.count()) {
        type = End;
        return;
    }
    tokenEnd++;
    BasicElement* element = list.at(tokenStart);
    QChar ch = element->getCharacter();
    char latin1 = ch.latin1();
    switch (latin1) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
            type = TokenType(latin1);
            break;
        case '\0':
            type = Element;
            break;
        default:
            if (ch.isNumber()) {
                readNumber();
                return;
            }
            else {
                readText();
            }
    }
}


void SequenceParser::readNumber()
{
    type = Number;
    readDigits();
    if (tokenEnd < list.count()-1) {
        QChar ch = getEndChar();

        // Look for a dot.
        if (ch == '.') {
            tokenEnd++;
            ch = getEndChar();
            if (ch.isNumber()) {
                readDigits();
            }
            else {
                tokenEnd--;
                return;
            }
        }

        // there might as well be an exponent
        if (tokenEnd < list.count()-1) {
            ch = getEndChar();
            //if ((ch == 'E') || (ch == 'e')) {
            // Because the symbol font doesn't have a 'e'.
            if (ch == 'E') {
                tokenEnd++;
                ch = getEndChar();

                // signs are allowed after the exponent
                if (((ch == '+') || (ch == '-')) &&
                    (tokenEnd < list.count()-1)) {
                    tokenEnd++;
                    ch = getEndChar();
                    if (ch.isNumber()) {
                        readDigits();
                    }
                    else {
                        tokenEnd -= 2;
                        return;
                    }
                }
                else if (ch.isNumber()) {
                    readDigits();
                }
                else {
                    tokenEnd--;
                }
            }
        }
    }
}


void SequenceParser::readDigits()
{
    for (; tokenEnd < list.count(); tokenEnd++) {
        QChar ch = getEndChar();
        if (!ch.isNumber()) {
            break;
        }
    }
}


void SequenceParser::readText()
{
    type = Text;
    for (; tokenEnd < list.count(); tokenEnd++) {
        QChar ch = getEndChar();
        if (!ch.isLetter()) {
            return;
        }
    }
}

QChar SequenceParser::getEndChar()
{
    BasicElement* element = list.at(tokenEnd);
    return element->getCharacter();
}


ElementType* SequenceParser::getPrimitive()
{
    switch (type) {
        case Text: {
            QString text = getText();
            if (table.contains(text)) {
                return new NameType(this, text);
            }
            else {
                return new TextType(this);
            }
        }
        case Number:
            return new NumberType(this);
        case Element:
            return new ComplexElementType(this);
        case End:
            return 0;
        case Minus: {
            // dirty hack: Lets save the current token end
            // so we can undo the next read.
            uint endPos = tokenEnd;
            nextToken();
            if (type == Number) {
                tokenStart--;
                return new NumberType(this);
            }
            else {
                tokenStart = endPos-1;
                tokenEnd = endPos;
                return new ErrorType(this);
            }
        }
        case Error:
        default:
            return new ErrorType(this);
    }
}


QString SequenceParser::getText()
{
    QString text;
    for (uint i = tokenStart; i < tokenEnd; i++) {
        BasicElement* element = list.at(i);
        text.append(element->getCharacter());
    }
    return text;
}
