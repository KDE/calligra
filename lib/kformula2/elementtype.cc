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

#include <iostream.h>

#include <qfont.h>
#include <qfontmetrics.h>
#include <qpainter.h>

#include "basicelement.h"
#include "contextstyle.h"
#include "elementtype.h"
#include "sequenceparser.h"


int ElementType::evilDestructionCount = 0;


ElementType::ElementType(SequenceParser* parser)
        : from(parser->getStart()), to(parser->getEnd())
{
    evilDestructionCount++;
}

ElementType::~ElementType()
{
    evilDestructionCount--;
}


OperatorType::OperatorType(SequenceParser* parser)
        : ElementType(parser), lhs(0), rhs(0)
{
}

OperatorType::~OperatorType()
{
    delete rhs;
    delete lhs;
}


Assignment::Assignment(SequenceParser* parser)
        : OperatorType(parser)
{
    lhs = new Expression(parser);

    switch (parser->getTokenType()) {
        case SequenceParser::Assign:
        case SequenceParser::Less:
        case SequenceParser::Greater:
            parser->setElementType(parser->getStart(), this);
            parser->nextToken();
            rhs = new Assignment(parser);
            break;
        case SequenceParser::End:
            break;
        default:
            rhs = new ErrorType(parser);
    }
}


Expression::Expression(SequenceParser* parser)
        : OperatorType(parser)
{
    lhs = new Term(parser);

    switch (parser->getTokenType()) {
        case SequenceParser::Plus:
        case SequenceParser::Minus:
            parser->setElementType(parser->getStart(), this);
            parser->nextToken();
            rhs = new Expression(parser);
            break;
        case SequenceParser::End:
            break;
        default:
            //rhs = new ErrorType(parser);
            break;
    }
}


Term::Term(SequenceParser* parser)
        : OperatorType(parser)
{
    lhs = parser->getPrimitive();
    
    switch (parser->getTokenType()) {
        case SequenceParser::Mul:
        case SequenceParser::Div:
            parser->setElementType(parser->getStart(), this);
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


ErrorType::ErrorType(SequenceParser* parser)
        : ElementType(parser)
{
    parser->setElementType(start(), this);
    parser->nextToken();
}


MultiElementType::MultiElementType(SequenceParser* parser)
        : ElementType(parser)
{
    for (uint i = start(); i < end(); i++) {
        parser->setElementType(i, this);
    }
    parser->nextToken();
}


TextType::TextType(SequenceParser* parser)
        : MultiElementType(parser)
{
}


NameType::NameType(SequenceParser* parser, QString n)
        : MultiElementType(parser), name(n)
{
}


NumberType::NumberType(SequenceParser* parser)
        : MultiElementType(parser)
{
}


ComplexElementType::ComplexElementType(SequenceParser* parser)
        : ElementType(parser)
{
    parser->setElementType(start(), this);
    parser->nextToken();
}


int ElementType::getSpace(const ContextStyle&, int)
{
    return 0;
}

QFont ElementType::getFont(const ContextStyle& context)
{
    return context.getDefaultFont();
}

void ElementType::setUpPainter(const ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getDefaultColor());
}



int OperatorType::getSpace(const ContextStyle& context, int size)
{
    QFont font = context.getOperatorFont();
    font.setPointSize(size);
    QFontMetrics fm(font);
    return fm.width('0')/2;
}

QFont OperatorType::getFont(const ContextStyle& context)
{
    return context.getOperatorFont();
}

void OperatorType::setUpPainter(const ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getOperatorColor());
}


void ErrorType::setUpPainter(const ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getErrorColor());
}

QFont NameType::getFont(const ContextStyle& context)
{
    return context.getNameFont();
}


QFont NumberType::getFont(const ContextStyle& context)
{
    return context.getNumberFont();
}

void NumberType::setUpPainter(const ContextStyle& context, QPainter& painter)
{
    painter.setPen(context.getNumberColor());
}


void MultiElementType::output()
{
    cout << "from = " << start() << "\tto = " << end() << endl;
}

void TextType::output()
{
    cout << "TextType: ";
    MultiElementType::output();
}


void ErrorType::output()
{
    cout << "ErrorType: pos = " << start() << endl;
}

void NumberType::output()
{
    cout << "NumberType: ";
    MultiElementType::output();
}


void OperatorType::output()
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
    OperatorType::output();
}


void Term::output()
{
    cout << "Term: ";
    OperatorType::output();
}


void ComplexElementType::output()
{
    cout << "ElementType" << endl;
}

