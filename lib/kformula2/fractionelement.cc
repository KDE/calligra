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

#include "formulaelement.h"
#include "formulacursor.h"
#include "fractionelement.h"
#include "sequenceelement.h"

using namespace std;

FractionElement::FractionElement(BasicElement* parent)
    : BasicElement(parent)
{
    numerator = new SequenceElement(this);
    denominator = new SequenceElement(this);
}

FractionElement::~FractionElement()
{
    delete denominator;
    delete numerator;
}


BasicElement* FractionElement::goToPos(FormulaCursor* cursor, bool& handled,
                                       const QPoint& point, const QPoint& parentOrigin)
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        QPoint myPos(parentOrigin.x() + getX(),
                     parentOrigin.y() + getY());
        e = numerator->goToPos(cursor, handled, point, myPos);
        if (e != 0) {
            return e;
        }
        e = denominator->goToPos(cursor, handled, point, myPos);
        if (e != 0) {
            return e;
        }

        int dx = point.x() - myPos.x();
        int dy = point.y() - myPos.y();

        // the positions after the numerator / denominator
        if ((dx > numerator->getX()) &&
            (dy < numerator->getHeight())) {
            numerator->moveLeft(cursor, this);
            handled = true;
            return numerator;
        }
        else if ((dx > denominator->getX()) &&
                 (dy > denominator->getY())) {
            denominator->moveLeft(cursor, this);
            handled = true;
            return denominator;
        }

        return this;
    }
    return 0;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void FractionElement::calcSizes(const ContextStyle& style, int parentSize)
{
    int mySize = parentSize;
    numerator->calcSizes(style, mySize);
    denominator->calcSizes(style, mySize);

    int distX = style.getDistanceX(mySize);
    int distY = style.getDistanceY(mySize);

    setWidth(QMAX(numerator->getWidth(), denominator->getWidth()) + distX);
    setHeight(numerator->getHeight() + distY + denominator->getHeight());
    setMidline(numerator->getHeight() + distY / 2);
    setBaseline(-1);

    numerator->setX((getWidth() - numerator->getWidth()) / 2);
    denominator->setX((getWidth() - denominator->getWidth()) / 2);

    numerator->setY(0);
    denominator->setY(getHeight() - denominator->getHeight());
}


/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void FractionElement::draw(QPainter& painter, const QRect& r,
                           const ContextStyle& style,
                           int parentSize, const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    int mySize = parentSize;
    if (!QRect(myPos, getSize()).intersects(r))
        return;

    numerator->draw(painter, r, style, mySize, myPos);
    denominator->draw(painter, r, style, mySize, myPos);

    int distX = style.getDistanceX(mySize);
    painter.setPen(QPen(style.getDefaultColor(), style.getLineWidth()));
    painter.drawLine(myPos.x() + distX/2, myPos.y() + getMidline(),
                     myPos.x() + getWidth() - (distX - distX/2), myPos.y() + getMidline());
}


/**
 * Enters this element while moving to the left starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the left of it.
 */
void FractionElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveLeft(cursor, this);
    }
    else {
        bool linear = cursor->getLinearMovement();
        if (from == getParent()) {
            if (linear) {
                denominator->moveLeft(cursor, this);
            }
            else {
                numerator->moveLeft(cursor, this);
            }
        }
        else if (from == denominator) {
            numerator->moveLeft(cursor, this);
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
void FractionElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveRight(cursor, this);
    }
    else {
        bool linear = cursor->getLinearMovement();
        if (from == getParent()) {
            numerator->moveRight(cursor, this);
        }
        else if (from == numerator) {
            if (linear) {
                denominator->moveRight(cursor, this);
            }
            else {
                getParent()->moveRight(cursor, this);
            }
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
void FractionElement::moveUp(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveUp(cursor, this);
    }
    else {
        if (from == getParent()) {
            denominator->moveRight(cursor, this);
        }
        else if (from == denominator) {
            numerator->moveRight(cursor, this);
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
void FractionElement::moveDown(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveDown(cursor, this);
    }
    else {
        if (from == getParent()) {
            numerator->moveRight(cursor, this);
        }
        else if (from == numerator) {
            denominator->moveRight(cursor, this);
        }
        else {
            getParent()->moveDown(cursor, this);
        }
    }
}


/**
 * Reinserts the denominator if it has been removed.
 */
void FractionElement::insert(FormulaCursor* cursor,
                             QList<BasicElement>& newChildren,
                             Direction direction)
{
    if (cursor->getPos() == denominatorPos) {
        denominator = static_cast<SequenceElement*>(newChildren.take(0));
        denominator->setParent(this);

        if (direction == beforeCursor) {
            denominator->moveLeft(cursor, this);
        }
        else {
            denominator->moveRight(cursor, this);
        }
        cursor->setSelection(false);
        formula()->changed();
    }
}


/**
 * Removes all selected children and returns them. Places the
 * cursor to where the children have been.
 *
 * We remove ourselve if we are requested to remove our numerator.
 *
 * It is possible to remove the denominator. But after this we
 * are senseless and the caller is required to replace us.
 */
void FractionElement::remove(FormulaCursor* cursor,
                             QList<BasicElement>& removedChildren,
                             Direction direction)
{
    switch (cursor->getPos()) {
    case numeratorPos:
        getParent()->selectChild(cursor, this);
        getParent()->remove(cursor, removedChildren, direction);
        break;
    case denominatorPos:
        removedChildren.append(denominator);
        formula()->elementRemoval(denominator);
        denominator = 0;
        cursor->setTo(this, denominatorPos);
        formula()->changed();
        break;
    }
}


/**
 * Returns wether the element has no more useful
 * children (except its main child) and should therefore
 * be replaced by its main child's content.
 */
bool FractionElement::isSenseless()
{
    return denominator == 0;
}


// main child
//
// If an element has children one has to become the main one.

SequenceElement* FractionElement::getMainChild()
{
    return numerator;
}

void FractionElement::setMainChild(SequenceElement* child)
{
    formula()->elementRemoval(numerator);
    numerator = child;
    numerator->setParent(this);
    formula()->changed();
}


/**
 * Sets the cursor to select the child. The mark is placed before,
 * the position behind it.
 */
void FractionElement::selectChild(FormulaCursor* cursor, BasicElement* child)
{
    if (child == numerator) {
        cursor->setTo(this, numeratorPos);
    }
    else if (child == denominator) {
        cursor->setTo(this, denominatorPos);
    }
}


/**
 * Appends our attributes to the dom element.
 */
void FractionElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);

    QDomDocument doc = element.ownerDocument();

    QDomElement num = doc.createElement("NUMERATOR");
    num.appendChild(numerator->getElementDom(doc));
    element.appendChild(num);

    QDomElement den = doc.createElement("DENOMINATOR");
    den.appendChild(denominator->getElementDom(doc));
    element.appendChild(den);
}

/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool FractionElement::readAttributesFromDom(QDomElement& element)
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
bool FractionElement::readContentFromDom(QDomNode& node)
{
    if (!BasicElement::readContentFromDom(node)) {
        return false;
    }

    delete numerator;
    numerator = buildChild(node, "NUMERATOR");
    if (numerator == 0) {
        cerr << "Empty numerator in FractionElement.\n";
        return false;
    }
    node = node.nextSibling();

    delete denominator;
    denominator = buildChild(node, "DENOMINATOR");
    if (denominator == 0) {
        cerr << "Empty denominator in FractionElement.\n";
        return false;
    }
    node = node.nextSibling();

    return true;
}

QString FractionElement::toLatex()
{
    QString num,den,fra;
    den=denominator->toLatex();
    num=numerator->toLatex();
    fra="\\frac{"+num+"}{"+den+"}";
    return fra;
}
