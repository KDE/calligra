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

#ifndef __FORMULACURSOR_H
#define __FORMULACURSOR_H

#include "basicelement.h"

class FormulaElement;
class IndexElement;
class QKeyEvent;


/**
 * The selection. This might be a one position selection or
 * an area. Each view will need one FormulaCursor.
 *
 * The FormulaContainer always uses the cursor to operate on
 * the element tree.
 *
 * Note that it is up to the elements to actually move the cursor.
 * (The cursor has no chance to know how.)
 */
class FormulaCursor {
    friend class SequenceElement;
    
public:

    /**
     * Creates a cursor and puts is at the beginning
     * of the rootElement.
     */
    FormulaCursor(FormulaElement* element);

    // where the cursor and the mark are
    int getPos() const { return cursorPos; }
    int getMark() const { return markPos; }

    
    /**
     * Returns wether we are in selection mode.
     */
    bool isSelectionMode() const { return selectionFlag; }

    /**
     * Returns wether there actually is a selection.
     */
    bool isSelection() const { return selectionFlag && (getPos() != getMark()); }

    /**
     * Sets the selection mode.
     */
    void setSelection(bool selection) { selectionFlag = selection; }

    /**
     * To be done later...
     */
    bool isMouseMark() const { return mouseSelectionFlag; }


    /**
     * Draws the cursor at its current position.
     * The cursor will always be drawn in xor mode.
     */
    void draw(QPainter&);

    
    // simple cursor movement.
    
    void moveLeft(QKeyEvent* event);
    void moveRight(QKeyEvent* event);
    void moveUp(QKeyEvent* event);
    void moveDown(QKeyEvent* event);

    void moveHome(QKeyEvent* event);
    void moveEnd(QKeyEvent* event);

    /**
     * Moves the cursor inside the element. Selection is turned off.
     */
    void goInsideElement(BasicElement* element);
    
    
    /**
     * Inserts the child at the current position.
     * Ignores the selection.
     */
    void insert(BasicElement*, BasicElement::Direction = BasicElement::beforeCursor);
        
    /**
     * Inserts the listed children at the current position.
     * Ignores the selection.
     * The list will be emptied.
     */
    void insert(QList<BasicElement>&,
                BasicElement::Direction = BasicElement::beforeCursor);

    /**
     * Removes the current selected children and returns them.
     */
    void remove(QList<BasicElement>&,
                BasicElement::Direction = BasicElement::beforeCursor);


    /**
     * Replaces the current selection with the supplied element.
     * The replaced elements become the new element's main child's content.
     */
    void replaceSelectionWith(BasicElement*,
                              BasicElement::Direction = BasicElement::beforeCursor);

    /**
     * Replaces the element the cursor points to with its main child's
     * content.
     */
    BasicElement* replaceByMainChildContent(BasicElement::Direction = BasicElement::beforeCursor);


    /**
     * Returns wether the element the cursor points to should be replaced.
     * Elements are senseless as soon as they only contain a main child.
     */
    bool elementIsSenseless();
    
    
    // The range that is selected. Makes no sense if there is
    // no selection.

    int getSelectionStart() const { return QMIN(getPos(), getMark()); }
    int getSelectionEnd() const { return QMAX(getPos(), getMark()); }
    

    /**
     * Sets the cursor to a new position.
     * This gets called from the element that wants
     * to own the cursor. It is a mistake to call this if you aren't
     * an element.
     *
     * If you provide a mark >= 0 the selection gets turned on.
     * If there is a selection and you don't provide a mark the
     * current mark won't change.
     */
    void setTo(BasicElement* element, int cursor, int mark=-1);


    /**
     * The element we are in. In most cases this is a SequenceElement.
     * There is no way to place a cursor outside a SequenceElement by
     * normal movement.
     * But in special cases (e.g. if you remove an index from an
     * IndexElement) the cursor can be placed to odd places. This is
     * the reason why you have to normalize the cursor after each
     * removal.
     */
    BasicElement* getElement() { return current; }

    /**
     * Returns the child the cursor points to. Depending on the
     * direction this might be the child before or after the
     * cursor.
     *
     * Might be 0 is there is no such child.
     */
    BasicElement* getActiveChild(BasicElement::Direction direction);

    
    /**
     * Moves the cursor to a normal position. That is somewhere
     * inside a SequenceElement.
     * You need to call this after each removal because the cursor
     * might point to some non existing place.
     */
    void normalize(BasicElement::Direction direction = BasicElement::beforeCursor);

    
    /**
     * Returns the IndexElement the cursor is on or 0
     * if there is non.
     */
    IndexElement* getActiveIndexElement();
    
private:

    /**
     * Returns wether we are inside the current sequence's parent
     * rather then inside the sequence itself.
     */
    //bool isInsideParent() const { return current->isEmpty() &&
    //                                  (current->getParent() != 0); }
    
    void setPos(int pos);
    void setMark(int mark);

    /**
     * Sets the selection according to the shift key.
     */
    void handleShiftState(int state);

    
    /**
     * The element the cursor is inside right now.
     */
    BasicElement* current;

    /**
     * The position the cursor in on inside the element.
     * Might be anything from 0 to current->children->size().
     *
     * This is where new elements are put in.
     */
    int cursorPos;

    /**
     * The position of the mark. If we are in selection mode this
     * is the other side of the selected area.
     * Note that the mark always belongs to the same SequenceElement
     * as the cursor.
     */
    int markPos;

    /**
     * Tells wether there is a selection area.
     * (This is not equal to (markPos != -1).)
     */
    bool selectionFlag;

    /**
     * Tells wether we are selecting with the mouse at the moment.
     * (Do we really need this?)
     */
    bool mouseSelectionFlag;
};


#endif // __FORMULACURSOR_H
