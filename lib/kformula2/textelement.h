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

#ifndef __TEXTELEMENT_H
#define __TEXTELEMENT_H

#include <qfont.h>
#include <qstring.h>

#include "basicelement.h"


/**
 * A element that represents one char.
 */
class TextElement : public BasicElement {
public:

    TextElement(QChar ch = ' ', BasicElement* parent = 0);

    
    /**
     * @returns the character that represents this element. Used for
     * parsing a sequence.
     */
    virtual QChar getCharacter() const { return character; }

    /**
     * @returns true if we don't want to see the element.
     */
    virtual bool isPhantom() const { return character == '\\'; }
    
    // drawing
    //
    // Drawing depends on a context which knows the required properties like
    // fonts, spaces and such.
    // It is essential to calculate elements size with the same context
    // before you draw.
    
    /**
     * Calculates our width and height and
     * our children's parentPosition.
     */
    virtual void calcSizes(const ContextStyle& context, int parentSize);

    /**
     * Draws the whole element including its children.
     * The `parentOrigin' is the point this element's parent starts.
     * We can use our parentPosition to get our own origin then.
     */
    virtual void draw(QPainter& painter, const ContextStyle& context,
                      int parentSize, const QPoint& parentOrigin);

protected:

    //Save/load support
    
    /**
     * @returns the tag name of this element type.
     */
    virtual QString getTagName() const { return "TEXT"; }
    
    /**
     * Appends our attributes to the dom element.
     */
    virtual void writeDom(QDomElement& element);
    
    /**
     * Reads our attributes from the element.
     * Returns false if it failed.
     */
    virtual bool readAttributesFromDom(QDomElement& element);

    /**
     * Reads our content from the node. Sets the node to the next node
     * that needs to be read.
     * Returns false if it failed.
     */
    virtual bool readContentFromDom(QDomNode& node);


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
     * Our content.
     */
    QChar character;

    /**
     * The position of our baseline.
     */
    int baseline;

    /**
     * Room to be left before and after the character.
     */
    int spaceWidth;
};


#endif // __TEXTELEMENT_H
