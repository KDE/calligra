/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.
 
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
#include <qpen.h>

#include "formulacursor.h"
#include "formulaelement.h"
#include "rootelement.h"
#include "sequenceelement.h"


RootElement::RootElement(BasicElement* parent)
    : BasicElement(parent)
{
    content = new SequenceElement(this);
    //index = 0;
    index = new SequenceElement(this);
}

RootElement::~RootElement()
{
    delete index;
    delete content;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void RootElement::calcSizes(ContextStyle& style, int parentSize)
{
    int mySize = parentSize + getRelativeSize();

    content->calcSizes(style, mySize);
    
    int indexWidth = 0;
    int indexHeight = 0;
    if (index != 0) {
        index->calcSizes(style, mySize);
        indexWidth = index->getWidth();
        indexHeight = index->getHeight();
    }
    
    int dist = style.getDistance();
    int unit = (content->getHeight() + dist)/ 3;

    if (index != 0) {
        index->setX(0);
        index->setY(0);
        rootOffset.setX(indexWidth - unit);
        rootOffset.setY(indexHeight - unit);
    }
    else {
        rootOffset.setX(0);
        rootOffset.setY(0);
    }

    content->setX(rootOffset.x() + unit+unit/3);
    content->setY(rootOffset.y() + dist);

    setWidth(content->getWidth() + unit + QMAX(indexWidth-unit/3, unit/3));
    setHeight(QMAX(content->getHeight() + dist, 2*unit + indexHeight));
    setMidline(getHeight() - content->getHeight() + content->getMidline());
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void RootElement::draw(QPainter& painter, ContextStyle& style,
                       int parentSize, const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    int mySize = parentSize + getRelativeSize();

    content->draw(painter, style, mySize, myPos);
    if (index != 0) {
        index->draw(painter, style, mySize, myPos);
    }

    int x = myPos.x() + rootOffset.x();
    int y = myPos.y() + rootOffset.y();
    int dist = style.getDistance();
    int unit = getHeight() / 3;

    painter.setPen(QPen(style.getDefaultColor(), 2));
    painter.drawLine(x+unit/3, y+unit+dist/2,
                     x+unit/2+unit/3, y+getHeight()+1);

    painter.setPen(QPen(style.getDefaultColor(), 1));

    painter.drawLine(x+unit+unit/3, y+3, x+unit/2+unit/3, y+getHeight());
    painter.drawLine(x+unit+unit/3, y+3, x+unit+unit/3+content->getWidth(), y+3);
    painter.drawLine(x+unit/3, y+unit+1, x, y+unit+1+unit/2);
}

/**
 * Enters this element while moving to the left starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the left of it.
 */
void RootElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveLeft(cursor, this);
    }
    else {
        if (from == getParent()) {
            content->moveLeft(cursor, this);
        }
        else if (from == content) {
            if (index != 0) {
                index->moveLeft(cursor, this);
            }
            else {
                getParent()->moveLeft(cursor, this);
            }
        }
        else {
            getParent()->moveLeft(cursor, this);
        }
    }
}

/**
 * Enters this element while moving to the right starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the right of it.
 */
void RootElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveRight(cursor, this);
    }
    else {
        if (from == getParent()) {
            if (index != 0) {
                index->moveRight(cursor, this);
            }
            else {
                content->moveRight(cursor, this);
            }
        }
        else if (from == index) {
            content->moveRight(cursor, this);
        }
        else {
            getParent()->moveRight(cursor, this);
        }
    }
}

/**
 * Enters this element while moving up starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or above it.
 */
void RootElement::moveUp(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveUp(cursor, this);
    }
    else {
        if (from == getParent()) {
            content->moveRight(cursor, this);
        }
        else if (from == content) {
            if (index != 0) {
                index->moveRight(cursor, this);
            }
            else {
                getParent()->moveUp(cursor, this);
            }
        }
        else {
            getParent()->moveUp(cursor, this);
        }
    }
}

/**
 * Enters this element while moving down starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or below it.
 */
void RootElement::moveDown(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveDown(cursor, this);
    }
    else {
        if (from == getParent()) {
            if (index != 0) {
                index->moveRight(cursor, this);
            }
            else {
                content->moveRight(cursor, this);
            }
        }
        else if (from == index) {
            content->moveRight(cursor, this);
        }
        else {
            getParent()->moveUp(cursor, this);
        }
    }
}

/**
 * Reinserts the index if it has been removed.
 */
void RootElement::insert(FormulaCursor* cursor,
                         QList<BasicElement>& newChildren,
                         Direction direction)
{
    if (cursor->getPos() == indexPos) {
        index = static_cast<SequenceElement*>(newChildren.take(0));
        index->setParent(this);

        if (direction == beforeCursor) {
            index->moveLeft(cursor, this);
        }
        else {
            index->moveRight(cursor, this);
        }
        cursor->setSelection(false);
        formula()->changed();
    }
}
    
/**
 * Removes all selected children and returns them. Places the
 * cursor to where the children have been.
 *
 * We remove ourselve if we are requested to remove our content.
 */
void RootElement::remove(FormulaCursor* cursor,
                         QList<BasicElement>& removedChildren,
                         Direction direction)
{
    switch (cursor->getPos()) {
    case contentPos:
        getParent()->selectChild(cursor, this);
        getParent()->remove(cursor, removedChildren, direction);
        break;
    case indexPos:
        removedChildren.append(index);
        formula()->elementRemoval(index);
        index = 0;
        cursor->setTo(this, indexPos);
        formula()->changed();
        break;
    }
}


/**
 * Moves the cursor to a normal place where new elements
 * might be inserted.
 */
void RootElement::normalize(FormulaCursor* cursor, Direction direction)
{
    if (direction == BasicElement::beforeCursor) {
        content->moveLeft(cursor, this);
    }
    else {
        content->moveRight(cursor, this);
    }
}


// main child
//
// If an element has children one has to become the main one.

SequenceElement* RootElement::getMainChild()
{
    return content;
}

void RootElement::setMainChild(SequenceElement* child)
{
    formula()->elementRemoval(content);
    content = child;
    content->setParent(this);
    formula()->changed();
}


/**
 * Sets the cursor to select the child. The mark is placed before,
 * the position behind it.
 */
void RootElement::selectChild(FormulaCursor* cursor, BasicElement* child)
{
    if (child == content) {
        cursor->setTo(this, contentPos);
    }
    else if (child == index) {
        cursor->setTo(this, indexPos);
    }
}
