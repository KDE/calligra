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
#include <qlist.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpointarray.h>

#include "artwork.h"
#include "bracketelement.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "sequenceelement.h"


BracketElement::BracketElement(char l, char r, BasicElement* parent)
    : BasicElement(parent)
{
    content = new SequenceElement(this);
    right = createBracket(r);
    left = createBracket(l);
}


BracketElement::~BracketElement()
{
    delete left;
    delete right;
    delete content;
}


BasicElement* BracketElement::goToPos(FormulaCursor* cursor, bool& handled,
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

        // We are in one of those gaps.
        int dx = point.x() - myPos.x();
        int dy = point.y() - myPos.y();

        if ((dx > content->getX()+content->getWidth()) ||
            (dy > content->getY()+content->getHeight())) {
            content->moveEnd(cursor);
            handled = true;
            return content;
        }
        return this;
    }
    return 0;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void BracketElement::calcSizes(ContextStyle& style, int parentSize)
{
    int mySize = parentSize + getRelativeSize();
    content->calcSizes(style, parentSize);
    int contentHeight = 2 * QMAX(content->getMidline(),
                                 content->getHeight() - content->getMidline());
    
    left->calcSizes(style, mySize);
    //left->scale(((double)contentHeight+2)/left->getHeight());
    left->setHeight(contentHeight+2);
    
    right->calcSizes(style, mySize);
    //right->scale(((double)contentHeight+2)/right->getHeight());
    right->setHeight(contentHeight+2);

    // width
    setWidth(left->getWidth() + content->getWidth() + right->getWidth());
    content->setX(left->getWidth());
    right  ->setX(left->getWidth()+content->getWidth());

    // height
    setHeight(QMAX(content->getHeight(),
                   QMAX(left->getHeight(), right->getHeight())));
    setMidline(getHeight() / 2);

    left   ->setY((getHeight() - left   ->getHeight())/2);
    right  ->setY((getHeight() - right  ->getHeight())/2);

    content->setY(getMidline() - content->getMidline());
}


/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void BracketElement::draw(QPainter& painter, ContextStyle& style,
                          int parentSize, const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    int mySize = parentSize + getRelativeSize();

    left->draw(painter, style, mySize, myPos);
    content->draw(painter, style, mySize, myPos);
    right->draw(painter, style, mySize, myPos);
}


/**
 * Enters this element while moving to the left starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the left of it.
 */
void BracketElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveLeft(cursor, this);
    }
    else {
        if (from == getParent()) {
            content->moveLeft(cursor, this);
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
void BracketElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveRight(cursor, this);
    }
    else {
        if (from == getParent()) {
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
void BracketElement::moveUp(FormulaCursor* cursor, BasicElement*)
{
    getParent()->moveUp(cursor, this);
}

/**
 * Enters this element while moving down starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or below it.
 */
void BracketElement::moveDown(FormulaCursor* cursor, BasicElement*)
{
    getParent()->moveDown(cursor, this);
}


/**
 * Removes all selected children and returns them. Places the
 * cursor to where the children have been.
 *
 * Here we remove ourselve if we are requested to remove our content.
 */
void BracketElement::remove(FormulaCursor* cursor,
                            QList<BasicElement>& removedChildren,
                            Direction direction)
{
    if (cursor->getPos() == contentPos) {
        BasicElement* parent = getParent();
        parent->selectChild(cursor, this);
        parent->remove(cursor, removedChildren, direction);
    }
}


// main child
//
// If an element has children one has to become the main one.

SequenceElement* BracketElement::getMainChild()
{
    return content;
}

void BracketElement::setMainChild(SequenceElement* child)
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
void BracketElement::selectChild(FormulaCursor* cursor, BasicElement* child)
{
    if (child == content) {
        cursor->setTo(this, contentPos);
    }
}


/**
 * Creates a new bracket object that matches the char.
 */
Artwork* BracketElement::createBracket(char bracket)
{
    Artwork* aw = new Artwork();
    switch (bracket) {
    case '[':
    case ']':
    case '{':
    case '}':
    case '|':
    case '<':
    case '>':
    case '(':
    case ')':
        aw->setType(static_cast<Artwork::SymbolType>(bracket));
        break;
    }
    return aw;
}


QDomElement BracketElement::getElementDom(QDomDocument *doc)
{
    QDomElement de=doc->createElement("BRACKET");
    de.appendChild(BasicElement::getElementDom(doc));
    
    QDomElement con=doc->createElement("CONTENT");
    con.appendChild(content->getElementDom(doc));
    de.appendChild(con);

    de.setAttribute("LEFT", left->getType());
    de.setAttribute("RIGHT", right->getType());
    return de;
}


bool BracketElement::buildFromDom(QDomElement *elem)
{
    // checking
    if (elem->tagName() != "BRACKET") {
        cerr << "Wrong tag name " << elem->tagName() << "for BracketElement.\n";
        return false;
    }

    // get attributes
    QString leftStr = elem->attribute("LEFT");
    if(!leftStr.isNull()) {
        left->setType(static_cast<Artwork::SymbolType>(leftStr.toInt()));
    }
    QString rightStr = elem->attribute("RIGHT");
    if(!rightStr.isNull()) {
        right->setType(static_cast<Artwork::SymbolType>(rightStr.toInt()));
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
    return true;
}
