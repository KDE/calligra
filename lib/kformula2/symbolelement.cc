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

#include "formulacursor.h"
#include "formulaelement.h"
#include "sequenceelement.h"
#include "symbolelement.h"


SymbolElement::SymbolElement(Artwork::SymbolType type, BasicElement* parent)
    : BasicElement(parent), symbol(type)
{
    content = new SequenceElement(this);
    //upper = new SequenceElement(this);
    //upper->setRelativeSize(-2);
    //lower = new SequenceElement(this);
    //lower->setRelativeSize(-2);
    upper = 0;
    lower = 0;
}

SymbolElement::~SymbolElement()
{
    delete lower;
    delete upper;
    delete content;
}


BasicElement* SymbolElement::goToPos(FormulaCursor* cursor, bool& handled,
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
        if (hasLower()) {
            e = lower->goToPos(cursor, handled, point, myPos);
            if (e != 0) {
                return e;
            }
        }
        if (hasUpper()) {
            e = upper->goToPos(cursor, handled, point, myPos);
            if (e != 0) {
                return e;
            }
        }

        // the positions after the indexes.
        int dx = point.x() - myPos.x();
        int dy = point.y() - myPos.y();
        if (dy < symbol.getY()) {
            if (hasUpper() && (dx > upper->getX())) {
                upper->moveLeft(cursor, this);
                handled = true;
                return upper;
            }
        }
        else if (dy > symbol.getY()+symbol.getHeight()) {
            if (hasLower() && (dx > lower->getX())) {
                lower->moveLeft(cursor, this);
                handled = true;
                return lower;
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
void SymbolElement::calcSizes(ContextStyle& style, int parentSize)
{
    int mySize = parentSize + getRelativeSize();
    symbol.calcSizes(style, mySize);
    content->calcSizes(style, mySize);

    symbol.scale(((double)parentSize)/symbol.getHeight()*2);
    
    int upperWidth = 0;
    int upperHeight = 0;
    if (hasUpper()) {
        upper->calcSizes(style, mySize);
        upperWidth = upper->getWidth();
        upperHeight = upper->getHeight();
    }

    int lowerWidth = 0;
    int lowerHeight = 0;
    if (hasLower()) {
        lower->calcSizes(style, mySize);
        lowerWidth = lower->getWidth();
        lowerHeight = lower->getHeight();
    }

    // widths
    int xOffset = QMAX(symbol.getWidth(), QMAX(upperWidth, lowerWidth));
    symbol.setX(xOffset - symbol.getWidth());
    content->setX(xOffset);

    setWidth(QMAX(content->getX() + content->getWidth(),
                  QMAX(upperWidth, lowerWidth)));
    
    // heights
    int toMidline = QMAX(content->getMidline(), upperHeight + symbol.getHeight()/2);
    int fromMidline = QMAX(content->getHeight() - content->getMidline(),
                           lowerHeight + symbol.getHeight()/2);
    setHeight(toMidline + fromMidline);
    setMidline(toMidline);

    symbol.setY(toMidline - symbol.getHeight()/2);
    content->setY(toMidline - content->getMidline());

    if (hasUpper()) {
        if (upperWidth < symbol.getWidth()) {
            upper->setX(symbol.getX() + (symbol.getWidth() - upperWidth) / 2);
        }
        else {
            upper->setX(xOffset - upperWidth);
        }
        upper->setY(toMidline - upperHeight - symbol.getHeight()/2);
    }
    if (hasLower()) {
        if (lowerWidth < symbol.getWidth()) {
            lower->setX(symbol.getX() + (symbol.getWidth() - lowerWidth) / 2);
        }
        else {
            lower->setX(xOffset - lowerWidth);
        }
        lower->setY(toMidline + symbol.getHeight()/2);
    }
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void SymbolElement::draw(QPainter& painter, ContextStyle& style,
                         int parentSize, const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    int mySize = parentSize + getRelativeSize();

    symbol.draw(painter, style, mySize, myPos);
    content->draw(painter, style, mySize, myPos);
    if (hasUpper()) {
        upper->draw(painter, style, mySize, myPos);
    }
    if (hasLower()) {
        lower->draw(painter, style, mySize, myPos);
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
void SymbolElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
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
            if (linear && hasLower()) {
                lower->moveLeft(cursor, this);
            }
            else if (linear && hasUpper()) {
                upper->moveLeft(cursor, this);
            }
            else {
                getParent()->moveLeft(cursor, this);
            }
        }
        else if (from == lower) {
            if (linear && hasUpper()) {
                upper->moveLeft(cursor, this);
            }
            else {
                getParent()->moveLeft(cursor, this);
            }
        }            
        else if (from == upper) {
            getParent()->moveLeft(cursor, this);
        }
    }
}

/**
 * Enters this element while moving to the right starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the right of it.
 */
void SymbolElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveRight(cursor, this);
    }
    else {
        bool linear = cursor->getLinearMovement();
        if (from == getParent()) {
            if (linear && hasUpper()) {
                upper->moveRight(cursor, this);
            }
            else if (linear && hasLower()) {
                lower->moveRight(cursor, this);
            }
            else {
                content->moveRight(cursor, this);
            }
        }            
        else if (from == upper) {
            if (linear && hasLower()) {
                lower->moveRight(cursor, this);
            }
            else {
                content->moveRight(cursor, this);
            }
        }
        else if (from == lower) {
            content->moveRight(cursor, this);
        }            
        else if (from == content) {
            getParent()->moveRight(cursor, this);
        }
    }
}

/**
 * Enters this element while moving up starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or above it.
 */
void SymbolElement::moveUp(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveUp(cursor, this);
    }
    else {
        if (from == content) {
            if (hasUpper()) {
                upper->moveLeft(cursor, this);
            }
            else {
                getParent()->moveUp(cursor, this);
            }
        }            
        else if (from == upper) {
            getParent()->moveUp(cursor, this);
        }            
        else if ((from == getParent()) || (from == lower)) {
            content->moveRight(cursor, this);
        }            
    }
}

/**
 * Enters this element while moving down starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or below it.
 */
void SymbolElement::moveDown(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveDown(cursor, this);
    }
    else {
        if (from == content) {
            if (hasLower()) {
                lower->moveLeft(cursor, this);
            }
            else {
                getParent()->moveDown(cursor, this);
            }
        }            
        else if (from == lower) {
            getParent()->moveDown(cursor, this);
        }            
        else if ((from == getParent()) || (from == upper)) {
            content->moveRight(cursor, this);
        }            
    }
}

// children
    
// main child
//
// If an element has children one has to become the main one.

void SymbolElement::setMainChild(SequenceElement* child)
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
void SymbolElement::insert(FormulaCursor* cursor,
                           QList<BasicElement>& newChildren,
                           Direction direction)
{
    SequenceElement* index = static_cast<SequenceElement*>(newChildren.take(0));
    index->setParent(this);
    index->setRelativeSize(-2);
    
    switch (cursor->getPos()) {
    case upperPos:
        upper = index;
        break;
    case lowerPos:
        lower = index;
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
void SymbolElement::remove(FormulaCursor* cursor,
                           QList<BasicElement>& removedChildren,
                           Direction direction)
{
    int pos = cursor->getPos();
    switch (pos) {
    case upperPos:
        removedChildren.append(upper);
        formula()->elementRemoval(upper);
        upper = 0;
        setToUpper(cursor);
        break;
    case lowerPos:
        removedChildren.append(lower);
        formula()->elementRemoval(lower);
        lower = 0;
        setToLower(cursor);
        break;
    case contentPos: {
        BasicElement* parent = getParent();
        parent->selectChild(cursor, this);
        parent->remove(cursor, removedChildren, direction);
        break;
    }
    }
    formula()->changed();
}

/**
 * Moves the cursor to a normal place where new elements
 * might be inserted.
 */
void SymbolElement::normalize(FormulaCursor* cursor, Direction direction)
{
    if (direction == BasicElement::beforeCursor) {
        content->moveLeft(cursor, this);
    }
    else {
        content->moveRight(cursor, this);
    }
}
    
/**
 * Returns the child at the cursor.
 */
BasicElement* SymbolElement::getChild(FormulaCursor* cursor, Direction)
{
    int pos = cursor->getPos();
    switch (pos) {
    case contentPos:
        return content;
    case upperPos:
        return upper;
    case lowerPos:
        return lower;
    }
    return 0;
}

/**
 * Sets the cursor to select the child. The mark is placed before,
 * the position behind it.
 */
void SymbolElement::selectChild(FormulaCursor* cursor, BasicElement* child)
{
    if (child == content) {
        setToContent(cursor);
    }
    else if (child == upper) {
        setToUpper(cursor);
    }
    else if (child == lower) {
        setToLower(cursor);
    }
}

void SymbolElement::setToUpper(FormulaCursor* cursor)
{
    cursor->setTo(this, upperPos);
}

void SymbolElement::setToLower(FormulaCursor* cursor)
{
    cursor->setTo(this, lowerPos);
}

/**
 * Sets the cursor to point to the place where the content is.
 * There always is a content so this is not a useful place.
 * No insertion or removal will succeed as long as the cursor is
 * there.
 */
void SymbolElement::setToContent(FormulaCursor* cursor)
{
    cursor->setTo(this, contentPos);
}


void SymbolElement::moveToUpper(FormulaCursor* cursor, Direction direction)
{
    if (hasUpper()) {
        if (direction == beforeCursor) {
            upper->moveLeft(cursor, this);
        }
        else {
            upper->moveRight(cursor, this);
        }
    }
}

void SymbolElement::moveToLower(FormulaCursor* cursor, Direction direction)
{
    if (hasLower()) {
        if (direction == beforeCursor) {
            lower->moveLeft(cursor, this);
        }
        else {
            lower->moveRight(cursor, this);
        }
    }
}

QDomElement SymbolElement::getElementDom(QDomDocument *doc)
{
    QDomElement de=doc->createElement("SYMBOL");
    de.appendChild(BasicElement::getElementDom(doc));

    de.setAttribute("TYPE", symbol.getType());
    
    QDomElement con=doc->createElement("CONTENT");
    con.appendChild(content->getElementDom(doc));
    de.appendChild(con);

    if(hasLower()) {
        QDomElement ind=doc->createElement("LOWER");
        ind.appendChild(lower->getElementDom(doc));
        de.appendChild(ind);
    }
    if(hasUpper()) {
        QDomElement ind=doc->createElement("UPPER");
        ind.appendChild(upper->getElementDom(doc));
        de.appendChild(ind);
    }
    return de;
}


bool SymbolElement::buildFromDom(QDomElement *elem)
{
    // checking
    if (elem->tagName() != "SYMBOL") {
        cerr << "Wrong tag name " << elem->tagName().latin1() << "for SymbolElement.\n";
        return false;
    }

    // get attributes
    QString typeStr = elem->attribute("TYPE");
    if(!typeStr.isNull()) {
        symbol.setType(static_cast<Artwork::SymbolType>(typeStr.toInt()));
    }

    // read parent
    QDomNode n = elem->firstChild();
    if (n.isElement()) {
        QDomElement e = n.toElement();
        if (!BasicElement::buildFromDom(&e)) {
            return false;
        }
    }
    else {
        return false;
    }
    n = n.nextSibling();

    // read content
    delete content;
    content = buildChild(n, "CONTENT");
    if (content == 0) {
        cerr << "Empty content in BracketElement.\n";
        return false;
    }
    n = n.nextSibling();

    lower = buildChild(n, "LOWER");
    if (lower != 0) {
        n = n.nextSibling();
    }

    upper = buildChild(n, "UPPER");
    if (upper != 0) {
        n = n.nextSibling();
    }
    
    return true;
}
