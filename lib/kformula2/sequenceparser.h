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

#ifndef __SEQUENCEPARSER_H
#define __SEQUENCEPARSER_H

#include <qlist.h>
#include <qstring.h>

class BasicElement;
class ElementType;
class SymbolTable;


/**
 * The parser that gets the element list and returns a syntax tree.
 */
class SequenceParser {
public:
    SequenceParser(const SymbolTable& table);

    /**
     * @returns a parse tree.
     */
    ElementType* parse(QList<BasicElement>& elements);

    /**
     * The token types the parser knows.
     */
    enum TokenType { Text, Number, Element, Error, End,
                     Plus='+', Minus='-', Mul='*', Div='/',
                     Assign='=', Less='<', Greater='>', Separator='\\',
                     Comma=',', Colon=':', Semicolon=';'
    };

    /**
     * Reads the next token.
     */
    void nextToken();

    uint getStart() const { return tokenStart; }
    uint getEnd() const { return tokenEnd; }
    TokenType getTokenType() const { return type; }

    /**
     * Tells the element about its new token.
     *
     * @param pos the position of the element
     * @param token the new token
     */
    void setElementType(uint pos, ElementType* type);

    /**
     * @returns a new primitive token.
     */
    ElementType* getPrimitive();

    /**
     * The character that makes a text symbol.
     */
    static const QChar textSymbol;
    
private:

    /**
     * Reads the next token which is a number.
     */
    void readNumber();

    /**
     * Reads a sequence of digits.
     */
    void readDigits();
    
    /**
     * Reads the next token which is some text.
     */
    void readText();

    /**
     * @returns the current token's text
     */
    QString getText();

    /**
     * @returns the char at tokenEnd.
     */
    QChar getEndChar();
    
    /**
     * The elements we want to parse. The parser must not change
     * it!
     */
    QList<BasicElement> list;

    /**
     * The position up to which we have read. The current
     * token starts here.
     */
    uint tokenStart;

    /**
     * The first position after the current token.
     */
    uint tokenEnd;
    
    /**
     * The type of the current token.
     */
    TokenType type;

    /**
     * The table that contains all known symbols.
     */
    const SymbolTable& table;
};


#endif // __SEQUENCEPARSER_H
