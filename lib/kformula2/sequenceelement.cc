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
#include "sequenceelement.h"


SequenceElement::SequenceElement(BasicElement* parent)
    : BasicElement(parent)
{
    children.setAutoDelete(true);
}


SequenceElement::~SequenceElement()
{
}


/**
 * Returns the element the point is in.
 */
BasicElement* SequenceElement::isInside(const QPoint& point,
                                        const QPoint& parentOrigin)
{
    BasicElement* e = BasicElement::isInside(point, parentOrigin);
    if (e != 0) {
        QPoint myPos(parentOrigin.x() + getX(),
                     parentOrigin.y() + getY());
        
        uint count = children.count();
        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            e = child->isInside(point, myPos);
            if (e != 0) return e;
        }
        return this;
    }
    return 0;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void SequenceElement::calcSizes(ContextStyle& context, int parentSize)
{
    if (children.count() > 0) {
        int width = 0;
        int toMidline = 0;
        int fromMidline = 0;

        uint count = children.count();
        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            child->calcSizes(context, parentSize+getRelativeSize());
            child->setX(width);
            width += child->getWidth();
            toMidline = QMAX(toMidline, child->getMidline());
            fromMidline = QMAX(fromMidline, child->getHeight()-child->getMidline());
        }
        setWidth(width);
        setHeight(toMidline+fromMidline);
        setMidline(toMidline);

        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            child->setY(getMidline() - child->getMidline());
        }
    }
    else {
        setWidth(10);
        setHeight(10);
        setMidline(5);
    }
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void SequenceElement::draw(QPainter& painter, ContextStyle& context,
                           int parentSize, const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x() + getX(),
                 parentOrigin.y() + getY());
    
    if (children.count() > 0) {
        int mySize = parentSize+getRelativeSize();
        uint count = children.count();
        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            child->draw(painter, context, mySize, myPos);

            // Debug
            //painter.setPen(Qt::green)
            //painter.drawRect(x, y, self.width(), self.height())
        }
    }
    else {
        //painter.setPen(Qt::blue);
        painter.drawRect(myPos.x(), myPos.y(), getWidth(), getHeight());
    }
}


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
void SequenceElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    // Our parent asks us for a cursor position. Found.
    if (from == getParent()) {
        cursor->setTo(this, children.count());
    }

    // We already owned the cursor. Ask next child then.
    else if (from == this) {
        if (cursor->pos() > 0) {
            if (cursor->isSelection()) {
                cursor->setTo(this, cursor->pos()-1);
            }
            else {
                children.at(cursor->pos()-1)->moveLeft(cursor, this);
            }
        }
        else {
            // Needed because FormulaElement derives this.
            if (getParent() != 0) {
                getParent()->moveLeft(cursor, this);
            }
        }
    }

    // The cursor came from one of our children or
    // something is wrong.
    else {
        int fromPos = children.find(from);
        cursor->setTo(this, fromPos);
        if (cursor->isSelection()) {
            if (!cursor->isMouseMark()) {
                cursor->setMark(fromPos+1);
            }
        }
    }
}

/**
 * Enters this element while moving to the right starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the right of it.
 */
void SequenceElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    // Our parent asks us for a cursor position. Found.
    if (from == getParent()) {
        cursor->setTo(this, 0);
    }

    // We already owned the cursor. Ask next child then.
    else if (from == this) {
        if (cursor->pos() < children.count()) {
            if (cursor->isSelection()) {
                cursor->setTo(this, cursor->pos()+1);
            }
            else {
                children.at(cursor->pos())->moveRight(cursor, this);
            }
        }
        else {
            // Needed because FormulaElement derives this.
            if (getParent() != 0) {
                getParent()->moveRight(cursor, this);
            }
        }
    }

    // The cursor came from one of our children or
    // something is wrong.
    else {
        int fromPos = children.find(from);
        cursor->setTo(this, fromPos+1);
        if (cursor->isSelection()) {
            if (!cursor->isMouseMark()) {
                cursor->setMark(fromPos);
            }
        }
    }
}

/**
 * Enters this element while moving up starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or above it.
 */
void SequenceElement::moveUp(FormulaCursor* cursor, BasicElement* from)
{
    if (from == getParent()) {
        moveRight(cursor, this);
    }
    else {
        if (getParent() != 0) {
            getParent()->moveUp(cursor, this);
        }
    }
}

/**
 * Enters this element while moving down starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or below it.
 */
void SequenceElement::moveDown(FormulaCursor* cursor, BasicElement* from)
{
    if (from == getParent()) {
        moveRight(cursor, this);
    }
    else {
        if (getParent() != 0) {
            getParent()->moveDown(cursor, this);
        }
    }
}

/**
 * Moves the cursor to the first position in this sequence.
 * (That is before the first child.)
 */
void SequenceElement::moveHome(FormulaCursor* cursor)
{
    if (cursor->isSelection()) {
        BasicElement* element = cursor->getElement();
        if (element != this) {
            while (element->getParent() != this) {
                element = element->getParent();
            }
            cursor->setMark(children.find(element)+1);
        }
    }
    cursor->setTo(this, 0);
}

/**
 * Moves the cursor to the last position in this sequence.
 * (That is behind the last child.)
 */
void SequenceElement::moveEnd(FormulaCursor* cursor)
{
    if (cursor->isSelection()) {
        BasicElement* element = cursor->getElement();
        if (element != this) {
            while (element->getParent() != this) {
                element = element->getParent();
            }
            cursor->setMark(children.find(element));
        }
    }
    cursor->setTo(this, children.count());
}

// children

/**
 * Removes the child. If this was the main child this element might
 * request its own removal.
 * The cursor is the one that caused the removal. It has to be moved
 * to the place any user expects the cursor after that particular
 * element has been removed.
 */
// void SequenceElement::removeChild(FormulaCursor* cursor, BasicElement* child)
// {
//     int pos = children.find(child);
//     formula()->elementRemoval(child, pos);
//     cursor->setTo(this, pos);
//     children.remove(pos);
//     /*
//         if len(self.children) == 0:
//             if self.parent != None:
//                 self.parent.removeChild(cursor, self)
//                 return
//     */
//     formula()->changed();
// }


/**
 * Inserts all new children at the cursor position. Places the
 * cursor according to the direction.
 *
 * The list will be emptied but stays the property of the caller.
 */
void SequenceElement::insert(FormulaCursor* cursor,
                             QList<BasicElement>* newChildren,
                             Direction direction)
{
    int pos = cursor->getPos();
    uint count = newChildren->count();
    for (uint i = 0; i < count; i++) {
        BasicElement* child = newChildren->take(i);
        child->setParent(this);
        children->insert(pos+i, child);
    }
    if (direction == beforeCursor) {
        cursor->setTo(this, pos+count);
    }
    formula()->changed();
}


/**
 * Removes all selected children and returns them. Places the
 * cursor to where the children have been.
 *
 * The ownership of the list is passed to the caller.
 */
QList<BasicElement>* SequenceElement::remove(FormulaCursor* cursor,
                                             Direction direction)
{
    // If there is no child to remove the sequence
    // itself is asked to vanish.
    if (children.count() == 0) {
        if (getParent() != 0) {
            return getParent()->remove(cursor, direction);
        }
    }
    
    QList<BasicElement>* removedChildren = new QList<BasicElement>;

    if (cursor->isSelection()) {
        int from = cursor->getSelectionStart();
        int to = cursor->getSelectionEnd();
        for (int i = from; i < to; i++) {
            removeChild(removedChildren, from);
        }
        cursor->setTo(this, from);
        cursor->setSelection(false);
    }
    else {
        if (direction == beforeCursor) {
            int pos = cursor->getPos() - 1;
            if (pos >= 0) {
                removeChild(removedChildren, pos);
                cursor->setTo(this, pos);
            }
        }
        else {
            uint pos = cursor->getPos();
            if (pos < children.count()) {
                removeChild(removedChildren, pos);
            }
        }
    }
    return removedChildren;
}


/**
 * Removes the children at pos and appends it to the list.
 */
void SequenceElement::removeChild(QList<BasicElement>* removedChildren, int pos)
{
    BasicElement* child = children.at(pos);
    formula()->elementRemoval(child);
    children.remove(pos);
    removedChildren->append(child);
    formula()->changed();
}


// void SequenceElement::replaceElementByMainChild(FormulaCursor* cursor,
//                                                 BasicElement* child)
// {
//     // How to undo this?
// }
