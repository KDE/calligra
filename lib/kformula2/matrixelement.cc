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

#include <qarray.h>
#include <qpainter.h>

#include "formulacursor.h"
#include "matrixelement.h"
#include "sequenceelement.h"


MatrixElement::MatrixElement(int rows, int columns, BasicElement* parent)
    : BasicElement(parent)
{
    for (uint r = 0; r < rows; r++) {
        QList<SequenceElement>* list = new QList<SequenceElement>;
        list->setAutoDelete(true);
        for (uint c = 0; c < columns; c++) {
            list->append(new SequenceElement(this));
        }
        content.append(list);
    }
    content.setAutoDelete(true);
}

MatrixElement::~MatrixElement()
{
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
void MatrixElement::calcSizes(ContextStyle& style, int parentSize)
{
    int mySize = parentSize + getRelativeSize();
    
    QArray<int> toMidlines(getRows());
    QArray<int> fromMidlines(getRows());
    QArray<int> widths(getColumns());

    toMidlines.fill(0);
    fromMidlines.fill(0);
    widths.fill(0);

    uint rows = getRows();
    uint columns = getColumns();

    for (uint r = 0; r < rows; r++) {
        QList<SequenceElement>* list = content.at(r);
        for (uint c = 0; c < columns; c++) {
            SequenceElement* element = list->at(c);
            element->calcSizes(style, mySize);
            toMidlines[r] = QMAX(toMidlines[r], element->getMidline());
            fromMidlines[r] = QMAX(fromMidlines[r],
                                   element->getHeight()-element->getMidline());
            widths[c] = QMAX(widths[c], element->getWidth());
        }
    }

    int dist = style.getDistance();

    int yPos = 0;
    for (uint r = 0; r < rows; r++) {
        QList<SequenceElement>* list = content.at(r);
        int xPos = 0;
        yPos += toMidlines[r];
        for (uint c = 0; c < columns; c++) {
            SequenceElement* element = list->at(c);
            switch (style.getMatrixAlignment()) {
            case ContextStyle::left:
                element->setX(xPos);
                break;
            case ContextStyle::center:
                element->setX(xPos + (widths[c] - element->getWidth())/2);
                break;
            case ContextStyle::right:
                element->setX(xPos + widths[c] - element->getWidth());
                break;
            }
            element->setY(yPos - element->getMidline());
            xPos += widths[c] + dist;
        }
        yPos += fromMidlines[r] + dist;
    }
    
    int width = dist * (columns - 1);
    int height = dist * (rows - 1);

    for (uint r = 0; r < rows; r++) height += toMidlines[r] + fromMidlines[r];
    for (uint c = 0; c < columns; c++) width += widths[c];

    setWidth(width);
    setHeight(height);
    setMidline(height/2);
}

/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void MatrixElement::draw(QPainter& painter, ContextStyle& style,
                         int parentSize, const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x()+getX(), parentOrigin.y()+getY());
    int mySize = parentSize + getRelativeSize();

    uint rows = getRows();
    uint columns = getColumns();

    for (uint r = 0; r < rows; r++) {
        for (uint c = 0; c < columns; c++) {
            getElement(r, c)->draw(painter, style, mySize, myPos);
        }
    }

    // Debug
    //painter.setPen(Qt::red);
    //painter.drawRect(myPos.x(), myPos.y(), getWidth(), getHeight());
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
void MatrixElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveLeft(cursor, this);
    }
    else {
        if (from == getParent()) {
            getElement(getRows()-1, getColumns()-1)->moveLeft(cursor, this);
        }
        else {
            int row = 0;
            int column = 0;
            if (searchElement(from, row, column)) {
                if (column > 0) {
                    getElement(row, column-1)->moveLeft(cursor, this);
                }
                else if (row > 0) {
                    getElement(row-1, getColumns()-1)->moveLeft(cursor, this);
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
}

/**
 * Enters this element while moving to the right starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the right of it.
 */
void MatrixElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveRight(cursor, this);
    }
    else {
        if (from == getParent()) {
            getElement(0, 0)->moveRight(cursor, this);
        }
        else {
            int row = 0;
            int column = 0;
            if (searchElement(from, row, column)) {
                if (column < getColumns()-1) {
                    getElement(row, column+1)->moveRight(cursor, this);
                }
                else if (row < getRows()-1) {
                    getElement(row+1, 0)->moveRight(cursor, this);
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
}

/**
 * Enters this element while moving up starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or above it.
 */
void MatrixElement::moveUp(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveUp(cursor, this);
    }
    else {
        if (from == getParent()) {
            getElement(0, 0)->moveRight(cursor, this);
        }
        else {
            int row = 0;
            int column = 0;
            if (searchElement(from, row, column)) {
                if (row > 0) {
                    getElement(row-1, column)->moveRight(cursor, this);
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
}

/**
 * Enters this element while moving down starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or below it.
 */
void MatrixElement::moveDown(FormulaCursor* cursor, BasicElement* from)
{
    if (cursor->isSelectionMode()) {
        getParent()->moveDown(cursor, this);
    }
    else {
        if (from == getParent()) {
            getElement(0, 0)->moveRight(cursor, this);
        }
        else {
            int row = 0;
            int column = 0;
            if (searchElement(from, row, column)) {
                if (row < getRows()-1) {
                    getElement(row+1, column)->moveRight(cursor, this);
                }
                else {
                    getParent()->moveDown(cursor, this);
                }
            }
            else {
                getParent()->moveDown(cursor, this);
            }
        }
    }
}

/**
 * Sets the cursor inside this element to its start position.
 * For most elements that is the main child.
 */
void MatrixElement::goInside(FormulaCursor* cursor)
{
    getElement(0, 0)->moveRight(cursor, this);
}


bool MatrixElement::searchElement(BasicElement* element, int& row, int& column)
{
    uint rows = getRows();
    uint columns = getColumns();
    for (uint r = 0; r < rows; r++) {
        for (uint c = 0; c < columns; c++) {
            if (element == getElement(r, c)) {
                row = r;
                column = c;
                return true;
            }
        }
    }
    return false;
}
