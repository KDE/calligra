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
#include <qstring.h>

#include "contextstyle.h"
#include "basicelement.h"
#include "formulacursor.h"
#include "sequenceelement.h"


int BasicElement::evilDestructionCount = 0;

BasicElement::BasicElement(BasicElement* p)
        : parent(p), elementType(0)
{
    relativeSize = 0;
    evilDestructionCount++;
}

BasicElement::~BasicElement()
{
    evilDestructionCount--;
}


/**
 * Returns the element the point is in.
 */
BasicElement* BasicElement::goToPos(FormulaCursor*, bool&, const QPoint& point, const QPoint& parentOrigin)
{
    int x = point.x() - (parentOrigin.x() + getX());
    if ((x >= 0) && (x < getWidth())) {
        int y = point.y() - (parentOrigin.y() + getY());
        if ((y >= 0) && (y < getHeight())) {
            return this;
        }
    }
    return 0;
}

/**
 * Returns our position inside the widget.
 */
QPoint BasicElement::widgetPos()
{
    int x = 0;
    int y = 0;
    for (BasicElement* element = this; element != 0; element = element->parent) {
        x += element->getX();
        y += element->getY();
    }
    return QPoint(x, y);
}


/**
 * Sets the cursor inside this element to its start position.
 * For most elements that is the main child.
 */
void BasicElement::goInside(FormulaCursor* cursor)
{
    BasicElement* mainChild = getMainChild();
    if (mainChild != 0) {
        mainChild->goInside(cursor);
    }
}


/**
 * Enters this element while moving to the left starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the left of it.
 */
void BasicElement::moveLeft(FormulaCursor* cursor, BasicElement*)
{
    getParent()->moveLeft(cursor, this);
}


/**
 * Enters this element while moving to the right starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the right of it.
 */
void BasicElement::moveRight(FormulaCursor* cursor, BasicElement*)
{
    getParent()->moveRight(cursor, this);
}


/**
 * Moves the cursor to a normal place where new elements
 * might be inserted.
 */
void BasicElement::normalize(FormulaCursor* cursor, Direction direction)
{
    BasicElement* element = getMainChild();
    if (element != 0) {
        if (direction == beforeCursor) {
            element->moveLeft(cursor, this);
        }
        else {
            element->moveRight(cursor, this);
        }
    }
}


QDomElement BasicElement::getElementDom(QDomDocument& doc)
{
    QDomElement de = doc.createElement(getTagName());
    writeDom(de);
    return de;
}

bool BasicElement::buildFromDom(QDomElement& element)
{
    if (element.tagName() != getTagName()) {
        cerr << "Wrong tag name " << element.tagName().latin1() << "for " << getTagName().latin1() << ".\n";
        return false;
    }
    if (!readAttributesFromDom(element)) {
        return false;
    }
    QDomNode node = element.firstChild();
    return readContentFromDom(node);
}

/**
 * Appends our attributes to the dom element.
 */
void BasicElement::writeDom(QDomElement& element)
{
    if (relativeSize != 0) {
        element.setAttribute("RELATIVESIZE", relativeSize);
    }	    
}
    
/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool BasicElement::readAttributesFromDom(QDomElement& element)
{
    QString sizeStr = element.attribute("RELATIVESIZE");
    if(!sizeStr.isNull()) {
        setRelativeSize(sizeStr.toInt());
    }
    return true;
}

/**
 * Reads our content from the node. Sets the node to the next node
 * that needs to be read.
 * Returns false if it failed.
 */
bool BasicElement::readContentFromDom(QDomNode&)
{
    return true;
}


/**
 * Returns a SequenceElement constructed from the nodes first child
 * if the nodes name matches the given name.
 */
SequenceElement* BasicElement::buildChild(QDomNode& node, QString name)
{
    SequenceElement* child = 0;
    if (node.isElement()) {
        QDomElement e = node.toElement();
        if (e.tagName().upper() == name) {
            QDomNode nodeInner = e.firstChild();
            if (nodeInner.isElement()) {
                QDomElement element = nodeInner.toElement();
                child = new SequenceElement(this);
                if (!child->buildFromDom(element)) {
                    delete child;
                    child = 0;
                }
            }
        }
    }
    return child;
}
