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

#ifndef ELEMENTTYPE_H
#define ELEMENTTYPE_H

#include <qfont.h>
#include <qstring.h>

#include "contextstyle.h"

class OperatorToken;
class QPainter;
class SequenceElement;
class SequenceParser;
class SymbolTableEntry;
class TextElement;


/**
 * Basis of all types. Types make up a hierarchy that describes
 * the semantic of the sequence.
 */
class ElementType {
public:
    ElementType(SequenceParser* parser);
    virtual ~ElementType();

    /**
     * @returns the name of the type. Only name types are allowed
     * to have a name.
     */
    virtual QString getName() const { return QString::null; }

    /**
     * @returns whether we want to see this element.
     */
    virtual bool isPhantom(const TextElement&) const { return false; }

    /**
     * @returns the position of the first character
     */
    uint start() { return from; }

    /**
     * @returns the position of the first character after the typed element
     */
    uint end() { return to; }

    /**
     * @returns the space to be left before each char
     * for the given style and font size.
     */
    virtual double getSpaceBefore(const ContextStyle& context, ContextStyle::TextStyle tstyle);

    /**
     * @returns the space to be left after each char
     * for the given style and font size.
     */
    virtual double getSpaceAfter(const ContextStyle& context, ContextStyle::TextStyle tstyle);

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont(const ContextStyle& context);

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter(const ContextStyle& context, QPainter& painter);

    // debug
    virtual void output() = 0;

    // debug
    static int getEvilDestructionCount() { return evilDestructionCount; }

protected:

    void setStart(uint start) { from = start; }
    void setEnd(uint end) { to = end; }

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

    // debug
    static int evilDestructionCount;
};


/**
 * Basis for all tokens that run along several elements.
 */
class MultiElementType : public ElementType {
public:
    MultiElementType(SequenceParser* parser);

    // debug
    virtual void output();
};


/**
 * A text element that doesn't belong to an name.
 * This might be considered an error.
 */
class TextType : public MultiElementType {
public:
    TextType(SequenceParser* parser);

    // debug
    virtual void output();
};


/**
 * A recognized name.
 */
class NameType : public MultiElementType {
public:
    NameType(SequenceParser* parser, QString name);

    /**
     * @returns the name of the type. Only name types are allowed
     * to have a name.
     */
    virtual QString getName() const { return name; }

    /**
     * @returns whether we want to see this element.
     */
    virtual bool isPhantom(const TextElement& element) const;

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont(const ContextStyle& context);

private:

    /**
     * The symbol table entry this name referres to.
     */
    //SymbolTableEntry* entry;

    /**
     * Our name.
     */
    QString name;
};


class TextSymbolType : public MultiElementType {
public:
    TextSymbolType(SequenceParser* parser);

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont(const ContextStyle& context);
};


/**
 * A element that doesn't fit into the syntax.
 */
class ErrorType : public ElementType {
public:
    ErrorType(SequenceParser* parser);

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter(const ContextStyle& context, QPainter& painter);

    // debug
    virtual void output();
};


/**
 * A range of elements that make up a number.
 */
class NumberType : public MultiElementType {
public:
    NumberType(SequenceParser* parser);

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont(const ContextStyle& context);

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter(const ContextStyle& context, QPainter& painter);

    // debug
    virtual void output();
};


/**
 * Basis of all tokens that contain two other tokens
 * which are a left hand side and a right hand side.
 */
class OperatorType : public ElementType {
public:
    OperatorType(SequenceParser* parser);
    ~OperatorType();

    /**
     * @returns the space to be left before each char
     * for the given style and font size.
     */
    virtual double getSpaceBefore(const ContextStyle& context, ContextStyle::TextStyle tstyle);

    /**
     * @returns the space to be left after each char
     * for the given style and font size.
     */
    virtual double getSpaceAfter(const ContextStyle& context, ContextStyle::TextStyle tstyle);

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont(const ContextStyle& context);

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter(const ContextStyle& context, QPainter& painter);

    // debug
    virtual void output();
protected:

    ElementType* lhs;
    ElementType* rhs;
};


/**
 * The addition operator. The parsing starts with it.
 */
class Expression : public OperatorType {
public:
    Expression(SequenceParser* parser);

    // debug
    virtual void output();
};


/**
 * The multiplication operator.
 */
class Term : public OperatorType {
public:
    Term(SequenceParser* parser);

    virtual double getSpaceBefore(const ContextStyle& context, ContextStyle::TextStyle tstyle);

    virtual double getSpaceAfter(const ContextStyle& context, ContextStyle::TextStyle tstyle);

    // debug
    virtual void output();
};


/**
 * The assignment operator.
 */
class Assignment : public OperatorType {
public:
    Assignment(SequenceParser* parser);
};


/**
 * The separator of assignments.
 *
 * A list of separate assignments. That's what each sequence
 * consists of.
 */
class AssignmentSep : public OperatorType {
public:
    AssignmentSep(SequenceParser* parser);

    virtual double getSpaceBefore(const ContextStyle& context, ContextStyle::TextStyle tstyle);

    virtual void setUpPainter(const ContextStyle& context, QPainter& painter);
};


/**
 * An element that has no textual representation.
 */
class ComplexElementType : public ElementType {
public:
    ComplexElementType(SequenceParser* parser);

    // debug
    virtual void output();
};

#endif // ELEMENTTYPE_H
