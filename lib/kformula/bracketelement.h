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

#ifndef BRACKETELEMENT_H
#define BRACKETELEMENT_H

#include <qpoint.h>
#include <qsize.h>

#include "basicelement.h"

class Artwork;
class SequenceElement;


/**
 * A left and/or right bracket around one child.
 */
class BracketElement : public BasicElement {
public:

    enum { contentPos };

    BracketElement(char left = ' ', char right = ' ', BasicElement* parent = 0);
    ~BracketElement();

    /**
     * Sets the cursor and returns the element the point is in.
     * The handled flag shows whether the cursor has been set.
     * This is needed because only the innermost matching element
     * is allowed to set the cursor.
     */
    virtual BasicElement* goToPos(FormulaCursor*, bool& handled,
                                  const KoPoint& point, const KoPoint& parentOrigin);

    /**
     * Calculates our width and height and
     * our children's parentPosition.
     */
    virtual void calcSizes(const ContextStyle& style,  ContextStyle::TextStyle tstyle, ContextStyle::IndexStyle istyle);

    /**
     * Draws the whole element including its children.
     * The `parentOrigin' is the point this element's parent starts.
     * We can use our parentPosition to get our own origin then.
     */
    virtual void draw(QPainter& painter, const QRect& r,
                      const ContextStyle& style,
                      ContextStyle::TextStyle tstyle,
		      ContextStyle::IndexStyle istyle,
		      const KoPoint& parentOrigin);

    /**
     * Enters this element while moving to the left starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or to the left of it.
     */
    virtual void moveLeft(FormulaCursor* cursor, BasicElement* from);

    /**
     * Enters this element while moving to the right starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or to the right of it.
     */
    virtual void moveRight(FormulaCursor* cursor, BasicElement* from);

    /**
     * Enters this element while moving up starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or above it.
     */
    virtual void moveUp(FormulaCursor* cursor, BasicElement* from);

    /**
     * Enters this element while moving down starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or below it.
     */
    virtual void moveDown(FormulaCursor* cursor, BasicElement* from);


    /**
     * Removes all selected children and returns them. Places the
     * cursor to where the children have been.
     *
     * Here we remove ourselve if we are requested to remove our content.
     */
    virtual void remove(FormulaCursor*, QList<BasicElement>&, Direction);

    // main child
    //
    // If an element has children one has to become the main one.

    virtual SequenceElement* getMainChild();
    //virtual void setMainChild(SequenceElement*);

    /**
     * Sets the cursor to select the child. The mark is placed before,
     * the position behind it.
     */
    virtual void selectChild(FormulaCursor* cursor, BasicElement* child);

    /**
     * Moves the cursor away from the given child. The cursor is
     * guaranteed to be inside this element.
     */
    //virtual void childWillVanish(FormulaCursor* cursor, BasicElement* child) = 0;


protected:

    //Save/load support

    /**
     * Returns the tag name of this element type.
     */
    virtual QString getTagName() const { return "BRACKET"; }

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
     * @returns the latex representation of the element and
     * of the element's children
     */
    virtual QString toLatex();


private:
    /**
     * @return a LaTex string for the given symbol
     */
     QString latexString(char);

    /**
     * Creates a new bracket object that matches the char.
     */
    Artwork* createBracket(char bracket);


    /**
     * The brackets we are showing.
     */
    Artwork* left;
    Artwork* right;

    /**
     * Our main child.
     */
    SequenceElement* content;

};


#endif // BRACKETELEMENT_H
