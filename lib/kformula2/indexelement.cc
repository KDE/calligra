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

#include "indexelement.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "sequenceelement.h"


IndexElement::IndexElement(BasicElement* parent)
    : BasicElement(parent)
{
    content = new SequenceElement(this);

    upperLeft = 0;
    upperRight = 0;
    lowerLeft = 0;
    lowerRight = 0;
}

IndexElement::~IndexElement()
{
    delete content;
    delete upperLeft;
    delete upperRight;
    delete lowerLeft;
    delete lowerRight;
}

ostream& IndexElement::output(ostream& stream)
{
    stream << "IndexElement {\n";
    BasicElement::output(stream) << endl;
    stream << "content: ";
    content->output(stream) << endl;
    if (hasUpperLeft()) {
        stream << "upperLeft: ";
        upperLeft->output(stream) << endl;
    }
    if (hasLowerLeft()) {
        stream << "lowerLeft: ";
        lowerLeft->output(stream) << endl;
    }
    if (hasUpperRight()) {
        stream << "upperRight: ";
        upperRight->output(stream) << endl;
    }
    if (hasLowerRight()) {
        stream << "lowerRight: ";
        lowerRight->output(stream) << endl;
    }
    stream << '}';
    return stream;
}

/**
 * Returns the element the point is in.
 */
BasicElement* IndexElement::isInside(const QPoint& point,
                                     const QPoint& parentOrigin)
{
    BasicElement* e = BasicElement::isInside(point, parentOrigin);
    if (e != 0) {
        QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
        e = content->isInside(point, myPos);
        if (e != 0) return e;

        if (hasUpperRight()) {
            e = upperRight->isInside(point, myPos);
            if (e != 0) return e;
        }
        if (hasUpperLeft()) {
            e = upperLeft->isInside(point, myPos);
            if (e != 0) return e;
        }
        if (hasLowerRight()) {
            e = lowerRight->isInside(point, myPos);
            if (e != 0) return e;
        }
        if (hasLowerLeft()) {
            e = lowerLeft->isInside(point, myPos);
            if (e != 0) return e;
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
    
/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void IndexElement::calcSizes(ContextStyle& contextStyle, int parentSize)
{
    int mySize = parentSize + getRelativeSize();
    
    // get the indexes size
    int ulWidth = 0, ulHeight = 0, ulMidline = 0;
    if (hasUpperLeft()) {
        upperLeft->calcSizes(contextStyle, mySize);
        ulWidth = upperLeft->getWidth();
        ulHeight = upperLeft->getHeight();
        ulMidline = upperLeft->getMidline();
    }

    int urWidth = 0, urHeight = 0, urMidline = 0;
    if (hasUpperRight()) {
        upperRight->calcSizes(contextStyle, mySize);
        urWidth = upperRight->getWidth();
        urHeight = upperRight->getHeight();
        urMidline = upperRight->getMidline();
    }

    int llWidth = 0, llHeight = 0, llMidline = 0;
    if (hasLowerLeft()) {
        lowerLeft->calcSizes(contextStyle, mySize);
        llWidth = lowerLeft->getWidth();
        llHeight = lowerLeft->getHeight();
        llMidline = lowerLeft->getMidline();
    }

    int lrWidth = 0, lrHeight = 0, lrMidline = 0;
    if (hasLowerRight()) {
        lowerRight->calcSizes(contextStyle, mySize);
        lrWidth = lowerRight->getWidth();
        lrHeight = lowerRight->getHeight();
        lrMidline = lowerRight->getMidline();
    }

    // get the contents size
    content->calcSizes(contextStyle, mySize);
    int width = content->getWidth();
    int toMidline = content->getMidline();
    int fromMidline = content->getHeight() - toMidline;

    // calculate the x offsets
    if (ulWidth > llWidth) {
        upperLeft->setX(0);
        if (hasLowerLeft()) {
            lowerLeft->setX(ulWidth - llWidth);
        }
        content->setX(ulWidth);
        width += ulWidth;
    }
    else {
        if (hasUpperLeft()) {
            upperLeft->setX(llWidth - ulWidth);
        }
        if (hasLowerLeft()) {
            lowerLeft->setX(0);
        }
        content->setX(llWidth);
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
    if (ulHeight > urHeight) {
        upperLeft->setY(0);
        if (hasUpperRight()) {
            upperRight->setY(ulHeight - urHeight);
        }
        content->setY(QMAX(ulHeight - toMidline/2, 0));
        toMidline += content->getY();
    }
    else {
        if (hasUpperLeft()) {
            upperLeft->setY(urHeight - ulHeight);
        }
        if (hasUpperRight()) {
            upperRight->setY(0);
        }
        content->setY(QMAX(urHeight - toMidline/2, 0));
        toMidline += content->getY();
    }
    
    if (hasLowerLeft()) {
        lowerLeft->setY(toMidline + fromMidline/2);
    }
    if (hasLowerRight()) {
        lowerRight->setY(toMidline + fromMidline/2);
    }
    fromMidline += QMAX(QMAX(llHeight, lrHeight) - fromMidline/2, 0);

    // set the result
    setWidth(width);
    setHeight(toMidline+fromMidline);
    setMidline(toMidline);
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void IndexElement::draw(QPainter& painter, ContextStyle& contextStyle,
                        int parentSize, const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    int mySize = parentSize + getRelativeSize();
    
    content->draw(painter, contextStyle, mySize, myPos);
    if (hasUpperLeft()) {
        upperLeft->draw(painter, contextStyle, mySize, myPos);
    }
    if (hasUpperRight()) {
        upperRight->draw(painter, contextStyle, mySize, myPos);
    }
    if (hasLowerLeft()) {
        lowerLeft->draw(painter, contextStyle, mySize, myPos);
    }
    if (hasLowerRight()) {
        lowerRight->draw(painter, contextStyle, mySize, myPos);
    }

    // Debug
    //painter.setPen(Qt.red)
    //painter.drawRect(x, y, self.width(), self.height())
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
void IndexElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveLeft(cursor, this);
    }
    else {
        if (from == getParent()) {
            if (hasLowerRight()) {
                lowerRight->moveLeft(cursor, this);
            }
            else if (hasUpperRight()) {
                upperRight->moveLeft(cursor, this);
            }
            else {
                content->moveLeft(cursor, this);
            }
        }            
        else if (from == lowerRight) {
            if (hasUpperRight()) {
                upperRight->moveLeft(cursor, this);
            }
            else {
                content->moveLeft(cursor, this);
            }
        }
        else if (from == upperRight) {
            content->moveLeft(cursor, this);
        }            
        else if (from == content) {
            if (hasLowerLeft()) {
                lowerLeft->moveLeft(cursor, this);
            }
            else if (hasUpperLeft()) {
                upperLeft->moveLeft(cursor, this);
            }
            else {
                getParent()->moveLeft(cursor, this);
            }
        }
        else if (from == lowerLeft) {
            if (hasUpperLeft()) {
                upperLeft->moveLeft(cursor, this);
            }
            else {
                getParent()->moveLeft(cursor, this);
            }
        }
        else if (from == upperLeft) {
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
        if (from == getParent()) {
            if (hasUpperLeft()) {
                upperLeft->moveRight(cursor, this);
            }
            else if (hasLowerLeft()) {
                lowerLeft->moveRight(cursor, this);
            }
            else {
                content->moveRight(cursor, this);
            }
        }            
        else if (from == getParent()) {
            if (hasLowerLeft()) {
                lowerLeft->moveRight(cursor, this);
            }
            else {
                content->moveRight(cursor, this);
            }
        }
        else if (from == lowerLeft) {
            content->moveRight(cursor, this);
        }
        else if (from == content) {
            if (hasUpperRight()) {
                upperRight->moveRight(cursor, this);
            }
            else if (hasLowerRight()) {
                lowerRight->moveRight(cursor, this);
            }
            else {
                getParent()->moveRight(cursor, this);
            }
        }            
        else if (from == upperRight) {
            if (hasLowerRight()) {
                lowerRight->moveRight(cursor, this);
            }
            else {
                getParent()->moveRight(cursor, this);
            }
        }
        else if (from == lowerRight) {
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
            if (hasUpperRight()) {
                upperRight->moveRight(cursor, this);
            }
            else if (hasUpperLeft()) {
                upperLeft->moveLeft(cursor, this);
            }
            else {
                getParent()->moveUp(cursor, this);
            }
        }            
        else if ((from == upperLeft) || (from == upperRight)) {
            getParent()->moveUp(cursor, this);
        }            
        else if ((from == getParent()) || (from == lowerLeft)) {
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
            if (hasLowerRight()) {
                lowerRight->moveRight(cursor, this);
            }
            else if (hasLowerLeft()) {
                lowerLeft->moveLeft(cursor, this);
            }
            else {
                getParent()->moveDown(cursor, this);
            }
        }            
        else if ((from == lowerLeft) || (from == lowerRight)) {
            getParent()->moveDown(cursor, this);
        }            
        else if ((from == getParent()) || (from == upperLeft)) {
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

void IndexElement::setMainChild(SequenceElement* child)
{
    formula()->elementRemoval(content);
    content = child;
    content->setParent(this);
    formula()->changed();
}


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
    return !hasUpperLeft() && !hasUpperRight() &&
        !hasLowerLeft() && !hasLowerRight();
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
    else if (child == upperRight) {
        setToUpperRight(cursor);
    }
    else if (child == lowerRight) {
        setToLowerRight(cursor);
    }
}


// SequenceElement* IndexElement::requireUpperLeft()
// {
//     if (!hasUpperLeft()) {
//         upperLeft = new SequenceElement(this);
//     }
//     return upperLeft;
// }

// SequenceElement* IndexElement::requireUpperRight()
// {
//     if (!hasUpperRight()) {
//         upperRight = new SequenceElement(this);
//     }
//     return upperRight;
// }

// SequenceElement* IndexElement::requireLowerLeft()
// {
//     if (!hasLowerLeft()) {
//         lowerLeft = new SequenceElement(this);
//     }
//     return lowerLeft;
// }

// SequenceElement* IndexElement::requireLowerRight()
// {
//     if (!hasLowerRight()) {
//         lowerRight = new SequenceElement(this);
//     }
//     return lowerRight;
// }


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

void IndexElement::setToUpperRight(FormulaCursor* cursor)
{
    cursor->setTo(this, upperRightPos);
}

void IndexElement::setToLowerLeft(FormulaCursor* cursor)
{
    cursor->setTo(this, lowerLeftPos);
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

