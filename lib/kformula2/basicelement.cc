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
    : parent(p)
{
    relativeSize = 0;
    evilDestructionCount++;
}

BasicElement::~BasicElement()
{
    evilDestructionCount--;
}

ostream& BasicElement::output(ostream& stream)
{
    return stream << "BasicElement: "
                  << position.x() << ", "
                  << position.y() << ", "
                  << size.width() << ", "
                  << size.height();
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

QDomElement BasicElement::getElementDom(QDomDocument *doc)
{
    QDomElement de = doc->createElement("BASIC");
    if (relativeSize != 0) {
        de.setAttribute("SIZE", relativeSize);
    }	    
    return de;
}

bool BasicElement::buildFromDom(QDomElement *elem)
{
    if (elem->tagName() != "BASIC") {
        cerr << "Wrong tag name " << elem->tagName().latin1() << "for BasicElement.\n";
        return false;
    }
    QString sizeStr = elem->attribute("SIZE");
    if(!sizeStr.isNull()) {
        setRelativeSize(sizeStr.toInt());
    }
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
                if (!child->buildFromDom(&element)) {
                    delete child;
                    child = 0;
                }
            }
        }
    }
    return child;
}
