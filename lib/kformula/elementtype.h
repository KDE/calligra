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

class QPainter;

KFORMULA_NAMESPACE_BEGIN

class BasicElement;
class BracketType;
class ComplexElementType;
class InnerElementType;
class MultiElementType;
class OperatorType;
class PunctuationType;
class RelationType;
class SequenceElement;
class SequenceParser;
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
    virtual bool isInvisible(const TextElement&) const { return false; }

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
    virtual double getSpaceBefore( const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( MultiElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( OperatorType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( RelationType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( PunctuationType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( BracketType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( ComplexElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( InnerElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont( const ContextStyle& context );

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter( const ContextStyle& context, QPainter& painter );

    // debug
    static int getEvilDestructionCount() { return evilDestructionCount; }

    virtual void output();

    /**
     * Adds a type at the end of the list.
     */
    void append( ElementType* );

    ElementType* getPrev() const { return prev; }

protected:

    void setStart( uint start ) { from = start; }
    void setEnd( uint end ) { to = end; }

    double thinSpaceIfNotScript( const ContextStyle& context, ContextStyle::TextStyle tstyle );
    double mediumSpaceIfNotScript( const ContextStyle& context, ContextStyle::TextStyle tstyle );
    double thickSpaceIfNotScript( const ContextStyle& context, ContextStyle::TextStyle tstyle );

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

    /**
     * We implement this list ourselves because we need to know
     * our neighbours.
     */
    ElementType* prev;

    // debug
    static int evilDestructionCount;
};


/**
 * The token that belongs to a sequence. Contains all the
 * other tokens.
 */
class SequenceType : public ElementType {
public:
    SequenceType( SequenceParser* parser );
    ~SequenceType();

    virtual void output();
private:

    /**
     * The last token type of this sequences chain.
     */
    ElementType* last;
};


/**
 * Basis for all tokens that run along several elements.
 */
class MultiElementType : public ElementType {
public:
    MultiElementType( SequenceParser* parser );

    virtual double getSpaceBefore( const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( OperatorType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( RelationType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( InnerElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
};


/**
 * A text element that doesn't belong to an name.
 * This might be considered an error.
 */
class TextType : public MultiElementType {
public:
    TextType( SequenceParser* parser );
};


/**
 * A recognized name.
 */
class NameType : public MultiElementType {
public:
    NameType( SequenceParser* parser, QString name );

    /**
     * @returns the name of the type. Only name types are allowed
     * to have a name.
     */
    virtual QString getName() const { return name; }

    /**
     * @returns whether we want to see this element.
     */
    virtual bool isInvisible( const TextElement& element ) const;

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont( const ContextStyle& context );

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
};


/**
 * Basis for all tokens that consist of one element only.
 */
class SingleElementType : public ElementType {
public:
    SingleElementType( SequenceParser* parser );
};


class OperatorType : public SingleElementType {
public:
    OperatorType( SequenceParser* parser );

    virtual double getSpaceBefore( const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( MultiElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( BracketType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( ComplexElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( InnerElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont(const ContextStyle& context);

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter(const ContextStyle& context, QPainter& painter);
};


class RelationType : public SingleElementType {
public:
    RelationType( SequenceParser* parser );

    virtual double getSpaceBefore( const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( MultiElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( BracketType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( ComplexElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( InnerElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont( const ContextStyle& context );

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter( const ContextStyle& context, QPainter& painter );
};


class PunctuationType : public SingleElementType {
public:
    PunctuationType( SequenceParser* parser );

    virtual double getSpaceBefore( const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( MultiElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( RelationType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( PunctuationType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( BracketType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( ComplexElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( InnerElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );

    /**
     * @returns the font to be used for this kind of element
     */
    virtual QFont getFont( const ContextStyle& context );

    /**
     * sets the painters pen to a appropiate value
     */
    virtual void setUpPainter( const ContextStyle& context, QPainter& painter );
};


class BracketType : public SingleElementType {
public:
    BracketType( SequenceParser* parser );

    virtual double getSpaceBefore( const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( OperatorType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( RelationType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( InnerElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
};


class ComplexElementType : public SingleElementType {
public:
    ComplexElementType( SequenceParser* parser );

    // these spacings are equal to the ones from MultiElementType
    virtual double getSpaceBefore( const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( OperatorType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( RelationType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( InnerElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
};


class InnerElementType : public SingleElementType {
public:
    InnerElementType( SequenceParser* parser );

    virtual double getSpaceBefore( const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( MultiElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( OperatorType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( RelationType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( PunctuationType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( BracketType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( ComplexElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
    virtual double getSpaceAfter( InnerElementType* type, const ContextStyle& context, ContextStyle::TextStyle tstyle );
};


KFORMULA_NAMESPACE_END

#endif // ELEMENTTYPE_H
