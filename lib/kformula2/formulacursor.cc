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

#include "formulacursor.h"
#include "formulaelement.h"
#include "sequenceelement.h"


FormulaCursor::FormulaCursor(FormulaElement* element)
{
    selectionFlag = false;
    mouseSelectionFlag = false;
    setTo(element, 0);
}

void FormulaCursor::setTo(SequenceElement* element, int cursor, int mark=-1)
{
    current = element;
    cursorPos = cursor;
    if ((mark == -1) && isSelection()) {
        return;
    }
    markPos = mark;
}


int FormulaCursor::getPos() const
{
//     if (isInsideParent()) {
//         BasicElement* parent = current->getParent();
//         return parent->getChildPos(current);
//     }
    return cursorPos;
}


void FormulaCursor::setPos(int pos)
{
}

void FormulaCursor::setMark(int mark)
{
}


/**
 * Moves the cursor to a normal position. That is somewhere
 * inside a SequenceElement.
 * You need to call this after each removal because the cursor
 * might point to some non existing place.
 */
void FormulaCursor::normalize(Direction direction)
{
    BasicElement* element = getElement();
    element->normalize(this, direction);
}



// BasicElement* FormulaCursor::getElement()
// {
//     if (isInsideParent()) {
//         return current->getParent();
//     }
//     return current;
// }

/**
 * Inserts the child at the current position.
 * Ignores the selection.
 */
void FormulaCursor::insert(BasicElement* child, Direction direction)
{
    QList<BasicElement> list;
    list.append(child);
    insert(&list, direction);
}

void FormulaCursor::insert(QList<BasicElement>* children, Direction direction)
{
    BasicElement* element = getElement();
    element->remove(this, children, direction);
}

QList<BasicElement>* FormulaCursor::remove(Direction direction)
{
    BasicElement* element = getElement();
    return element->remove(this, direction);
}


/**
 * Returns the child the cursor points to. Depending on the
 * direction this might be the child before or after the
 * cursor.
 *
 * Might be 0 is there is no such child.
 */
BasicElement* FormulaCursor::getAktiveChild(BasicElement::Direction direction)
{
    return getElement()->getChild(cursor, direction);
}


/**
 * Returns the IndexElement the cursor is on or 0
 * if there is non.
 */
IndexElement* FormulaCursor::getAktiveIndexElement()
{
    BasicElement* child = getAktiveChild(BasicElement::beforeCursor);
    IndexElement* element = 0;
    if (child != 0) {
        element = dynamic_cast<IndexElement*>(child);
    }
    if (element == 0) {
        element = dynamic_cast<IndexElement*>(getElement()->getParent());
    }
    return element;
}
