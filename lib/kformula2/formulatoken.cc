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

#include <ctype.h>

#include <qfont.h>
#include <qfontmetrics.h>
#include <qpainter.h>

#include "basicelement.h"
#include "contextstyle.h"
#include "formulatoken.h"


int Token::evilDestructionCount = 0;


SequenceParser::SequenceParser()
        : tokenStart(0), tokenEnd(0)
{
}


void SequenceParser::setToken(uint pos, Token* token)
{
    list.at(pos)->setToken(token);
}


Token* SequenceParser::parse(QList<BasicElement>& elements)
{
    list = elements;
    nextToken();
    return new Expression(this);
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


Token* SequenceParser::getPrimitive()
{
    switch (type) {
        case Text:
            return new TextToken(this);
        case Number:
            return new NumberToken(this);
        case Element:
            return new ElementToken(this);
        case End:
            return 0;
        case Minus: {
            // dirty hack: Lets save the current token end
            // so we can undo the next read.
            uint endPos = tokenEnd;
            nextToken();
            if (type == Number) {
                tokenStart--;
                return new NumberToken(this);
            }
            else {
                tokenStart = endPos-1;
                tokenEnd = endPos;
                return new ErrorToken(this);
            }
        }
        case Error:
        default:
            return new ErrorToken(this);
    }
}


int Token::getSpace(ContextStyle&, int)
{
    return 0;
}

QFont Token::getFont(ContextStyle& context)
{
    return context.getDefaultFont();
}

void Token::setUpPainter(ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getDefaultColor());
}


OperatorToken::OperatorToken()
        : lhs(0), rhs(0)
{
}

OperatorToken::~OperatorToken()
{
    delete rhs;
    delete lhs;
}

int OperatorToken::getSpace(ContextStyle& context, int size)
{
    QFont font = context.getOperatorFont();
    font.setPointSize(size);
    QFontMetrics fm(font);
    return fm.width('0')/2;
}

QFont OperatorToken::getFont(ContextStyle& context)
{
    return context.getOperatorFont();
}

void OperatorToken::setUpPainter(ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getOperatorColor());
}


void ErrorToken::setUpPainter(ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getErrorColor());
}


QFont NumberToken::getFont(ContextStyle& context)
{
    return context.getNumberFont();
}

void NumberToken::setUpPainter(ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getNumberColor());
}


Expression::Expression(SequenceParser* parser)
{
    lhs = new Term(parser);

    switch (parser->getType()) {
        case SequenceParser::Plus:
        case SequenceParser::Minus:
            parser->setToken(parser->getStart(), this);
            parser->nextToken();
            rhs = new Expression(parser);
            break;
        case SequenceParser::End:
            break;
        default:
            rhs = new ErrorToken(parser);
    }
}


Term::Term(SequenceParser* parser)
{
    lhs = parser->getPrimitive();
    
    switch (parser->getType()) {
        case SequenceParser::Mul:
        case SequenceParser::Div:
            parser->setToken(parser->getStart(), this);
            parser->nextToken();
            rhs = new Term(parser);
            break;
        case SequenceParser::Text:
        case SequenceParser::Number:
        case SequenceParser::Element:
            // assume mul if no operator
            rhs = new Term(parser);
            break;
        case SequenceParser::End:
            break;
        default:
            break;
    }
}


ErrorToken::ErrorToken(SequenceParser* parser)
{
    pos = parser->getStart();
    parser->setToken(pos, this);
    parser->nextToken();
}


MultiElementToken::MultiElementToken(SequenceParser* parser)
{
    from = parser->getStart();
    to = parser->getEnd();
    for (uint i = from; i < to; i++) {
        parser->setToken(i, this);
    }
    parser->nextToken();
}


TextToken::TextToken(SequenceParser* parser)
        : MultiElementToken(parser)
{
}


NumberToken::NumberToken(SequenceParser* parser)
        : MultiElementToken(parser)
{
}


ElementToken::ElementToken(SequenceParser* parser)
{
    pos = parser->getStart();
    parser->setToken(pos, this);
    parser->nextToken();
}


void MultiElementToken::output()
{
    cout << "from = " << from << "\tto = " << to << endl;
}

void TextToken::output()
{
    cout << "TextToken: ";
    MultiElementToken::output();
}


void ErrorToken::output()
{
    cout << "ErrorToken: pos = " << pos << endl;
}

void NumberToken::output()
{
    cout << "NumberToken: ";
    MultiElementToken::output();
}


void OperatorToken::output()
{
    cout << "{\n";
    if (lhs != 0) {
        cout << "{\n";
        lhs->output();
        cout << "}\n";
    }
    if (rhs != 0) {
        cout << "{\n";
        rhs->output();
        cout << "}\n";
    }
    cout << "}" << endl;
}

void Expression::output()
{
    cout << "Expression: ";
    OperatorToken::output();
}


void Term::output()
{
    cout << "Term: ";
    OperatorToken::output();
}


void ElementToken::output()
{
    cout << "ElementToken" << endl;
}

