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

#ifndef __INDEXELEMENT_H
#define __INDEXELEMENT_H

// Formula include
#include "complexelement.h"

class SequenceElement;


/**
 * The element with up to four indexes in the four corners.
 */
class IndexElement : public ComplexElement {
public:

    // each index has its own number.
    enum { upperLeftPos, lowerLeftPos, contentPos, upperRightPos, lowerRightPos };
    
    IndexElement(BasicElement* parent = 0);
    ~IndexElement();
    
    /**
     * Returns the element the point is in.
     */
    virtual BasicElement* isInside(const QPoint& point, const QPoint& parentOrigin);

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
    virtual void calcSizes(ContextStyle& context, int parentSize);

    /**
     * Draws the whole element including its children.
     * The `parentOrigin' is the point this element's parent starts.
     * We can use our parentPosition to get our own origin then.
     */
    virtual void draw(QPainter& painter, ContextStyle& context,
                      int parentSize, const QPoint& parentOrigin);

    
    // navigation
    // 
    // The elements are responsible to handle cursor movement themselves.
    // To do this they need to know the direction the cursor moves and
    // the element it comes from.
    //
    // The cursor might be in normal or in selection mode.
    
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

    // children

    /**
     * Removes the child. If this was the main child this element might
     * request its own removal.
     * The cursor is the one that caused the removal. It has to be moved
     * to the place any user expects the cursor after that particular
     * element has been removed.
     */
    //virtual void removeChild(FormulaCursor* cursor, BasicElement* child);

    
    // main child
    //
    // If an element has children one has to become the main one.
    
    virtual SequenceElement* getMainChild() { return content; }
    virtual void setMainChild(SequenceElement*);

    
    /**
     * Inserts all new children at the cursor position. Places the
     * cursor according to the direction.
     *
     * You only can insert one index at a time. So the list must contain
     * exactly on SequenceElement. And the index you want to insert
     * must not exist already.
     *
     * The list will be emptied but stays the property of the caller.
     */
    virtual void insert(FormulaCursor*, QList<BasicElement>&, Direction);

    /**
     * Removes all selected children and returns them. Places the
     * cursor to where the children have been.
     *
     * The cursor has to be inside one of our indexes which is supposed
     * to be empty. The index will be removed and the cursor will
     * be placed to the removed index so it can be inserted again.
     * This methode is called by SequenceElement::remove only.
     *
     * The ownership of the list is passed to the caller.
     */
    virtual void remove(FormulaCursor*, QList<BasicElement>&, Direction);

    /**
     * Moves the cursor to a normal place where new elements
     * might be inserted.
     */
    virtual void normalize(FormulaCursor*, Direction);
    
    /**
     * Returns the child at the cursor.
     */
    virtual BasicElement* getChild(FormulaCursor*, Direction = beforeCursor);

    /**
     * Sets the cursor to select the child. The mark is placed before,
     * the position behind it.
     */
    virtual void selectChild(FormulaCursor* cursor, BasicElement* child);

    /**
     * Returns wether the element has no more useful
     * children (except its main child) and should therefore
     * be replaced by its main child's content.
     */
    virtual bool isSenseless();
    
    
    bool hasUpperLeft()  const { return upperLeft  != 0; }
    bool hasUpperRight() const { return upperRight != 0; }
    bool hasLowerLeft()  const { return lowerLeft  != 0; }
    bool hasLowerRight() const { return lowerRight != 0; }

    // Return the index. If there is non, create it first.
    
//     SequenceElement* requireUpperLeft();
//     SequenceElement* requireUpperRight();
//     SequenceElement* requireLowerLeft();
//     SequenceElement* requireLowerRight();
    
    // If we want to create an index we need a cursor that points there.
    
    void setToUpperLeft(FormulaCursor* cursor);
    void setToUpperRight(FormulaCursor* cursor);
    void setToLowerLeft(FormulaCursor* cursor);
    void setToLowerRight(FormulaCursor* cursor);

    // If the index is there we need a way to move into it.

    void moveToUpperLeft(FormulaCursor* cursor, Direction direction);
    void moveToUpperRight(FormulaCursor* cursor, Direction direction);
    void moveToLowerLeft(FormulaCursor* cursor, Direction direction);
    void moveToLowerRight(FormulaCursor* cursor, Direction direction);


    // ComplexElement
    
    // Moves the cursor inside the index. The index has to exist.
    virtual void moveToUpperIndex(FormulaCursor* cursor, Direction direction)
        { moveToUpperRight(cursor, direction); }
    virtual void moveToLowerIndex(FormulaCursor* cursor, Direction direction)
        { moveToLowerRight(cursor, direction); }
    
    // Sets the cursor to point to the place where the index normaly
    // is. These functions are only used if there is no such index and
    // we want to insert them.
    virtual void setToUpperIndex(FormulaCursor* cursor) { setToUpperRight(cursor); }
    virtual void setToLowerIndex(FormulaCursor* cursor) { setToLowerRight(cursor); }

    // Tells whether we own those indexes
    virtual bool hasUpperIndex() const { return hasUpperRight(); }
    virtual bool hasLowerIndex() const { return hasLowerRight(); }
    
    
    virtual QDomElement getElementDom(QDomDocument *doc);
        
    // debug
    virtual ostream& output(ostream&);

private:

    /**
     * Sets the cursor to point to the place where the content is.
     * There always is a content so this is not a useful place.
     * No insertion or removal will succeed as long as the cursor is
     * there.
     */
    void setToContent(FormulaCursor* cursor);
    
    /**
     * Our main child. This is guaranteed not to be null.
     */
    SequenceElement* content;

    /**
     * The four indexes. Each one might be null.
     * If the last one is removed the whole IndexElement
     * should be replaced by its main child.
     */
    SequenceElement* upperLeft;
    SequenceElement* upperRight;
    SequenceElement* lowerLeft;
    SequenceElement* lowerRight;
};


#endif // __INDEXELEMENT_H
