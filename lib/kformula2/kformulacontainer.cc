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

#include <qpainter.h>
#include <qevent.h>
#include <qstring.h>

#include "bracketelement.h"
#include "contextstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "kformulacontainer.h"
#include "matrixelement.h"
#include "operatorelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "symbolelement.h"
#include "textelement.h"


KFormulaContainer::KFormulaContainer()
    : rootElement(this)
{
    dirty = true;
    testDirty();
}

KFormulaContainer::~KFormulaContainer()
{
}


FormulaCursor* KFormulaContainer::createCursor()
{
    FormulaCursor* cursor = new FormulaCursor(&rootElement);
    connect(this, SIGNAL(elementWillVanish(BasicElement*)),
            cursor, SLOT(elementWillVanish(BasicElement*)));
    return cursor;
}

void KFormulaContainer::destroyCursor(FormulaCursor* cursor)
{
    delete cursor;
}


/**
 * Gets called just before the child is removed from
 * the element tree.
 */
void KFormulaContainer::elementRemoval(BasicElement* child)
{
    emit elementWillVanish(child);
}

/**
 * Gets called whenever something changes and we need to
 * recalc.
 */
void KFormulaContainer::changed()
{
    dirty = true;
}


/**
 * Draws the whole thing.
 */
void KFormulaContainer::draw(QPainter& painter)
{
    rootElement.draw(painter, context);
}


void KFormulaContainer::addText(FormulaCursor* cursor, QChar ch)
{
    if (cursor->isSelection()) {
        removeSelection(cursor, BasicElement::beforeCursor);
    }
    QList<BasicElement> list;
    list.setAutoDelete(true);
    list.append(new TextElement(ch));
    cursor->insert(list);
    cursor->setSelection(false);
}

void KFormulaContainer::addOperator(FormulaCursor* cursor, QChar ch)
{
    QList<BasicElement> list;
    list.setAutoDelete(true);
    list.append(new OperatorElement(ch));
    cursor->insert(list);
    cursor->setSelection(false);
}

void KFormulaContainer::addBracket(FormulaCursor* cursor, char left, char right)
{
    BracketElement* bracket = new BracketElement(left, right);
    if (cursor->isSelection()) {
        cursor->replaceSelectionWith(bracket);
    }
    else {
        cursor->insert(bracket);
        //cursor->setSelection(false);
    }
    cursor->goInsideElement(bracket);
}

void KFormulaContainer::addFraction(FormulaCursor* cursor)
{
    FractionElement* fraction = new FractionElement;
    if (cursor->isSelection()) {
        cursor->replaceSelectionWith(fraction);
    }
    else {
        cursor->insert(fraction);
        //cursor->setSelection(false);
    }
    cursor->goInsideElement(fraction);
}

void KFormulaContainer::addRoot(FormulaCursor* cursor)
{
    RootElement* root = new RootElement();
    if (cursor->isSelection()) {
        cursor->replaceSelectionWith(root);
    }
    else {
        cursor->insert(root);
        //cursor->setSelection(false);
    }
    cursor->goInsideElement(root);
}

void KFormulaContainer::addSymbol(FormulaCursor* cursor,
                                  Artwork::SymbolType type)
{
    SymbolElement* symbol = new SymbolElement(type);
    if (cursor->isSelection()) {
        cursor->replaceSelectionWith(symbol);
    }
    else {
        cursor->insert(symbol);
        //cursor->setSelection(false);
    }
    cursor->goInsideElement(symbol);
}

void KFormulaContainer::addMatrix(FormulaCursor* cursor, int rows, int columns)
{
    MatrixElement* matrix = new MatrixElement(rows, columns);
    cursor->insert(matrix);
    cursor->goInsideElement(matrix);
}

void KFormulaContainer::addLowerRightIndex(FormulaCursor* cursor)
{
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;
        cursor->replaceSelectionWith(element, BasicElement::beforeCursor);
    }

    if (!element->hasLowerRight()) {
        SequenceElement* index = new SequenceElement;
        element->setToLowerRight(cursor);
        cursor->insert(index);
        //cursor->goInsideElement(index);
    }
    else {
        element->moveToLowerRight(cursor, BasicElement::afterCursor);
    }
}

void KFormulaContainer::addUpperRightIndex(FormulaCursor* cursor)
{
    IndexElement* element = cursor->getActiveIndexElement();
    if (element == 0) {
        element = new IndexElement;
        cursor->replaceSelectionWith(element, BasicElement::beforeCursor);
    }

    if (!element->hasUpperRight()) {
        SequenceElement* index = new SequenceElement;
        element->setToUpperRight(cursor);
        cursor->insert(index);
        //cursor->goInsideElement(index);
    }
    else {
        element->moveToUpperRight(cursor, BasicElement::afterCursor);
    }
}

void KFormulaContainer::removeSelection(FormulaCursor* cursor,
                                        BasicElement::Direction direction)
{
    QList<BasicElement> list;
    list.setAutoDelete(true);
    cursor->remove(list, direction);
    if (cursor->elementIsSenseless()) {
        BasicElement* element = cursor->replaceByMainChildContent();
        delete element;
    }
    cursor->normalize(direction);
}


void KFormulaContainer::testDirty()
{
    if (dirty) {
        dirty = false;
        rootElement.calcSizes(context);
        emit formulaChanged();
    }
}
