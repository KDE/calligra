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

#ifndef __SEQUENCEELEMENT_H
#define __SEQUENCEELEMENT_H

// Qt Include
#include <qlist.h>

// Formula include
#include "basicelement.h"


/**
 * The element that contains a number of children.
 * The children are aligned in one line.
 */
class SequenceElement : public BasicElement {
public:

    SequenceElement(BasicElement* parent = 0);
    ~SequenceElement();
    
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

    /**
     * If the cursor is inside a sequence it needs to be drawn.
     */
    void drawCursor(FormulaCursor* cursor, QPainter& painter);
    
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

    /**
     * Moves the cursor to the first position in this sequence.
     * (That is before the first child.)
     */
    virtual void moveHome(FormulaCursor* cursor);

    /**
     * Moves the cursor to the last position in this sequence.
     * (That is behind the last child.)
     */
    virtual void moveEnd(FormulaCursor* cursor);

    /**
     * Sets the cursor inside this element to its start position.
     * For most elements that is the main child.
     */
    virtual void goInside(FormulaCursor* cursor);

    
    // children

    /**
     * Removes the child. If this was the main child this element might
     * request its own removal.
     * The cursor is the one that caused the removal. It has to be moved
     * to the place any user expects the cursor after that particular
     * element has been removed.
     */
    //virtual void removeChild(FormulaCursor* cursor, BasicElement* child);


    /**
     * Inserts all new children at the cursor position. Places the
     * cursor according to the direction. The inserted elements will
     * be selected.
     *
     * The list will be emptied but stays the property of the caller.
     */
    virtual void insert(FormulaCursor*, QList<BasicElement>&, Direction);

    /**
     * Removes all selected children and returns them. Places the
     * cursor to where the children have been.
     */
    virtual void remove(FormulaCursor*, QList<BasicElement>&, Direction);

    /**
     * If an element is not needed anymore it requests to be
     * replaced.
     */
    //void replaceElementByMainChild(FormulaCursor* cursor, BasicElement* child);

    /**
     * Returns the child at the cursor.
     * Does not care about the selection.
     */
    virtual BasicElement* getChild(FormulaCursor*, Direction = beforeCursor);

    /**
     * Returns the number of children we have.
     */
    int countChildren() const { return children.count(); }

    /**
     * Selects all children. The cursor is put behind, the mark before them.
     */
    void selectAllChildren(FormulaCursor* cursor);
    
    //BasicElement* getChildAt(int pos);

    // debug
    virtual ostream& output(ostream&);
    
private:

    /**
     * Removes the children at pos and appends it to the list.
     */
    void removeChild(QList<BasicElement>& removedChildren, int pos);

    
    /**
     * Our children. Be sure to notify the rootElement before
     * you remove any.
     */
    QList<BasicElement> children;
};

#endif // __SEQUENCEELEMENT_H
