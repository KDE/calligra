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

#ifndef SEQUENCEELEMENT_H
#define SEQUENCEELEMENT_H

#include <qlist.h>
#include <qstring.h>

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
     * Sets the cursor and returns the element the point is in.
     * The handled flag shows whether the cursor has been set.
     * This is needed because only the innermost matching element
     * is allowed to set the cursor.
     */
    virtual BasicElement* goToPos(FormulaCursor*, bool& handled,
                                  const QPoint& point, const QPoint& parentOrigin);

    // drawing
    //
    // Drawing depends on a context which knows the required properties like
    // fonts, spaces and such.
    // It is essential to calculate elements size with the same context
    // before you draw.
    
    /**
     * Tells the sequence to have a smaller size than its parant.
     */
    void setSizeReduction(const ContextStyle& context);

    /**
     * @returns true if there is no visible element in the sequence.
     * Please note that there might be phantom elements.
     */
    bool isEmpty();
    
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
    virtual void draw(QPainter& painter, const QRect& r,
                      const ContextStyle& context,
                      int parentSize, const QPoint& parentOrigin);

    void calcCursorSize(FormulaCursor* cursor, bool smallCursor);
    
    /**
     * If the cursor is inside a sequence it needs to be drawn.
     */
    void drawCursor(FormulaCursor* cursor, QPainter& painter, bool smallCursor);
    
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
     * Moves to the beginning of this word or if we are there already
     * to the beginning of the previous.
     */
    void moveWordLeft(FormulaCursor* cursor);
    
    /**
     * Moves to the end of this word or if we are there already
     * to the end of the next.
     */
    void moveWordRight(FormulaCursor* cursor);

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
     * Moves the cursor to a normal place where new elements
     * might be inserted.
     */
    virtual void normalize(FormulaCursor*, Direction);
    
    /**
     * Returns the child at the cursor.
     * Does not care about the selection.
     */
    virtual BasicElement* getChild(FormulaCursor*, Direction = beforeCursor);

    /**
     * Sets the cursor to select the child. The mark is placed before,
     * the position behind it.
     */
    virtual void selectChild(FormulaCursor* cursor, BasicElement* child);

    /**
     * Selects the current name if there currently is a name.
     * If there is no name and the cursor is at the end of the sequence
     * it moves one step right.
     * Leaves the cursor untouched otherwise.
     * @returns the current name
     */
    QString getCurrentName(FormulaCursor* cursor);
    
    /**
     * Returns the number of children we have.
     */
    int countChildren() const { return children.count(); }

    /**
     * Selects all children. The cursor is put behind, the mark before them.
     */
    void selectAllChildren(FormulaCursor* cursor);
    
    /**
     * Stores the given childrens dom in the element.
     */
    void getChildrenDom(QDomDocument& doc, QDomElement& elem, uint from, uint to);
    
    /**
     * Builds elements from the given node and its siblings and
     * puts them into the list.
     * Returns false if an error occures.
     */
    bool buildChildrenFromDom(QList<BasicElement>& list, QDomNode n);
    
    /**
     * Parses the sequence and generates a new syntax tree.
     * Has to be called after each modification.
     *
     * This is done internally in most cases. Only children that
     * change their TokenType need to call this.
     */
    void parse();

    /**
     * @returns the latex representation of the element and 
     * of the element's children
     */
    virtual QString toLatex();
    

protected:

    //Save/load support
    
    /**
     * Returns the tag name of this element type.
     */
    virtual QString getTagName() const { return "SEQUENCE"; }
    
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
     * Sets the childrens' positions after their size has been
     * calculated.
     *
     * @see #calcSizes
     */
    virtual void setChildrenPositions();

    /**
     * Creates a new element with the given type.
     *
     * @param type the desired type of the element
     */
    virtual BasicElement* createElement(QString type);

    /**
     * @returns the child at position i.
     */
    BasicElement* getChild(uint i) { return children.at(i); }

    /**
     * @returns the position where the child starts.
     *
     * @param child the child's number
     */
    int getChildPosition(uint child);

    
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

    /**
     * the syntax tree of the sequence
     */
    ElementType* parseTree;

    /**
     * Our size relative to those of our parent.
     * That's needed only in the sequence because it is to
     * support smaller indexes and indexes are always a sequence
     * with some content.
     */
    int relativeSize;
};

#endif // SEQUENCEELEMENT_H
