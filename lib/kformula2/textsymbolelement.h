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

#ifndef __TEXTSYMBOLELEMENT_H
#define __TEXTSYMBOLELEMENT_H

#include "textelement.h"


/**
 * A text char that gets formated as symbol.
 * (Using the symbol font.)
 */
class TextSymbolElement : public TextElement {
public:

    TextSymbolElement(QChar ch = ' ', BasicElement* parent = 0);

    /**
     * @returns the character that marks a complex element.
     * A text symbol does its drawing itself.
     */
    virtual QChar getCharacter() const { return QChar::null; }

protected:
    
    //Save/load support
    
    /**
     * @returns the tag name of this element type.
     */
    virtual QString getTagName() const { return "TEXTSYMBOL"; }
    
    /**
     * Appends our attributes to the dom element.
     */
    virtual void writeDom(QDomElement& element);
    
    /**
     * Reads our attributes from the element.
     * @returns false if it failed.
     */
    virtual bool readAttributesFromDom(QDomElement& element);

    /**
     * @returns the font to be used for the element.
     */
    virtual QFont getFont(const ContextStyle& context);

    /**
     * @returns the space to be left before and behind the character.
     */
    virtual int getSpaceWidth(const ContextStyle& context, int size);

    /**
     * Sets up the painter to be used for drawing.
     */
    virtual void setUpPainter(const ContextStyle& context, QPainter& painter);

private:

    /**
     * The symbol table entry we refer to.
     */
    //QString name;
};


#endif // __TEXTSYMBOLELEMENT_H
