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

#ifndef __FORMULATOKEN_H
#define __FORMULATOKEN_H

#include <iostream.h>

#include <qlist.h>
#include <qstring.h>


class BasicElement;
class ContextStyle;
class OperatorToken;
class QFont;
class QPainter;
class SequenceElement;
class SymbolTableEntry;
class Token;


/**
 * The parser that gets the element list and returns a syntax tree.
 */
class SequenceParser {
public:
    SequenceParser();

    /**
     * @returns a parse tree.
     */
    Token* parse(QList<BasicElement>& elements);

    /**
     * The token types the parser knows.
     */
    enum TokenType { Text, Number, Element, Error, End,
                     Plus='+', Minus='-', Mul='*', Div='/',
                     Assign='='
    };

    /**
     * Reads the next token.
     */
    void nextToken();

    uint getStart() const { return tokenStart; }
    uint getEnd() const { return tokenEnd; }
    TokenType getType() const { return type; }

    /**
     * Tells the element about its new token.
     *
     * @param pos the position of the element
     * @param token the new token
     */
    void setToken(uint pos, Token* token);

    /**
     * @returns a new primitive token.
     */
    Token* getPrimitive();
    
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
};


/**
 * Basis of all token. Token make up a hierarchy that describes
 * the semantic of the sequence.
 */
class Token {
public:
    Token() { evilDestructionCount++; }
    virtual ~Token() { evilDestructionCount--; }

    virtual void output() = 0;

    /**
     * @returns the space to be left before and behind each char
     * for the given style and font size.
     */
    virtual int getSpace(ContextStyle& context, int size);
    
    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont(ContextStyle& context);

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter(ContextStyle& context, QPainter& painter);
    
    // debug
    static int getEvilDestructionCount() { return evilDestructionCount; }

private:

    // debug
    static int evilDestructionCount;
};


/**
 * Basis for all tokens that run along several elements.
 */
class MultiElementToken : public Token {
public:
    MultiElementToken(SequenceParser* parser);

    virtual void output();
    
private:

    /**
     * the index of the first element that belongs
     * to the name.
     */
    uint from;

    /**
     * the index of the first element that doesn't belong
     * to the name.
     */
    uint to;
};


/**
 * A text element that doesn't belong to an name.
 * This might be considered an error.
 */
class TextToken : public MultiElementToken {
public:
    TextToken(SequenceParser* parser);

    // debug
    virtual void output();
};


// /**
//  * A recognized name.
//  */
// class NameToken : public MultiElementToken {
// public:
//     NameToken(SequenceParser* parser);
    
// private:

//     /**
//      * The symbol table entry this name referres to.
//      */
//     SymbolTableEntry* entry;
// };


/**
 * A element that doesn't fit into the syntax.
 */
class ErrorToken : public Token {
public:
    ErrorToken(SequenceParser* parser);

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter(ContextStyle& context, QPainter& painter);

    // debug
    virtual void output();
    
private:

    /**
     * The elements position.
     */
    uint pos;
};


/**
 * A range of elements that make up a number.
 */
class NumberToken : public MultiElementToken {
public:
    NumberToken(SequenceParser* parser);

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont(ContextStyle& context);

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter(ContextStyle& context, QPainter& painter);

    // debug
    virtual void output();
};


/**
 * Basis of all tokens that contain two other tokens
 * which are a left hand side and a right hand side.
 */
class OperatorToken : public Token {
public:
    OperatorToken();
    ~OperatorToken();

    /**
     * @returns the space to be left before and behind each char
     * for the given style and font size.
     */
    virtual int getSpace(ContextStyle& context, int size);
    
    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont(ContextStyle& context);

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter(ContextStyle& context, QPainter& painter);

    // debug
    virtual void output();
protected:

    Token* lhs;
    Token* rhs;
};


/**
 * The addition operator. The parsing starts with it.
 */
class Expression : public OperatorToken {
public:
    Expression(SequenceParser* parser);

    virtual void output();
};


/**
 * The multiplication operator.
 */
class Term : public OperatorToken {
public:
    Term(SequenceParser* parser);

    virtual void output();
};


// /**
//  * The assignment operator.
//  */
// class Assignment : public OperatorToken {
// public:
//     Assignment(SequenceParser* parser);
// };


/**
 * An element that has no textual representation.
 */
class ElementToken : public Token {
public:
    ElementToken(SequenceParser* parser);

    virtual void output();
private:

    uint pos;
};

#endif // __FORMULATOKEN_H
