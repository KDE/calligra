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

#ifndef __BASICELEMENT_H
#define __BASICELEMENT_H

#include <iostream>

// Qt Include
#include <qlist.h>
#include <qpoint.h>
#include <qsize.h>

// KDE Include


// Formula include
#include "contextstyle.h"

class FormulaCursor;
class FormulaElement;
class SequenceElement;


/**
 * Basis of every formula element.
 *
 * Each element might contain children. Each child needs
 * its own unique number. It is not guaranteed, however,
 * that the number stays the same all the time.
 * (The SequenceElement's children are simply counted.)
 */
class BasicElement
{
public:  

    BasicElement(BasicElement* parent = 0);
    virtual ~BasicElement();

    /**
     * Provide fast access to the rootElement for each child.
     */
    virtual FormulaElement* formula() { return parent->formula(); }
    
    /**
     * Returns the element the point is in.
     */
    virtual BasicElement* isInside(const QPoint& point, const QPoint& parentOrigin);

    /**
     * Returns our position inside the widget.
     */
    QPoint widgetPos();

    
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
    virtual void calcSizes(ContextStyle& context, int parentSize) = 0;

    /**
     * Draws the whole element including its children.
     * The `parentOrigin' is the point this element's parent starts.
     * We can use our parentPosition to get our own origin then.
     */
    virtual void draw(QPainter& painter, ContextStyle& context,
                      int parentSize, const QPoint& parentOrigin) = 0;

    
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
    virtual void moveLeft(FormulaCursor* cursor, BasicElement* from) = 0;

    /**
     * Enters this element while moving to the right starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or to the right of it.
     */
    virtual void moveRight(FormulaCursor* cursor, BasicElement* from) = 0;

    /**
     * Enters this element while moving up starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or above it.
     */
    virtual void moveUp(FormulaCursor* cursor, BasicElement* from) {}

    /**
     * Enters this element while moving down starting inside
     * the element `from'. Searches for a cursor position inside
     * this element or below it.
     */
    virtual void moveDown(FormulaCursor* cursor, BasicElement* from) {}

    /**
     * Moves the cursor to the first position in this sequence.
     * (That is before the first child.)
     */
    virtual void moveHome(FormulaCursor* cursor) {}

    /**
     * Moves the cursor to the last position in this sequence.
     * (That is behind the last child.)
     */
    virtual void moveEnd(FormulaCursor* cursor) {}

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
    //virtual void removeChild(FormulaCursor* cursor, BasicElement* child) {}

    
    // main child
    //
    // If an element has children one has to become the main one.
    
    virtual SequenceElement* getMainChild() { return 0; }
    virtual void setMainChild(SequenceElement*) {}
    

    // editing
    //
    // Insert and remove children.

    /**
     * Wether we want to insert to the left of the cursor
     * or right of it.
     * The same for deletion.
     */
    enum Direction { beforeCursor, afterCursor };

    /**
     * Inserts all new children at the cursor position. Places the
     * cursor according to the direction.
     *
     * The list will be emptied but stays the property of the caller.
     */
    virtual void insert(FormulaCursor*, QList<BasicElement>&, Direction) {}

    /**
     * Removes all selected children and returns them. Places the
     * cursor to where the children have been.
     */
    virtual void remove(FormulaCursor*, QList<BasicElement>&, Direction) {}

    /**
     * Moves the cursor to a normal place where new elements
     * might be inserted.
     */
    virtual void normalize(FormulaCursor*, Direction) {}

    
    /**
     * Returns wether the element has no more useful
     * children (except its main child) and should therefore
     * be replaced by its main child's content.
     */
    virtual bool isSenseless() { return false; }

    /**
     * Returns the child at the cursor.
     */
    virtual BasicElement* getChild(FormulaCursor*, Direction = beforeCursor) { return 0; }


    /**
     * Sets the cursor to select the child. The mark is placed before,
     * the position behind it.
     */
    virtual void selectChild(FormulaCursor* cursor, BasicElement* child) {}

    /**
     * Moves the cursor inside the main child.
     */
    //virtual void moveToMainChild(FormulaCursor*) {}

    //virtual void selectMainChildContent(FormulaCursor*) {}

    
    // basic support
    
    BasicElement* getParent() { return parent; }
    void setParent(BasicElement* p) { parent = p; }
    
    int getX() const { return position.x(); }
    int getY() const { return position.y(); }

    void setX(int x) { position.setX(x); }
    void setY(int y) { position.setY(y); }

    //QSize getSize() { return size; }

    int getWidth() const { return size.width(); }
    int getHeight() const { return size.height(); }

    int getRelativeSize() const { return relativeSize; }

    /**
     * Our position inside our parent.
     */
    //QPoint getParentPosition() { return position; }

    int getMidline() const { return midline; }


    // debug
    virtual ostream& output(ostream&);
    static int getEvilDestructionCount() { return evilDestructionCount; }
    
protected:

    void setWidth(int width) { size.setWidth(width); }
    void setHeight(int height) { size.setHeight(height); }
    
    void setMidline(int mline) { midline = mline; }
    
private:

    /**
     * Our parent.
     * The parent might not be null except for the FormulaElement
     * that is the top of the element tree.
     */
    BasicElement* parent;

    /**
     * This elements size.
     */
    QSize size;

    /**
     * Our position relative to our parent.
     */
    QPoint position;

    /**
     * The position of our middle line from
     * the upper border.
     */
    int midline;

    /**
     * Our size relative to those of our parent.
     */
    int relativeSize;

    // debug
    static int evilDestructionCount;

    // copying not allowed.
    BasicElement(BasicElement&) {}
    BasicElement& operator= (BasicElement&) { return *this; }
};

#endif // __BASICELEMENT_H
