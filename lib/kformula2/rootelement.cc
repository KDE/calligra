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

#include <iostream>
#include <qpainter.h>
#include <qpen.h>

#include "formulacursor.h"
#include "formulaelement.h"
#include "rootelement.h"
#include "sequenceelement.h"

using namespace std;

RootElement::RootElement(BasicElement* parent)
    : BasicElement(parent)
{
    content = new SequenceElement(this);
    index = 0;
}

RootElement::~RootElement()
{
    delete index;
    delete content;
}


BasicElement* RootElement::goToPos(FormulaCursor* cursor, bool& handled,
                                   const QPoint& point, const QPoint& parentOrigin)
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        QPoint myPos(parentOrigin.x() + getX(),
                     parentOrigin.y() + getY());

        e = content->goToPos(cursor, handled, point, myPos);
        if (e != 0) {
            return e;
        }
        if (hasIndex()) {
            e = index->goToPos(cursor, handled, point, myPos);
            if (e != 0) {
                return e;
            }
        }

        //int dx = point.x() - myPos.x();
        int dy = point.y() - myPos.y();

        // the position after the index
        if (hasIndex()) {
            if (dy < index->getHeight()) {
                index->moveLeft(cursor, this);
                handled = true;
                return index;
            }
        }

        return this;
    }
    return 0;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void RootElement::calcSizes(const ContextStyle& style, int parentSize)
{
    int mySize = parentSize;

    content->calcSizes(style, mySize);

    int indexWidth = 0;
    int indexHeight = 0;
    if (hasIndex()) {
        index->setSizeReduction(style);
        index->calcSizes(style, mySize);
        indexWidth = index->getWidth();
        indexHeight = index->getHeight();
    }

    int distX = style.getDistanceX(mySize);
    int distY = style.getDistanceY(mySize);
    int unit = (content->getHeight() + distY)/ 3;

    if (hasIndex()) {
        if (indexWidth > unit) {
            index->setX(0);
            rootOffset.setX(indexWidth - unit);
        }
        else {
            index->setX((unit - indexWidth)/2);
            rootOffset.setX(0);
        }
        if (indexHeight > unit) {
            index->setY(0);
            rootOffset.setY(indexHeight - unit);
        }
        else {
            index->setY(unit - indexHeight);
            rootOffset.setY(0);
        }
    }
    else {
        rootOffset.setX(0);
        rootOffset.setY(0);
    }

    setWidth(content->getWidth() + unit+unit/3+ rootOffset.x() + distX/2);
    setHeight(content->getHeight() + distY + rootOffset.y());
    setMidline(getHeight() - content->getHeight() + content->getMidline());

    content->setX(rootOffset.x() + unit+unit/3);
    content->setY(rootOffset.y() + distY);
    calcBaseline();
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void RootElement::draw(QPainter& painter, const QRect& r,
                       const ContextStyle& style,
                       int parentSize, const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    int mySize = parentSize;
    if (!QRect(myPos, getSize()).intersects(r))
        return;

    content->draw(painter, r, style, mySize, myPos);
    if (hasIndex()) {
        index->draw(painter, r, style, mySize, myPos);
    }

    int x = myPos.x() + rootOffset.x();
    int y = myPos.y() + rootOffset.y();
    //int distX = style.getDistanceX(mySize);
    int distY = style.getDistanceY(mySize);
    int unit = (content->getHeight() + distY)/ 3;

    painter.setPen(QPen(style.getDefaultColor(), 2*style.getLineWidth()));
    painter.drawLine(x+unit/3, y+unit+distY/2,
                     x+unit/2+unit/3, myPos.y()+getHeight());

    painter.setPen(QPen(style.getDefaultColor(), style.getLineWidth()));

    painter.drawLine(x+unit+unit/3, y+distY/2, x+unit/2+unit/3, myPos.y()+getHeight());
    painter.drawLine(x+unit+unit/3, y+distY/2, x+unit+unit/3+content->getWidth(), y+distY/2);
    painter.drawLine(x+unit/3, y+unit+distY/2, x, y+unit+unit/2);
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
        bool linear = cursor->getLinearMovement();
        if (from == getParent()) {
            content->moveLeft(cursor, this);
        }
        else if (from == content) {
            if (linear && hasIndex()) {
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
        bool linear = cursor->getLinearMovement();
        if (from == getParent()) {
            if (linear && hasIndex()) {
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
            if (hasIndex()) {
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
            if (hasIndex()) {
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
            getParent()->moveDown(cursor, this);
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

// void RootElement::setMainChild(SequenceElement* child)
// {
//     formula()->elementRemoval(content);
//     content = child;
//     content->setParent(this);
//     formula()->changed();
// }


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


void RootElement::moveToIndex(FormulaCursor* cursor, Direction direction)
{
    if (hasIndex()) {
        if (direction == beforeCursor) {
            index->moveLeft(cursor, this);
        }
        else {
            index->moveRight(cursor, this);
        }
    }
}

void RootElement::setToIndex(FormulaCursor* cursor)
{
    cursor->setTo(this, indexPos);
}


/**
 * Appends our attributes to the dom element.
 */
void RootElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);

    QDomDocument doc = element.ownerDocument();

    QDomElement con = doc.createElement("CONTENT");
    con.appendChild(content->getElementDom(doc));
    element.appendChild(con);

    if(hasIndex()) {
        QDomElement ind = doc.createElement("INDEX");
        ind.appendChild(index->getElementDom(doc));
        element.appendChild(ind);
    }
}

/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool RootElement::readAttributesFromDom(QDomElement& element)
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
bool RootElement::readContentFromDom(QDomNode& node)
{
    if (!BasicElement::readContentFromDom(node)) {
        return false;
    }

    delete content;
    content = buildChild(node, "CONTENT");
    if (content == 0) {
        cerr << "Empty content in RootElement.\n";
        return false;
    }
    node = node.nextSibling();

    index = buildChild(node, "INDEX");
    if (index != 0) {
        node = node.nextSibling();
    }

    return true;
}

QString RootElement::toLatex()
{
    QString root;
    root="\\sqrt";
    if(hasIndex()) {
        root+="[";
	root+=index->toLatex();
	root+="]";
    }
    root+="{";
    root+=content->toLatex();
    root+="}";

    return root;
}
