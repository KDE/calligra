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

KFORMULA_NAMESPACE_BEGIN
using namespace std;

SymbolElement::SymbolElement(SymbolType type, BasicElement* parent)
    : BasicElement(parent), symbol(type)
{
    content = new SequenceElement(this);
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
                                     const KoPoint& point, const KoPoint& parentOrigin)
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        KoPoint myPos(parentOrigin.x() + getX(),
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
        double dx = point.x() - myPos.x();
        double dy = point.y() - myPos.y();
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
void SymbolElement::calcSizes(const ContextStyle& style, ContextStyle::TextStyle tstyle, ContextStyle::IndexStyle istyle )
{
    double mySize = style.getAdjustedSize( tstyle );
    double distX = style.getThinSpace( tstyle );
    double distY = style.getThinSpace( tstyle );

    symbol.calcSizes(style, qRound(mySize*1.5));
    content->calcSizes(style, tstyle, istyle);

    //symbol.scale(((double)parentSize)/symbol.getHeight()*2);

    double upperWidth = 0;
    double upperHeight = 0;
    if (hasUpper()) {
        upper->calcSizes(style, style.convertTextStyleIndex( tstyle ),
			 style.convertIndexStyleUpper( istyle ) );
        upperWidth = upper->getWidth();
        upperHeight = upper->getHeight() + distY;
    }

    double lowerWidth = 0;
    double lowerHeight = 0;
    if (hasLower()) {
        lower->calcSizes(style, style.convertTextStyleIndex( tstyle ),
			 style.convertIndexStyleLower( istyle ) );
        lowerWidth = lower->getWidth();
        lowerHeight = lower->getHeight() + distY;
    }

    // widths
    double xOffset = QMAX(symbol.getWidth(), QMAX(upperWidth, lowerWidth));
    if (style.getCenterSymbol()) {
        symbol.setX((xOffset - symbol.getWidth()) / 2);
    }
    else {
        symbol.setX(xOffset - symbol.getWidth());
    }
    content->setX(xOffset + distX/2);

    setWidth(QMAX(content->getX() + content->getWidth(),
                  QMAX(upperWidth, lowerWidth)));

    // heights
    //int toMidline = QMAX(content->getHeight() / 2,
    double toMidline = QMAX(content->getMidline(),
                            upperHeight + symbol.getHeight()/2);
    //int fromMidline = QMAX(content->getHeight() / 2,
    double fromMidline = QMAX(content->getHeight() - content->getMidline(),
                              lowerHeight + symbol.getHeight()/2);
    setHeight(toMidline + fromMidline);
    setMidline(toMidline);

    symbol.setY(toMidline - symbol.getHeight()/2);
    //content->setY(toMidline - content->getHeight()/2);
    content->setY(toMidline - content->getMidline());

    if (hasUpper()) {
        if (style.getCenterSymbol()) {
            upper->setX((xOffset - upperWidth) / 2);
        }
        else {
            if (upperWidth < symbol.getWidth()) {
                upper->setX(symbol.getX() + (symbol.getWidth() - upperWidth) / 2);
            }
            else {
                upper->setX(xOffset - upperWidth);
            }
        }
        upper->setY(toMidline - upperHeight - symbol.getHeight()/2);
    }
    if (hasLower()) {
        if (style.getCenterSymbol()) {
            lower->setX((xOffset - lowerWidth) / 2);
        }
        else {
            if (lowerWidth < symbol.getWidth()) {
                lower->setX(symbol.getX() + (symbol.getWidth() - lowerWidth) / 2);
            }
            else {
                lower->setX(xOffset - lowerWidth);
            }
        }
        lower->setY(toMidline + symbol.getHeight()/2 + distY);
    }
    calcBaseline();
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void SymbolElement::draw(QPainter& painter, const QRect& r,
                         const ContextStyle& style,
			 ContextStyle::TextStyle tstyle,
			 ContextStyle::IndexStyle istyle,
                         const KoPoint& parentOrigin)
{
    KoPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    double mySize = style.getAdjustedSize( tstyle );
    if (!QRect(myPos.x(), myPos.y(), getWidth(), getHeight()).intersects(r))
        return;

    symbol.draw(painter, r, style, qRound(mySize), myPos);
    content->draw(painter, r, style, tstyle, istyle, myPos);
    if (hasUpper()) {
        upper->draw(painter, r, style, style.convertTextStyleIndex( tstyle ),
			 style.convertIndexStyleUpper( istyle ), myPos);
    }
    if (hasLower()) {
        lower->draw(painter, r, style, style.convertTextStyleIndex( tstyle ),
			 style.convertIndexStyleLower( istyle ), myPos);
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

// void SymbolElement::setMainChild(SequenceElement* child)
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
void SymbolElement::insert(FormulaCursor* cursor,
                           QList<BasicElement>& newChildren,
                           Direction direction)
{
    SequenceElement* index = static_cast<SequenceElement*>(newChildren.take(0));
    index->setParent(this);

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


/**
 * Appends our attributes to the dom element.
 */
void SymbolElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);

    element.setAttribute("TYPE", symbol.getType());

    QDomDocument doc = element.ownerDocument();

    QDomElement con = doc.createElement("CONTENT");
    con.appendChild(content->getElementDom(doc));
    element.appendChild(con);

    if(hasLower()) {
        QDomElement ind = doc.createElement("LOWER");
        ind.appendChild(lower->getElementDom(doc));
        element.appendChild(ind);
    }
    if(hasUpper()) {
        QDomElement ind = doc.createElement("UPPER");
        ind.appendChild(upper->getElementDom(doc));
        element.appendChild(ind);
    }
}

/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool SymbolElement::readAttributesFromDom(QDomElement& element)
{
    if (!BasicElement::readAttributesFromDom(element)) {
        return false;
    }

    QString typeStr = element.attribute("TYPE");
    if(!typeStr.isNull()) {
        symbol.setType(static_cast<SymbolType>(typeStr.toInt()));
    }

    return true;
}

/**
 * Reads our content from the node. Sets the node to the next node
 * that needs to be read.
 * Returns false if it failed.
 */
bool SymbolElement::readContentFromDom(QDomNode& node)
{
    if (!BasicElement::readContentFromDom(node)) {
        return false;
    }

    delete content;
    content = buildChild(node, "CONTENT");
    if (content == 0) {
        cerr << "Empty content in SymbolElement.\n";
        return false;
    }
    node = node.nextSibling();

    bool lowerRead = false;
    bool upperRead = false;

    while (!node.isNull() && !(upperRead && lowerRead)) {

        if (!lowerRead && (node.nodeName().upper() == "LOWER")) {
            lower = buildChild(node, "LOWER");
            lowerRead = lower != 0;
        }

        if (!upperRead && (node.nodeName().upper() == "UPPER")) {
            upper = buildChild(node, "UPPER");
            upperRead = upper != 0;
        }

        node = node.nextSibling();
    }
    return true;
}

QString SymbolElement::toLatex()
{
    QString sym;

    switch(symbol.getType()) {

	case 1001:
	 sym="\\int";
	break;
	case 1002:
	 sym="\\sum";
	break;
	case 1003:
	 sym="\\prod";
	break;

	default:
	 sym=" ";

    }


    if(hasLower()) {
        sym+="_{";
	sym+=lower->toLatex();
	sym+="}";
    }

    if(hasUpper()) {
        sym+="^{";
	sym+=upper->toLatex();
	sym+="}";
    }

        sym+="{";
	sym+=content->toLatex();
	sym+="}";


    return sym;
}

KFORMULA_NAMESPACE_END
