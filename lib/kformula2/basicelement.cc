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


#include "basicelement.h"
#include "formulacursor.h"
#include "sequenceelement.h"


BasicElement::BasicElement(BasicElement* p)
    : parent(p)
{
    relativeSize = -1;
}

BasicElement::~BasicElement()
{
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
BasicElement* BasicElement::isInside(const QPoint& point,
                                     const QPoint& parentOrigin)
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
