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

#include <qpainter.h>

#include "indexelement.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "sequenceelement.h"

using namespace std;


IndexElement::IndexElement(BasicElement* parent)
    : BasicElement(parent)
{
    content = new SequenceElement(this);

    upperLeft   = 0;
    upperMiddle = 0;
    upperRight  = 0;
    lowerLeft   = 0;
    lowerMiddle = 0;
    lowerRight  = 0;
}

IndexElement::~IndexElement()
{
    delete content;
    delete upperLeft;
    delete upperMiddle;
    delete upperRight;
    delete lowerLeft;
    delete lowerMiddle;
    delete lowerRight;
}


/**
 * Returns the element the point is in.
 */
BasicElement* IndexElement::goToPos(FormulaCursor* cursor, bool& handled,
                                    const QPoint& point, const QPoint& parentOrigin)
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
        e = content->goToPos(cursor, handled, point, myPos);
        if (e != 0) return e;

        if (hasUpperRight()) {
            e = upperRight->goToPos(cursor, handled, point, myPos);
            if (e != 0) return e;
        }
        if (hasUpperMiddle()) {
            e = upperMiddle->goToPos(cursor, handled, point, myPos);
            if (e != 0) return e;
        }
        if (hasUpperLeft()) {
            e = upperLeft->goToPos(cursor, handled, point, myPos);
            if (e != 0) return e;
        }
        if (hasLowerRight()) {
            e = lowerRight->goToPos(cursor, handled, point, myPos);
            if (e != 0) return e;
        }
        if (hasLowerMiddle()) {
            e = lowerMiddle->goToPos(cursor, handled, point, myPos);
            if (e != 0) return e;
        }
        if (hasLowerLeft()) {
            e = lowerLeft->goToPos(cursor, handled, point, myPos);
            if (e != 0) return e;
        }

        int dx = point.x() - myPos.x();
        int dy = point.y() - myPos.y();

        // the positions after the left indexes
        if (dx < content->getX()+content->getWidth()) {
            if (dy < content->getY()) {
                if (hasUpperMiddle() && (dx > upperMiddle->getX())) {
                    upperMiddle->moveLeft(cursor, this);
                    handled = true;
                    return upperMiddle;
                }
                if (hasUpperLeft() && (dx > upperLeft->getX())) {
                    upperLeft->moveLeft(cursor, this);
                    handled = true;
                    return upperLeft;
                }
            }
            else if (dy > content->getY()+content->getHeight()) {
                if (hasLowerMiddle() && (dx > lowerMiddle->getX())) {
                    lowerMiddle->moveLeft(cursor, this);
                    handled = true;
                    return lowerMiddle;
                }
                if (hasLowerLeft() && (dx > lowerLeft->getX())) {
                    lowerLeft->moveLeft(cursor, this);
                    handled = true;
                    return lowerLeft;
                }
            }
        }
        // the positions after the left indexes
        else {
            if (dy < content->getY()) {
                if (hasUpperRight()) {
                    upperRight->moveLeft(cursor, this);
                    handled = true;
                    return upperRight;
                }
            }
            else if (dy > content->getY()+content->getHeight()) {
                if (hasLowerRight()) {
                    lowerRight->moveLeft(cursor, this);
                    handled = true;
                    return lowerRight;
                }
            }
            else {
                content->moveLeft(cursor, this);
                handled = true;
                return content;
            }
        }

        return this;
    }
    return 0;
}


// drawing
//
// Drawing depends on a context which knows the required properties like
// fonts, spaces and such.
// It is essential to calculate elements size with the same context
// before you draw.


void IndexElement::setMiddleX(int xOffset, int middleWidth)
{
    content->setX(xOffset + (middleWidth - content->getWidth()) / 2);
    if (hasUpperMiddle()) {
        upperMiddle->setX(xOffset + (middleWidth - upperMiddle->getWidth()) / 2);
    }
    if (hasLowerMiddle()) {
        lowerMiddle->setX(xOffset + (middleWidth - lowerMiddle->getWidth()) / 2);
    }
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void IndexElement::calcSizes(const ContextStyle& contextStyle, int parentSize)
{
    int mySize = parentSize;
    //int distX = contextStyle.getDistanceX(mySize);
    int distY = contextStyle.getDistanceY(mySize);

    // get the indexes size
    int ulWidth = 0, ulHeight = 0, ulMidline = 0;
    if (hasUpperLeft()) {
        upperLeft->setSizeReduction(contextStyle);
        upperLeft->calcSizes(contextStyle, mySize);
        ulWidth = upperLeft->getWidth();
        ulHeight = upperLeft->getHeight();
        ulMidline = upperLeft->getMidline();
    }

    int umWidth = 0, umHeight = 0, umMidline = 0;
    if (hasUpperMiddle()) {
        upperMiddle->setSizeReduction(contextStyle);
        upperMiddle->calcSizes(contextStyle, mySize);
        umWidth = upperMiddle->getWidth();
        umHeight = upperMiddle->getHeight() + distY;
        umMidline = upperMiddle->getMidline();
    }

    int urWidth = 0, urHeight = 0, urMidline = 0;
    if (hasUpperRight()) {
        upperRight->setSizeReduction(contextStyle);
        upperRight->calcSizes(contextStyle, mySize);
        urWidth = upperRight->getWidth();
        urHeight = upperRight->getHeight();
        urMidline = upperRight->getMidline();
    }

    int llWidth = 0, llHeight = 0, llMidline = 0;
    if (hasLowerLeft()) {
        lowerLeft->setSizeReduction(contextStyle);
        lowerLeft->calcSizes(contextStyle, mySize);
        llWidth = lowerLeft->getWidth();
        llHeight = lowerLeft->getHeight();
        llMidline = lowerLeft->getMidline();
    }

    int lmWidth = 0, lmHeight = 0, lmMidline = 0;
    if (hasLowerMiddle()) {
        lowerMiddle->setSizeReduction(contextStyle);
        lowerMiddle->calcSizes(contextStyle, mySize);
        lmWidth = lowerMiddle->getWidth();
        lmHeight = lowerMiddle->getHeight() + distY;
        lmMidline = lowerMiddle->getMidline();
    }

    int lrWidth = 0, lrHeight = 0, lrMidline = 0;
    if (hasLowerRight()) {
        lowerRight->setSizeReduction(contextStyle);
        lowerRight->calcSizes(contextStyle, mySize);
        lrWidth = lowerRight->getWidth();
        lrHeight = lowerRight->getHeight();
        lrMidline = lowerRight->getMidline();
    }

    // get the contents size
    content->calcSizes(contextStyle, mySize);
    int width = QMAX(content->getWidth(), QMAX(umWidth, lmWidth));
    int toMidline = content->getMidline();
    int fromMidline = content->getHeight() - toMidline;

    // calculate the x offsets
    if (ulWidth > llWidth) {
        upperLeft->setX(0);
        if (hasLowerLeft()) {
            lowerLeft->setX(ulWidth - llWidth);
        }
        setMiddleX(ulWidth, width);
        width += ulWidth;
    }
    else {
        if (hasUpperLeft()) {
            upperLeft->setX(llWidth - ulWidth);
        }
        if (hasLowerLeft()) {
            lowerLeft->setX(0);
        }
        setMiddleX(llWidth, width);
        width += llWidth;
    }

    if (hasUpperRight()) {
        upperRight->setX(width);
    }
    if (hasLowerRight()) {
        lowerRight->setX(width);
    }
    width += QMAX(urWidth, lrWidth);

    // calculate the y offsets
    // the upper half
    int ulOffset = (ulHeight-ulMidline) < content->getMidline() ? ulMidline : ulHeight-content->getMidline();
    int urOffset = (urHeight-urMidline) < content->getMidline() ? urMidline : urHeight-content->getMidline();
    int height = QMAX(umHeight, QMAX(ulOffset, urOffset));

    content->setY(height);
    toMidline += height;
    if (hasUpperLeft()) {
        upperLeft->setY(height-ulOffset);
    }
    if (hasUpperMiddle()) {
        upperMiddle->setY(height-umHeight);
    }
    if (hasUpperRight()) {
        upperRight->setY(height-urOffset);
    }

    int llOffset = llMidline < (content->getHeight() - content->getMidline()) ? (content->getHeight() - llMidline) : content->getHeight() - content->getMidline();
    int lrOffset = lrMidline < (content->getHeight() - content->getMidline()) ? (content->getHeight() - lrMidline) : content->getHeight() - content->getMidline();
    if (hasLowerLeft()) {
        lowerLeft->setY(height+llOffset);
    }
    if (hasLowerMiddle()) {
        lowerMiddle->setY(height+content->getHeight()+distY);
    }
    if (hasLowerRight()) {
        lowerRight->setY(height+lrOffset);
    }
    fromMidline += QMAX(QMAX(llHeight+llOffset, lrHeight+lrOffset) - content->getHeight(), lmHeight);

    // set the result
    setWidth(width);
    setHeight(toMidline+fromMidline);
    setMidline(toMidline);
    calcBaseline();
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void IndexElement::draw(QPainter& painter, const QRect& r,
                        const ContextStyle& contextStyle,
                        int parentSize, const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    int mySize = parentSize;
    if (!QRect(myPos, getSize()).intersects(r))
        return;

    content->draw(painter, r, contextStyle, mySize, myPos);
    if (hasUpperLeft()) {
        upperLeft->draw(painter, r, contextStyle, mySize, myPos);
    }
    if (hasUpperMiddle()) {
        upperMiddle->draw(painter, r, contextStyle, mySize, myPos);
    }
    if (hasUpperRight()) {
        upperRight->draw(painter, r, contextStyle, mySize, myPos);
    }
    if (hasLowerLeft()) {
        lowerLeft->draw(painter, r, contextStyle, mySize, myPos);
    }
    if (hasLowerMiddle()) {
        lowerMiddle->draw(painter, r, contextStyle, mySize, myPos);
    }
    if (hasLowerRight()) {
        lowerRight->draw(painter, r, contextStyle, mySize, myPos);
    }

    // Debug
    //painter.setBrush(Qt::NoBrush);
    //painter.setPen(Qt::red);
    //painter.drawRect(myPos.x(), myPos.y(), getWidth(), getHeight());
    //painter.drawLine(myPos.x(), myPos.y()+getMidline(),
    //                 myPos.x()+getWidth(), myPos.y()+getMidline());
}


// navigation
//
// The elements are responsible to handle cursor movement themselves.
// To do this they need to know the direction the cursor moves and
// the element it comes from.
//
// The cursor might be in normal or in selection mode.

int IndexElement::getFromPos(BasicElement* from)
{
    if (from == lowerRight) {
        return lowerRightPos;
    }
    else if (from == upperRight) {
        return upperRightPos;
    }
    else if (from == lowerMiddle) {
        return lowerMiddlePos;
    }
    else if (from == content) {
        return contentPos;
    }
    else if (from == upperMiddle) {
        return upperMiddlePos;
    }
    else if (from == lowerLeft) {
        return lowerLeftPos;
    }
    else if (from == upperLeft) {
        return upperLeftPos;
    }
    return parentPos;
}

/**
 * Enters this element while moving to the left starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the left of it.
 */
void IndexElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveLeft(cursor, this);
    }
    else {
        bool linear = cursor->getLinearMovement();
        int fromPos = getFromPos(from);
        if (!linear) {
            if ((fromPos == lowerRightPos) && hasLowerMiddle()) {
                lowerMiddle->moveLeft(cursor, this);
                return;
            }
            else if ((fromPos == upperRightPos) && hasUpperMiddle()) {
                upperMiddle->moveLeft(cursor, this);
                return;
            }
            else if ((fromPos == lowerMiddlePos) && hasLowerLeft()) {
                lowerLeft->moveLeft(cursor, this);
                return;
            }
            else if ((fromPos == upperMiddlePos) && hasUpperLeft()) {
                upperLeft->moveLeft(cursor, this);
                return;
            }
        }
        switch (fromPos) {
            case parentPos:
                if (hasLowerRight() && linear) {
                    lowerRight->moveLeft(cursor, this);
                    break;
                }
            case lowerRightPos:
                if (hasUpperRight() && linear) {
                    upperRight->moveLeft(cursor, this);
                    break;
                }
            case upperRightPos:
                if (hasLowerMiddle() && linear) {
                    lowerMiddle->moveLeft(cursor, this);
                    break;
                }
            case lowerMiddlePos:
                content->moveLeft(cursor, this);
                break;
            case contentPos:
                if (hasUpperMiddle() && linear) {
                    upperMiddle->moveLeft(cursor, this);
                    break;
                }
            case upperMiddlePos:
                if (hasLowerLeft() && linear) {
                    lowerLeft->moveLeft(cursor, this);
                    break;
                }
            case lowerLeftPos:
                if (hasUpperLeft() && linear) {
                    upperLeft->moveLeft(cursor, this);
                    break;
                }
            case upperLeftPos:
                getParent()->moveLeft(cursor, this);
        }
    }
}

/**
 * Enters this element while moving to the right starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the right of it.
 */
void IndexElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveRight(cursor, this);
    }
    else {
        bool linear = cursor->getLinearMovement();
        int fromPos = getFromPos(from);
        if (!linear) {
            if ((fromPos == lowerLeftPos) && hasLowerMiddle()) {
                lowerMiddle->moveRight(cursor, this);
                return;
            }
            else if ((fromPos == upperLeftPos) && hasUpperMiddle()) {
                upperMiddle->moveRight(cursor, this);
                return;
            }
            else if ((fromPos == lowerMiddlePos) && hasLowerLeft()) {
                lowerRight->moveRight(cursor, this);
                return;
            }
            else if ((fromPos == upperMiddlePos) && hasUpperLeft()) {
                upperRight->moveRight(cursor, this);
                return;
            }
        }
        switch (fromPos) {
            case parentPos:
                if (hasUpperLeft() && linear) {
                    upperLeft->moveRight(cursor, this);
                    break;
                }
            case upperLeftPos:
                if (hasLowerLeft() && linear) {
                    lowerLeft->moveRight(cursor, this);
                    break;
                }
            case lowerLeftPos:
                if (hasUpperMiddle() && linear) {
                    upperMiddle->moveRight(cursor, this);
                    break;
                }
            case upperMiddlePos:
                content->moveRight(cursor, this);
                break;
            case contentPos:
                if (hasLowerMiddle() && linear) {
                    lowerMiddle->moveRight(cursor, this);
                    break;
                }
            case lowerMiddlePos:
                if (hasUpperRight() && linear) {
                    upperRight->moveRight(cursor, this);
                    break;
                }
            case upperRightPos:
                if (hasLowerRight() && linear) {
                    lowerRight->moveRight(cursor, this);
                    break;
                }
            case lowerRightPos:
                getParent()->moveRight(cursor, this);
        }
    }
}

/**
 * Enters this element while moving up starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or above it.
 */
void IndexElement::moveUp(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveUp(cursor, this);
    }
    else {
        if (from == content) {
            if ((cursor->getPos() == 0) && (cursor->getElement() == from)) {
                if (hasUpperLeft()) {
                    upperLeft->moveLeft(cursor, this);
                    return;
                }
                else if (hasUpperMiddle()) {
                    upperMiddle->moveRight(cursor, this);
                    return;
                }
            }
            if (hasUpperRight()) {
                upperRight->moveRight(cursor, this);
            }
            else if (hasUpperMiddle()) {
                upperMiddle->moveLeft(cursor, this);
            }
            else if (hasUpperLeft()) {
                upperLeft->moveLeft(cursor, this);
            }
            else {
                getParent()->moveUp(cursor, this);
            }
        }
        else if ((from == upperLeft) || (from == upperMiddle) || (from == upperRight)) {
            getParent()->moveUp(cursor, this);
        }
        else if ((from == getParent()) || (from == lowerLeft) || (from == lowerMiddle)) {
            content->moveRight(cursor, this);
        }
        else if (from == lowerRight) {
            content->moveLeft(cursor, this);
        }
    }
}

/**
 * Enters this element while moving down starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or below it.
 */
void IndexElement::moveDown(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveDown(cursor, this);
    }
    else {
        if (from == content) {
            if ((cursor->getPos() == 0) && (cursor->getElement() == from)) {
                if (hasLowerLeft()) {
                    lowerLeft->moveLeft(cursor, this);
                    return;
                }
                else if (hasLowerMiddle()) {
                    lowerMiddle->moveRight(cursor, this);
                    return;
                }
            }
            if (hasLowerRight()) {
                lowerRight->moveRight(cursor, this);
            }
            else if (hasLowerMiddle()) {
                lowerMiddle->moveLeft(cursor, this);
            }
            else if (hasLowerLeft()) {
                lowerLeft->moveLeft(cursor, this);
            }
            else {
                getParent()->moveDown(cursor, this);
            }
        }
        else if ((from == lowerLeft) || (from == lowerMiddle) || (from == lowerRight)) {
            getParent()->moveDown(cursor, this);
        }
        else if ((from == getParent()) || (from == upperLeft) || (from == upperMiddle)) {
            content->moveRight(cursor, this);
        }
        if (from == upperRight) {
            content->moveLeft(cursor, this);
        }
    }
}


// children


// main child
//
// If an element has children one has to become the main one.

// void IndexElement::setMainChild(SequenceElement* child)
// {
//     formula()->elementRemoval(content);
//     content = child;
//     content->setParent(this);
//     formula()->changed();
// }


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
void IndexElement::insert(FormulaCursor* cursor,
                          QList<BasicElement>& newChildren,
                          Direction direction)
{
    SequenceElement* index = static_cast<SequenceElement*>(newChildren.take(0));
    index->setParent(this);

    switch (cursor->getPos()) {
    case upperLeftPos:
        upperLeft = index;
        break;
    case lowerLeftPos:
        lowerLeft = index;
        break;
    case upperMiddlePos:
        upperMiddle = index;
        break;
    case lowerMiddlePos:
        lowerMiddle = index;
        break;
    case upperRightPos:
        upperRight = index;
        break;
    case lowerRightPos:
        lowerRight = index;
        break;
    default:
        // this is an error!
        return;
    }

    if (direction == beforeCursor) {
        index->moveLeft(cursor, this);
    }
    else {
        index->moveRight(cursor, this);
    }
    cursor->setSelection(false);
    formula()->changed();
}


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
void IndexElement::remove(FormulaCursor* cursor,
                          QList<BasicElement>& removedChildren,
                          Direction direction)
{
    int pos = cursor->getPos();
    switch (pos) {
    case upperLeftPos:
        removedChildren.append(upperLeft);
        formula()->elementRemoval(upperLeft);
        upperLeft = 0;
        setToUpperLeft(cursor);
        break;
    case lowerLeftPos:
        removedChildren.append(lowerLeft);
        formula()->elementRemoval(lowerLeft);
        lowerLeft = 0;
        setToLowerLeft(cursor);
        break;
    case contentPos: {
        BasicElement* parent = getParent();
        parent->selectChild(cursor, this);
        parent->remove(cursor, removedChildren, direction);
        break;
    }
    case upperMiddlePos:
        removedChildren.append(upperMiddle);
        formula()->elementRemoval(upperMiddle);
        upperMiddle = 0;
        setToUpperMiddle(cursor);
        break;
    case lowerMiddlePos:
        removedChildren.append(lowerMiddle);
        formula()->elementRemoval(lowerMiddle);
        lowerMiddle = 0;
        setToLowerMiddle(cursor);
        break;
    case upperRightPos:
        removedChildren.append(upperRight);
        formula()->elementRemoval(upperRight);
        upperRight = 0;
        setToUpperRight(cursor);
        break;
    case lowerRightPos:
        removedChildren.append(lowerRight);
        formula()->elementRemoval(lowerRight);
        lowerRight = 0;
        setToLowerRight(cursor);
        break;
    }
    formula()->changed();
}

/**
 * Moves the cursor to a normal place where new elements
 * might be inserted.
 */
void IndexElement::normalize(FormulaCursor* cursor, BasicElement::Direction direction)
{
    if (direction == BasicElement::beforeCursor) {
        content->moveLeft(cursor, this);
    }
    else {
        content->moveRight(cursor, this);
    }
}

/**
 * Returns wether the element has no more useful
 * children (except its main child) and should therefore
 * be replaced by its main child's content.
 */
bool IndexElement::isSenseless()
{
    return !hasUpperLeft() && !hasUpperRight() && !hasUpperMiddle() &&
        !hasLowerLeft() && !hasLowerRight() && !hasLowerMiddle();
}


/**
 * Returns the child at the cursor.
 */
BasicElement* IndexElement::getChild(FormulaCursor* cursor, Direction)
{
    int pos = cursor->getPos();
    /*
      It makes no sense to care for the direction.
    if (direction == beforeCursor) {
        pos -= 1;
    }
    */
    switch (pos) {
    case contentPos:
        return content;
    case upperLeftPos:
        return upperLeft;
    case lowerLeftPos:
        return lowerLeft;
    case upperMiddlePos:
        return upperMiddle;
    case lowerMiddlePos:
        return lowerMiddle;
    case upperRightPos:
        return upperRight;
    case lowerRightPos:
        return lowerRight;
    }
    return 0;
}


/**
 * Sets the cursor to select the child. The mark is placed before,
 * the position behind it.
 */
void IndexElement::selectChild(FormulaCursor* cursor, BasicElement* child)
{
    if (child == content) {
        setToContent(cursor);
    }
    else if (child == upperLeft) {
        setToUpperLeft(cursor);
    }
    else if (child == lowerLeft) {
        setToLowerLeft(cursor);
    }
    else if (child == upperMiddle) {
        setToUpperMiddle(cursor);
    }
    else if (child == lowerMiddle) {
        setToLowerMiddle(cursor);
    }
    else if (child == upperRight) {
        setToUpperRight(cursor);
    }
    else if (child == lowerRight) {
        setToLowerRight(cursor);
    }
}


/**
 * Sets the cursor to point to the place where the content is.
 * There always is a content so this is not a useful place.
 * No insertion or removal will succeed as long as the cursor is
 * there.
 */
void IndexElement::setToContent(FormulaCursor* cursor)
{
    cursor->setTo(this, contentPos);
}

// point the cursor to a gap where an index is to be inserted.
// this makes no sense if there is such an index already.

void IndexElement::setToUpperLeft(FormulaCursor* cursor)
{
    cursor->setTo(this, upperLeftPos);
}

void IndexElement::setToUpperMiddle(FormulaCursor* cursor)
{
    cursor->setTo(this, upperMiddlePos);
}

void IndexElement::setToUpperRight(FormulaCursor* cursor)
{
    cursor->setTo(this, upperRightPos);
}

void IndexElement::setToLowerLeft(FormulaCursor* cursor)
{
    cursor->setTo(this, lowerLeftPos);
}

void IndexElement::setToLowerMiddle(FormulaCursor* cursor)
{
    cursor->setTo(this, lowerMiddlePos);
}

void IndexElement::setToLowerRight(FormulaCursor* cursor)
{
    cursor->setTo(this, lowerRightPos);
}


// move inside an index that exists already.

void IndexElement::moveToUpperLeft(FormulaCursor* cursor, Direction direction)
{
    if (hasUpperLeft()) {
        if (direction == beforeCursor) {
            upperLeft->moveLeft(cursor, this);
        }
        else {
            upperLeft->moveRight(cursor, this);
        }
    }
}

void IndexElement::moveToUpperMiddle(FormulaCursor* cursor, Direction direction)
{
    if (hasUpperMiddle()) {
        if (direction == beforeCursor) {
            upperMiddle->moveLeft(cursor, this);
        }
        else {
            upperMiddle->moveRight(cursor, this);
        }
    }
}

void IndexElement::moveToUpperRight(FormulaCursor* cursor, Direction direction)
{
    if (hasUpperRight()) {
        if (direction == beforeCursor) {
            upperRight->moveLeft(cursor, this);
        }
        else {
            upperRight->moveRight(cursor, this);
        }
    }
}

void IndexElement::moveToLowerLeft(FormulaCursor* cursor, Direction direction)
{
    if (hasLowerLeft()) {
        if (direction == beforeCursor) {
            lowerLeft->moveLeft(cursor, this);
        }
        else {
            lowerLeft->moveRight(cursor, this);
        }
    }
}

void IndexElement::moveToLowerMiddle(FormulaCursor* cursor, Direction direction)
{
    if (hasLowerMiddle()) {
        if (direction == beforeCursor) {
            lowerMiddle->moveLeft(cursor, this);
        }
        else {
            lowerMiddle->moveRight(cursor, this);
        }
    }
}

void IndexElement::moveToLowerRight(FormulaCursor* cursor, Direction direction)
{
    if (hasLowerRight()) {
        if (direction == beforeCursor) {
            lowerRight->moveLeft(cursor, this);
        }
        else {
            lowerRight->moveRight(cursor, this);
        }
    }
}


/**
 * Appends our attributes to the dom element.
 */
void IndexElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);

    QDomDocument doc = element.ownerDocument();

    QDomElement cont = doc.createElement("CONTENT");
    cont.appendChild(content->getElementDom(doc));
    element.appendChild(cont);

    if (hasUpperLeft()) {
        QDomElement ind = doc.createElement("UPPERLEFT");
        ind.appendChild(upperLeft->getElementDom(doc));
        element.appendChild(ind);
    }
    if (hasUpperMiddle()) {
        QDomElement ind = doc.createElement("UPPERMIDDLE");
        ind.appendChild(upperMiddle->getElementDom(doc));
        element.appendChild(ind);
    }
    if (hasUpperRight()) {
        QDomElement ind = doc.createElement("UPPERRIGHT");
        ind.appendChild(upperRight->getElementDom(doc));
        element.appendChild(ind);
    }
    if (hasLowerLeft()) {
        QDomElement ind = doc.createElement("LOWERLEFT");
        ind.appendChild(lowerLeft->getElementDom(doc));
        element.appendChild(ind);
    }
    if (hasLowerMiddle()) {
        QDomElement ind = doc.createElement("LOWERMIDDLE");
        ind.appendChild(lowerMiddle->getElementDom(doc));
        element.appendChild(ind);
    }
    if (hasLowerRight()) {
        QDomElement ind = doc.createElement("LOWERRIGHT");
        ind.appendChild(lowerRight->getElementDom(doc));
        element.appendChild(ind);
    }
}

/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool IndexElement::readAttributesFromDom(QDomElement& element)
{
    if (!BasicElement::readAttributesFromDom(element)) {
        return false;
    }
    return true;
}

/**
 * Reads our content from the node. Sets the node to the next node
 * that needs to be read.
 * Returns false if it failed.
 */
bool IndexElement::readContentFromDom(QDomNode& node)
{
    if (!BasicElement::readContentFromDom(node)) {
        return false;
    }

    delete content;
    content = buildChild(node, "CONTENT");
    if (content == 0) {
        cerr << "Empty content in IndexElement.\n";
        return false;
    }
    node = node.nextSibling();

    upperLeft = buildChild(node, "UPPERLEFT");
    if (upperLeft != 0) {
        node = node.nextSibling();
    }

    upperMiddle = buildChild(node, "UPPERMIDDLE");
    if (upperMiddle != 0) {
        node = node.nextSibling();
    }

    upperRight = buildChild(node, "UPPERRIGHT");
    if (upperRight != 0) {
        node = node.nextSibling();
    }

    lowerLeft = buildChild(node, "LOWERLEFT");
    if (lowerLeft != 0) {
        node = node.nextSibling();
    }

    lowerMiddle = buildChild(node, "LOWERMIDDLE");
    if (lowerMiddle != 0) {
        node = node.nextSibling();
    }

    lowerRight = buildChild(node, "LOWERRIGHT");
    if (lowerRight != 0) {
        node = node.nextSibling();
    }

    return true;
}

ElementIndexPtr IndexElement::getIndex(int position)
{
    switch (position) {
	case upperRightPos:
	    return getUpperRight();
	case lowerRightPos:
	    return getLowerRight();
	case lowerMiddlePos:
	    return getLowerMiddle();
	case upperMiddlePos:
	    return getUpperMiddle();
	case lowerLeftPos:
	    return getLowerLeft();
	case upperLeftPos:
	    return getUpperLeft();
    }
    return getUpperRight();
}



QString IndexElement::toLatex()
{
    QString index="";
    
    bool onlyRight=!hasUpperLeft() && !hasLowerLeft() 
		    && !hasUpperMiddle() && !hasLowerMiddle();    

    if(onlyRight){

        index+="{"+content->toLatex()+"}";

	if(hasUpperRight())
            index+="^{"+upperRight->toLatex()+"}";
        if(hasLowerRight())
	    index+="_{"+lowerRight->toLatex()+"}";
    }
    else
    {

	if(hasUpperMiddle()) {
       	    index+="\\overset{";
            index+="^{"+upperMiddle->toLatex()+"}";
	    index+="}{";

	}

	if(hasLowerMiddle()) {
       	    index+="\\underset{";
            index+="^{"+lowerMiddle->toLatex()+"}";
	    index+="}{";

	}

	index+="\\sideset{";

	if(hasUpperLeft())
            index+="^{"+upperLeft->toLatex()+"}";

	if(hasLowerLeft())
    	    index+="_{"+lowerLeft->toLatex()+"}";

	index+="}{";

	if(hasUpperRight())
	    index+="^{"+upperRight->toLatex()+"}";
	if(hasLowerRight())
    	    index+="_{"+lowerRight->toLatex()+"}";
	index+="}";

        index+="{"+content->toLatex()+"}";

	if(hasUpperMiddle()) index+="}";
	if(hasLowerMiddle()) index+="}";

    }
    return index;
}
