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

#include <qpainter.h>

#include "formulacursor.h"
#include "formulaelement.h"
#include "indexelement.h"
#include "sequenceelement.h"


FormulaCursor::FormulaCursor(FormulaElement* element)
{
    selectionFlag = false;
    mouseSelectionFlag = false;
    setTo(element, 0);
}

void FormulaCursor::setTo(BasicElement* element, int cursor, int mark)
{
    current = element;
    cursorPos = cursor;
    if ((mark == -1) && selectionFlag) {
        return;
    }
    if (mark != -1) {
        setSelection(true);
    }
    markPos = mark;
}


void FormulaCursor::setPos(int pos)
{
    cursorPos = pos;
}

void FormulaCursor::setMark(int mark)
{
    markPos = mark;
}


void FormulaCursor::draw(QPainter& painter)
{
    // We only draw the cursor if its normalized.
    SequenceElement* sequence = dynamic_cast<SequenceElement*>(current);

    if (sequence != 0) {
        sequence->drawCursor(this, painter);
    }
}


void FormulaCursor::handleSelectState(int flag)
{
    if (flag & SelectMovement) {
        if (!isSelection()) {
            setMark(getPos());
            setSelection(true);
        }
    }
    else {
        setSelection(false);
    }
}

void FormulaCursor::moveLeft(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    if (flag & WordMovement) {
        element->moveHome(this);
    }
    else {
        element->moveLeft(this, element);
    }
}

void FormulaCursor::moveRight(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    if (flag & WordMovement) {
        element->moveEnd(this);
    }
    else {
        element->moveRight(this, element);
    }
}

void FormulaCursor::moveUp(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    element->moveUp(this, element);
}

void FormulaCursor::moveDown(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    element->moveDown(this, element);
}

void FormulaCursor::moveHome(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    element->formula()->moveHome(this);
}

void FormulaCursor::moveEnd(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    element->formula()->moveEnd(this);
}


/**
 * Moves the cursor inside the element. Selection is turned off.
 */
void FormulaCursor::goInsideElement(BasicElement* element)
{
    element->goInside(this);
}


/**
 * Moves the cursor to a normal position. That is somewhere
 * inside a SequenceElement.
 * You need to call this after each removal because the cursor
 * might point to some non existing place.
 */
void FormulaCursor::normalize(BasicElement::Direction direction)
{
    BasicElement* element = getElement();
    element->normalize(this, direction);
}



// BasicElement* FormulaCursor::getElement()
// {
//     if (isInsideParent()) {
//         return current->getParent();
//     }
//     return current;
// }

/**
 * Inserts the child at the current position.
 * Ignores the selection.
 */
void FormulaCursor::insert(BasicElement* child, BasicElement::Direction direction)
{
    QList<BasicElement> list;
    list.append(child);
    insert(list, direction);
}

void FormulaCursor::insert(QList<BasicElement>& children,
                           BasicElement::Direction direction)
{
    BasicElement* element = getElement();
    element->insert(this, children, direction);
}


/**
 * Removes the current selected children and returns them.
 * The cursor needs to be normal (that is be inside a SequenceElement)
 * for this to have any effect.
 */
void FormulaCursor::remove(QList<BasicElement>& children,
                           BasicElement::Direction direction)
{
    SequenceElement* sequence = getNormal();
    if (sequence != 0) {

        // If there is no child to remove in the sequence
        // remove the sequence instead.
        if (sequence->countChildren() == 0) {
            BasicElement* parent = sequence->getParent();
            if (parent != 0) {
                parent->selectChild(this, sequence);
                parent->remove(this, children, direction);
                return;
            }
        }
        else {
            sequence->remove(this, children, direction);
        }
    }
}


/**
 * Replaces the current selection with the supplied element.
 * The replaced elements become the new element's main child's content.
 */
void FormulaCursor::replaceSelectionWith(BasicElement* element,
                                         BasicElement::Direction direction)
{
    QList<BasicElement> list;
    //remove(list, direction);
    getElement()->remove(this, list, direction);
    
    insert(element, direction);
    SequenceElement* mainChild = element->getMainChild();
    if (mainChild != 0) {
        mainChild->goInside(this);
        insert(list);
        BasicElement* parent = element->getParent();
        if (direction == BasicElement::beforeCursor) {
            parent->moveRight(this, element);
        }
        else {
            parent->moveLeft(this, element);
        }
    }
}


/**
 * Replaces the element the cursor points to with its main child's
 * content.
 */
BasicElement* FormulaCursor::replaceByMainChildContent(BasicElement::Direction direction)
{
    QList<BasicElement> childrenList;
    QList<BasicElement> list;
    BasicElement* element = getElement();
    SequenceElement* mainChild = element->getMainChild();
    if ((mainChild != 0) && (mainChild->countChildren() > 0)) {
        mainChild->selectAllChildren(this);
        remove(childrenList);
    }
    element->getParent()->moveRight(this, element);
    setSelection(false);
    remove(list);
    insert(childrenList, direction);
    if (list.count() > 0) {
        return list.take(0);
    }
    return 0;
}


/**
 * Returns wether the element the cursor points to should be replaced.
 * Elements are senseless as soon as they only contain a main child.
 */
bool FormulaCursor::elementIsSenseless()
{
    BasicElement* element = getElement();
    return element->isSenseless();
}


/**
 * Returns the child the cursor points to. Depending on the
 * direction this might be the child before or after the
 * cursor.
 *
 * Might be 0 is there is no such child.
 */
BasicElement* FormulaCursor::getActiveChild(BasicElement::Direction direction)
{
    return getElement()->getChild(this, direction);
}


/**
 * Returns the IndexElement the cursor is on or 0
 * if there is non.
 */
IndexElement* FormulaCursor::getActiveIndexElement()
{
    if (isSelection()) {
        if ((getSelectionEnd() - getSelectionStart()) > 1) {
            return 0;
        }
        BasicElement* child = getActiveChild((getPos() > getMark()) ?
                                             BasicElement::beforeCursor :
                                             BasicElement::afterCursor);
        return dynamic_cast<IndexElement*>(child);
    }
    else {
        BasicElement* child = getActiveChild(BasicElement::beforeCursor);
        IndexElement* element = 0;
        if (child != 0) {
            element = dynamic_cast<IndexElement*>(child);
        }
        if (element == 0) {
            element = dynamic_cast<IndexElement*>(getElement()->getParent());
            if (element != 0) {
                SequenceElement* mainChild = element->getMainChild();
                if ((getElement() != mainChild) ||
                    (mainChild->countChildren() != getPos())) {
                    return 0;
                }
            }
        }
        return element;
    }
}


/**
 * The element is going to leave the formula with and all its children.
 */
void FormulaCursor::elementWillVanish(BasicElement* element)
{
    BasicElement* child = getElement();
    if (child == element->getParent()) {
        child->moveHome(this);
        setSelection(false);
        return;
    }
    while (child != 0) {
        if (child == element) {
            // This is meant to catch all cursors that did not
            // cause the deletion.
            child->getParent()->moveLeft(this, child);
            setSelection(false);
            return;
        }
        child = child->getParent();
    }
}


/**
 * Creates a new CursorData object that describes the cursor.
 * It's up to the caller to delete this object.
 */
FormulaCursor::CursorData* FormulaCursor::getCursorData()
{
    return new CursorData(current, cursorPos, markPos, selectionFlag);
}

/**
 * Sets the cursor to where the CursorData points to. No checking is done
 * so you better make sure the point exists.
 */
void FormulaCursor::setCursorData(FormulaCursor::CursorData* data)
{
    current = data->current;
    cursorPos = data->cursorPos;
    markPos = data->markPos;
    selectionFlag = data->selectionFlag;
}


/**
 * Returns the sequence the cursor is in if we are normal. If not returns 0.
 */
SequenceElement* FormulaCursor::getNormal()
{
    return dynamic_cast<SequenceElement*>(current);
}
