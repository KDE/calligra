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

#include <iostream>
#include <stdlib.h>

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

ostream& SequenceElement::output(ostream& stream)
{
    stream << "SequenceElement {\n";
    BasicElement::output(stream) << endl;
    uint count = children.count();
    for (uint i = 0; i < count; i++) {
        children.at(i)->output(stream);
        stream << '\n';
    }
    stream << '}';
    return stream;
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
            //painter.setPen(Qt::green);
            //painter.drawRect(parentOrigin.x() + getX(), parentOrigin.y() + getY(),
            //                 getWidth(), getHeight());
        }
    }
    else {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::blue);
        painter.drawRect(myPos.x(), myPos.y(), getWidth(), getHeight());
    }
}


/**
 * If the cursor is inside a sequence it needs to be drawn.
 */
void SequenceElement::drawCursor(FormulaCursor* cursor, QPainter& painter)
{
    QPoint point = widgetPos();
    int height = getHeight();

    int posX;
    uint pos = cursor->getPos();
    if (pos < children.count()) {
        posX = children.at(pos)->getX();
    }
    else {
        if (children.count() > 0) {
            posX = getWidth();
        }
        else {
            posX = 2;
        }
    }

    if (cursor->isSelection()) {
        int markX;
        uint mark = cursor->getMark();
        if (mark < children.count()) {
            markX = children.at(mark)->getX();
        }
        else {
            markX = getWidth();
        }
        
        int x = QMIN(posX, markX);
        int width = abs(posX - markX);
        painter.setRasterOp(Qt::XorROP);
        painter.fillRect(point.x()+x, point.y(), width, height, Qt::white);
        painter.setRasterOp(Qt::CopyROP);
    }
    else {
        painter.setRasterOp(Qt::XorROP);
        painter.setPen(Qt::white);
        painter.drawLine(point.x()+posX, point.y()-2,
                         point.x()+posX, point.y()+height+2);
        painter.drawLine(point.x(), point.y()+height+3,
                         point.x()+getWidth(), point.y()+height+3);
        painter.setRasterOp(Qt::CopyROP);
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
        if (cursor->getPos() > 0) {
            if (cursor->isSelectionMode()) {
                cursor->setTo(this, cursor->getPos()-1);
            }
            else {
                children.at(cursor->getPos()-1)->moveLeft(cursor, this);
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
        if (cursor->isSelectionMode()) {
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
        uint pos = cursor->getPos();
        if (pos < children.count()) {
            if (cursor->isSelectionMode()) {
                cursor->setTo(this, pos+1);
            }
            else {
                children.at(pos)->moveRight(cursor, this);
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
        if (cursor->isSelectionMode()) {
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
    if (cursor->isSelectionMode()) {
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
    if (cursor->isSelectionMode()) {
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

/**
 * Sets the cursor inside this element to its start position.
 * For most elements that is the main child.
 */
void SequenceElement::goInside(FormulaCursor* cursor)
{
    cursor->setSelection(false);
    cursor->setTo(this, 0);
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
 * cursor according to the direction. The inserted elements will
 * be selected.
 *
 * The list will be emptied but stays the property of the caller.
 */
void SequenceElement::insert(FormulaCursor* cursor,
                             QList<BasicElement>& newChildren,
                             Direction direction)
{
    int pos = cursor->getPos();
    uint count = newChildren.count();
    for (uint i = 0; i < count; i++) {
        BasicElement* child = newChildren.take(0);
        child->setParent(this);
        children.insert(pos+i, child);
    }
    if (direction == beforeCursor) {
        cursor->setTo(this, pos+count, pos);
    }
    else {
        cursor->setTo(this, pos, pos+count);
    }
    formula()->changed();
}


/**
 * Removes all selected children and returns them. Places the
 * cursor to where the children have been.
 *
 * The ownership of the list is passed to the caller.
 */
void SequenceElement::remove(FormulaCursor* cursor,
                             QList<BasicElement>& removedChildren,
                             Direction direction)
{
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
                // It is necessary to set the cursor to its old
                // position because it got a notification and
                // moved to the beginning of this sequence.
                cursor->setTo(this, pos);
            }
        }
    }
}


/**
 * Removes the children at pos and appends it to the list.
 */
void SequenceElement::removeChild(QList<BasicElement>& removedChildren, int pos)
{
    BasicElement* child = children.at(pos);
    formula()->elementRemoval(child);
    children.take(pos);
    removedChildren.append(child);
    //cerr << *removedChildren.at(0) << endl;
    formula()->changed();
}


/**
 * Moves the cursor to a normal place where new elements
 * might be inserted.
 */
void SequenceElement::normalize(FormulaCursor* cursor, Direction)
{
    cursor->setSelection(false);
}


/**
 * Returns the child at the cursor.
 * Does not care about the selection.
 */
BasicElement* SequenceElement::getChild(FormulaCursor* cursor, Direction direction)
{
    if (direction == beforeCursor) {
        int pos = cursor->getPos()-1;
        if (pos >= 0) {
            return children.at(pos);
        }
    }
    else {
        uint pos = cursor->getPos();
        if (pos < children.count()) {
            return children.at(pos);
        }
    }
    return 0;
}


/**
 * Sets the cursor to select the child. The mark is placed before,
 * the position behind it.
 */
void SequenceElement::selectChild(FormulaCursor* cursor, BasicElement* child)
{
    int pos = children.find(child);
    if (pos > -1) {
        cursor->setTo(this, pos+1, pos);
    }
}


/**
 * Selects all children. The cursor is put behind, the mark before them.
 */
void SequenceElement::selectAllChildren(FormulaCursor* cursor)
{
    cursor->setTo(this, children.count(), 0);
}


QDomElement SequenceElement::getElementDom(QDomDocument *doc)
{
    QDomElement de=doc->createElement("SEQUENCE");
    int sz=getRelativeSize();
    if(sz!=0) {
         de.setAttribute("SIZE",sz);
    }

    uint count = children.count();
    for (uint i = 0; i < count; i++) {
        QDomElement tmpEleDom=children.at(i)->getElementDom(doc);
	de.appendChild(tmpEleDom);
    }
    return de;
}




