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

#include "formulacursor.h"
#include "formulaelement.h"
#include "indexelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "symbolelement.h"
#include "textelement.h"


FormulaCursor::FormulaCursor(FormulaElement* element)
        : selectionFlag(false), linearMovement(false),
          hasChangedFlag(true), readOnly(false)
{
    setTo(element, 0);
}

void FormulaCursor::setTo(BasicElement* element, int cursor, int mark)
{
    hasChangedFlag = true;
    current = element;
    cursorPos = cursor;
    if ((mark == -1) && selectionFlag) {
        return;
    }
    if (mark != -1) {
        setSelection(true);
    }
    markPos = mark;
}


void FormulaCursor::setPos(int pos)
{
    hasChangedFlag = true;
    cursorPos = pos;
}

void FormulaCursor::setMark(int mark)
{
    hasChangedFlag = true;
    markPos = mark;
}

void FormulaCursor::calcCursorSize(bool smallCursor)
{
    // We only draw the cursor if its normalized.
    SequenceElement* sequence = dynamic_cast<SequenceElement*>(current);

    if (sequence != 0) {
        sequence->calcCursorSize(this, smallCursor);
    }
}

void FormulaCursor::draw(QPainter& painter, bool smallCursor)
{
    if (readOnly && !isSelection())
        return;
    
    // We only draw the cursor if its normalized.
    SequenceElement* sequence = dynamic_cast<SequenceElement*>(current);

    if (sequence != 0) {
        sequence->drawCursor(this, painter, smallCursor);
    }
}


void FormulaCursor::handleSelectState(int flag)
{
    if (flag & SelectMovement) {
        if (!isSelection()) {
            setMark(getPos());
            setSelection(true);
        }
    }
    else {
        setSelection(false);
    }
}

void FormulaCursor::moveLeft(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    if (flag & WordMovement) {
        SequenceElement* sequence = dynamic_cast<SequenceElement*>(current);
        if (sequence != 0) {
            sequence->moveWordLeft(this);
        }
        else {
            element->moveHome(this);
        }
    }
    else {
        element->moveLeft(this, element);
    }
}

void FormulaCursor::moveRight(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    if (flag & WordMovement) {
        SequenceElement* sequence = dynamic_cast<SequenceElement*>(current);
        if (sequence != 0) {
            sequence->moveWordRight(this);
        }
        else {
            element->moveEnd(this);
        }
    }
    else {
        element->moveRight(this, element);
    }
}

void FormulaCursor::moveUp(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    element->moveUp(this, element);
}

void FormulaCursor::moveDown(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    element->moveDown(this, element);
}

void FormulaCursor::moveHome(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    if (flag & WordMovement) {
        element->formula()->moveHome(this);
    }
    else {
        element->moveHome(this);
    }
}

void FormulaCursor::moveEnd(int flag)
{
    BasicElement* element = getElement();
    handleSelectState(flag);
    if (flag & WordMovement) {
        element->formula()->moveEnd(this);
    }
    else {
        element->moveEnd(this);
    }
}


void FormulaCursor::mousePress(const QPoint& pos, int flag)
{
    FormulaElement* formula = getElement()->formula();
    formula->goToPos(this, pos);
    if (flag & SelectMovement) {
        setSelection(true);
        if (getMark() == -1) {
            setMark(getPos());
        }
    }
    else {
        setSelection(false);
        setMark(getPos());
    }
}

void FormulaCursor::mouseMove(const QPoint& point, int)
{
    setSelection(true);
    BasicElement* element = getElement();
    int mark = getMark();

    FormulaElement* formula = getElement()->formula();
    formula->goToPos(this, point);
    BasicElement* newElement = getElement();
    int pos = getPos();

    BasicElement* posChild = 0;
    BasicElement* markChild = 0;
    while (element != newElement) {
        posChild = newElement;
        newElement = newElement->getParent();
        if (newElement == 0) {
            posChild = 0;
            newElement = getElement();
            markChild = element;
            element = element->getParent();
        }
    }

    if (dynamic_cast<SequenceElement*>(element) == 0) {
        element = element->getParent();
        element->selectChild(this, newElement);
    }
    else {
        if (posChild != 0) {
            element->selectChild(this, posChild);
            pos = getPos();
        }
        if (markChild != 0) {
            element->selectChild(this, markChild);
            mark = getMark();
        }
        if (pos == mark) {
            if ((posChild == 0) && (markChild != 0)) {
                mark++;
            }
            else if ((posChild != 0) && (markChild == 0)) {
                mark--;
            }
        }
        else if (pos < mark) {
            if (posChild != 0) {
                pos--;
            }
        }
        setTo(element, pos, mark);
    }
}

void FormulaCursor::mouseRelease(const QPoint&, int)
{
    //mouseSelectionFlag = false;
}


/**
 * Moves the cursor inside the element. Selection is turned off.
 */
void FormulaCursor::goInsideElement(BasicElement* element)
{
    element->goInside(this);
}


/**
 * Moves the cursor to a normal position. That is somewhere
 * inside a SequenceElement.
 * You need to call this after each removal because the cursor
 * might point to some non existing place.
 */
void FormulaCursor::normalize(BasicElement::Direction direction)
{
    BasicElement* element = getElement();
    element->normalize(this, direction);
}


/**
 * Inserts the child at the current position.
 * Ignores the selection.
 */
void FormulaCursor::insert(BasicElement* child, BasicElement::Direction direction)
{
    QList<BasicElement> list;
    list.append(child);
    insert(list, direction);
}

void FormulaCursor::insert(QList<BasicElement>& children,
                           BasicElement::Direction direction)
{
    if (readOnly)
        return;
    BasicElement* element = getElement();
    element->insert(this, children, direction);
}


/**
 * Removes the current selected children and returns them.
 * The cursor needs to be normal (that is be inside a SequenceElement)
 * for this to have any effect.
 */
void FormulaCursor::remove(QList<BasicElement>& children,
                           BasicElement::Direction direction)
{
    if (readOnly)
        return;
    SequenceElement* sequence = getNormal();
    if (sequence != 0) {

        // If there is no child to remove in the sequence
        // remove the sequence instead.
        if (sequence->countChildren() == 0) {
            BasicElement* parent = sequence->getParent();
            if (parent != 0) {
                parent->selectChild(this, sequence);
                parent->remove(this, children, direction);
                return;
            }
        }
        else {
            sequence->remove(this, children, direction);
        }
    }
}


/**
 * Replaces the current selection with the supplied element.
 * The replaced elements become the new element's main child's content.
 */
void FormulaCursor::replaceSelectionWith(BasicElement* element,
                                         BasicElement::Direction direction)
{
    if (readOnly)
        return;
    QList<BasicElement> list;
    // we suppres deletion here to get an error if something
    // was left in the list.
    //list.setAutoDelete(true);
    
    //remove(list, direction);
    if (isSelection()) {
        getElement()->remove(this, list, direction);
    }
    
    insert(element, direction);
    SequenceElement* mainChild = element->getMainChild();
    if (mainChild != 0) {
        mainChild->goInside(this);
        insert(list);
        /*
        BasicElement* parent = element->getParent();
        if (direction == BasicElement::beforeCursor) {
            parent->moveRight(this, element);
        }
        else {
            parent->moveLeft(this, element);
        }
        */
        element->selectChild(this, mainChild);
    }
}


/**
 * Replaces the element the cursor points to with its main child's
 * content.
 */
BasicElement* FormulaCursor::replaceByMainChildContent(BasicElement::Direction direction)
{
    if (readOnly)
        return 0;
    QList<BasicElement> childrenList;
    QList<BasicElement> list;
    BasicElement* element = getElement();
    SequenceElement* mainChild = element->getMainChild();
    if ((mainChild != 0) && (mainChild->countChildren() > 0)) {
        mainChild->selectAllChildren(this);
        remove(childrenList);
    }
    element->getParent()->moveRight(this, element);
    setSelection(false);
    remove(list);
    insert(childrenList, direction);
    if (list.count() > 0) {
        return list.take(0);
    }
    return 0;
}


/**
 * Trys to find the element we are the main child of and replace
 * it with our content.
 *
 * This is simply another form of replaceByMainChildContent. You
 * use this one if the cursor is normalized and inside the main child.
 */
BasicElement* FormulaCursor::removeEnclosingElement(BasicElement::Direction direction)
{
    if (readOnly)
        return 0;
    BasicElement* parent = getElement()->getParent();
    if (parent != 0) {
        if (getElement() == parent->getMainChild()) {
            parent->selectChild(this, getElement());
            return replaceByMainChildContent(direction);
        }
    }
    return 0;
}


/**
 * Returns wether the element the cursor points to should be replaced.
 * Elements are senseless as soon as they only contain a main child.
 */
bool FormulaCursor::elementIsSenseless()
{
    BasicElement* element = getElement();
    return element->isSenseless();
}


/**
 * Returns the child the cursor points to. Depending on the
 * direction this might be the child before or after the
 * cursor.
 *
 * Might be 0 is there is no such child.
 */
BasicElement* FormulaCursor::getActiveChild(BasicElement::Direction direction)
{
    return getElement()->getChild(this, direction);
}

BasicElement* FormulaCursor::getSelectedChild()
{
    if (isSelection()) {
        if ((getSelectionEnd() - getSelectionStart()) > 1) {
            return 0;
        }
        return getActiveChild((getPos() > getMark()) ?
                              BasicElement::beforeCursor :
                              BasicElement::afterCursor);
    }
    else {
        return getActiveChild(BasicElement::beforeCursor);
    }
}

/**
 * Tells whether we currently point to the given elements
 * main child and to the place behind its last child.
 */
bool FormulaCursor::pointsAfterMainChild(BasicElement* element)
{
    if (element != 0) {
        SequenceElement* mainChild = element->getMainChild();
        return (getElement() == mainChild) &&
            ((mainChild->countChildren() == getPos()) || (0 == getPos()));
    }
    return false;
}


/**
 * Returns the IndexElement the cursor is on or 0
 * if there is non.
 */
IndexElement* FormulaCursor::getActiveIndexElement()
{
    IndexElement* element = dynamic_cast<IndexElement*>(getSelectedChild());

    if ((element == 0) && !isSelection()) {
        element = dynamic_cast<IndexElement*>(getElement()->getParent());
        if (!pointsAfterMainChild(element)) {
            return 0;
        }
    }
    return element;
}


/**
 * Returns the RootElement the cursor is on or 0
 * if there is non.
 */
RootElement* FormulaCursor::getActiveRootElement()
{
    RootElement* element = dynamic_cast<RootElement*>(getSelectedChild());

    if ((element == 0) && !isSelection()) {
        element = dynamic_cast<RootElement*>(getElement()->getParent());
        if (!pointsAfterMainChild(element)) {
            return 0;
        }
    }
    return element;
}


/**
 * @returns the SymbolElement the cursor is on or 0
 * if there is non.
 */
SymbolElement* FormulaCursor::getActiveSymbolElement()
{
    SymbolElement* element = dynamic_cast<SymbolElement*>(getSelectedChild());

    if ((element == 0) && !isSelection()) {
        element = dynamic_cast<SymbolElement*>(getElement()->getParent());
        if (!pointsAfterMainChild(element)) {
            return 0;
        }
    }
    return element;
}


/**
 * @returns the TextElement the cursor is on or 0.
 */
TextElement* FormulaCursor::getActiveTextElement()
{
    return dynamic_cast<TextElement*>(getSelectedChild());
}


QString FormulaCursor::getCurrentName()
{
    if (isSelection()) {
        return QString::null;
    }
    SequenceElement* sequence = getNormal();
    if (sequence != 0) {
        return sequence->getCurrentName(this);
    }
    return QString::null;
}


/**
 * The element is going to leave the formula with and all its children.
 */
void FormulaCursor::elementWillVanish(BasicElement* element)
{
    BasicElement* child = getElement();
    if (child == element->getParent()) {
        child->childWillVanish(this, element);
        return;
    }
    while (child != 0) {
        if (child == element) {
            // This is meant to catch all cursors that did not
            // cause the deletion.
            child->getParent()->moveLeft(this, child);
            setSelection(false);
            return;
        }
        child = child->getParent();
    }
}


/**
 * A new formula has been loaded. Our current element has to change.
 */
void FormulaCursor::formulaLoaded(FormulaElement* rootElement)
{
    current = rootElement;
    setPos(0);
    setMark(-1);
    setSelection(false);
}

/**
 * Stores the currently selected elements inside a dom.
 */
QDomDocument FormulaCursor::copy()
{
    QDomDocument doc("KFORMULA");
    QDomElement de = doc.createElement("KFORMULACOPY");
    doc.appendChild(de);
    if (isSelection()) {
        SequenceElement* sequence = getNormal();
        if (sequence != 0) {
            sequence->getChildrenDom(doc, de, getSelectionStart(), getSelectionEnd());
        }
        else {
            // This must never happen.
            qFatal("A not normalized cursor is selection in.");
        }
    }
    return doc;
}

/**
 * Inserts the elements that could be read from the dom into
 * the list. Returns true on success.
 */
bool FormulaCursor::buildElementsFromDom(QDomDocument doc, QList<BasicElement>& list)
{
    if (readOnly)
        return false;
    SequenceElement* sequence = getNormal();
    if (sequence != 0) {
        QDomNode n = doc.firstChild();
        if (n.isElement()) {
            QDomElement e = n.toElement();
            if (sequence->buildChildrenFromDom(list, e.firstChild())) {
                return true;
            }
        }
    }
    return false;
}


/**
 * Creates a new CursorData object that describes the cursor.
 * It's up to the caller to delete this object.
 */
FormulaCursor::CursorData* FormulaCursor::getCursorData()
{
    return new CursorData(current, cursorPos, markPos,
                          selectionFlag, linearMovement, readOnly);
}


// Keep in sync with 'setCursorData'
FormulaCursor& FormulaCursor::operator= (const FormulaCursor& other)
{
    current = other.current;
    cursorPos = other.cursorPos;
    markPos = other.markPos;
    selectionFlag = other.selectionFlag;
    linearMovement = other.linearMovement;
    readOnly = other.readOnly;
    hasChangedFlag = true;
    return *this;
}


/**
 * Sets the cursor to where the CursorData points to. No checking is done
 * so you better make sure the point exists.
 */
void FormulaCursor::setCursorData(FormulaCursor::CursorData* data)
{
    current = data->current;
    cursorPos = data->cursorPos;
    markPos = data->markPos;
    selectionFlag = data->selectionFlag;
    linearMovement = data->linearMovement;
    readOnly = data->readOnly;
    hasChangedFlag = true;
}


/**
 * Returns the sequence the cursor is in if we are normal. If not returns 0.
 */
SequenceElement* FormulaCursor::getNormal()
{
    return dynamic_cast<SequenceElement*>(current);
}
