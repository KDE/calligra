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
#include <stdlib.h>

#include <qpainter.h>

#include "bracketelement.h"
#include "elementtype.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "matrixelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "sequenceparser.h"
#include "symbolelement.h"
#include "textelement.h"


SequenceElement::SequenceElement(BasicElement* parent)
        : BasicElement(parent), parseTree(0), relativeSize(0)
{
    children.setAutoDelete(true);
}


SequenceElement::~SequenceElement()
{
    delete parseTree;
}


/**
 * Returns the element the point is in.
 */
BasicElement* SequenceElement::goToPos(FormulaCursor* cursor, bool& handled,
                                       const QPoint& point, const QPoint& parentOrigin)
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        QPoint myPos(parentOrigin.x() + getX(),
                     parentOrigin.y() + getY());
        
        uint count = children.count();
        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            e = child->goToPos(cursor, handled, point, myPos);
            if (e != 0) {
                if (!handled) {
                    handled = true;
                    cursor->setTo(this, children.find(e));
                }
                return e;
            }
        }

        int dx = point.x() - myPos.x();
        //int dy = point.y() - myPos.y();
        
        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            if (dx < child->getX()) {
                if (i > 0) {
                    cursor->setTo(this, i-1);
                    handled = true;
                    return children.at(i-1);
                }
                else {
                    break;
                }
            }
        }

        cursor->setTo(this, countChildren());
        handled = true;
        return this;
    }
    return 0;
}


void SequenceElement::setSizeReduction(const ContextStyle& context)
{
    relativeSize = context.getSizeReduction();
}


bool SequenceElement::isEmpty()
{
    uint count = children.count();
    for (uint i = 0; i < count; i++) {
        BasicElement* child = children.at(i);
        if (!child->isPhantom()) {
            return false;
        }
    }
    return true;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void SequenceElement::calcSizes(const ContextStyle& context, int parentSize)
{
    if (!isEmpty()) {
        int mySize = parentSize - relativeSize;
        int width = 0;
        int toBaseline = 0;
        int fromBaseline = 0;

        QFont font = context.getDefaultFont();
        font.setPointSize(mySize);
        QFontMetrics fm(font);
        int fromMidline = fm.strikeOutPos();

        uint count = children.count();

        // Let's do all normal elements that have a base line.
        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            if (!child->isPhantom()) {
                child->calcSizes(context, mySize);
                child->setX(width);
                width += child->getWidth();

                if (child->getBaseline() > -1) {
                    toBaseline = QMAX(toBaseline, child->getBaseline());
                    fromBaseline = QMAX(fromBaseline, child->getHeight()-child->getBaseline());
                }
            }
            else {
                child->setX(width);
            }
        }

        bool noBaseline = toBaseline == 0;
        
        // Now all normal elements without a base line.
        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            if (!child->isPhantom()) {
                if (child->getBaseline() == -1) {
                    toBaseline = QMAX(toBaseline, child->getMidline()+fromMidline);
                    fromBaseline = QMAX(fromBaseline, child->getHeight()-(child->getMidline()+fromMidline));
                }
            }
        }

        setWidth(width);
        setHeight(toBaseline+fromBaseline);
        setBaseline(noBaseline ? -1 : toBaseline);
        setMidline(toBaseline-fromMidline);

        setChildrenPositions();
    }
    else {
        int w = context.getEmptyRectWidth();
        int h = context.getEmptyRectHeight();
        setWidth( w );
        setHeight( h );
        setBaseline( h );
        setMidline( h/2 );
    }
}


void SequenceElement::setChildrenPositions()
{
    uint count = children.count();
    for (uint i = 0; i < count; i++) {
        BasicElement* child = children.at(i);
        if (child->getBaseline() > -1) {
            child->setY(getBaseline() - child->getBaseline());
        }
        else {
            child->setY(getMidline() - child->getMidline());
        }
    }
}


/**
 * Draws the whole element including its children.
 * The `parentOrigin' is the point this element's parent starts.
 * We can use our parentPosition to get our own origin then.
 */
void SequenceElement::draw(QPainter& painter, const QRect& r,
                           const ContextStyle& context,
                           int parentSize, const QPoint& parentOrigin)
{
    QPoint myPos(parentOrigin.x() + getX(),
                 parentOrigin.y() + getY());
    if (!QRect(myPos, getSize()).intersects(r))
        return;
    
    if (!isEmpty()) {
        int mySize = parentSize - relativeSize;
        uint count = children.count();
        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            if (!child->isPhantom()) {
                child->draw(painter, r, context, mySize, myPos);
            }
            // Debug
            //painter.setPen(Qt::green);
            //painter.drawRect(parentOrigin.x() + getX(), parentOrigin.y() + getY(),
            //                 getWidth(), getHeight());
        }
    }
    else {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(context.getEmptyColor(), context.getLineWidth()));
        painter.drawRect(myPos.x(), myPos.y(), getWidth(), getHeight());
    }
}


void SequenceElement::calcCursorSize(FormulaCursor* cursor, bool smallCursor)
{
    QPoint point = widgetPos();
    uint pos = cursor->getPos();

    int posX = getChildPosition(pos);
    int height = getHeight();

    if (cursor->isSelection()) {
        uint mark = cursor->getMark();
        int markX = getChildPosition(mark);
        int x = QMIN(posX, markX);
        int width = abs(posX - markX);
        
        if (smallCursor) {
            cursor->cursorSize.setRect(point.x()+x, point.y(), width, height);
        }
        else {
            cursor->cursorSize.setRect(point.x()+x, point.y()-2, width, height+4);
        }
    }
    else {
        if (smallCursor) {
            cursor->cursorSize.setRect(point.x()+posX, point.y(), 1, height);
        }
        else {
            cursor->cursorSize.setRect(point.x(), point.y()-2, getWidth()+1, height+6);
        }
    }

    cursor->cursorPoint.setX(point.x()+posX);
    cursor->cursorPoint.setY(point.y()+getHeight()/2);
}


/**
 * If the cursor is inside a sequence it needs to be drawn.
 */
void SequenceElement::drawCursor(FormulaCursor* cursor, QPainter& painter, bool smallCursor)
{
    painter.setRasterOp(Qt::XorROP);
    if (cursor->isSelection()) {
        painter.fillRect(cursor->cursorSize, Qt::white);
    }
    else {
        painter.setPen(Qt::white);
        QPoint point = cursor->cursorPoint;
        QRect size = cursor->cursorSize;
        if (smallCursor) {
            painter.drawLine(point.x(), size.top(),
                             point.x(), size.bottom());
        }
        else {
            painter.drawLine(point.x(), size.top(),
                             point.x(), size.bottom()-1);
            painter.drawLine(size.left(), size.bottom(),
                             size.right(), size.bottom());
        }
    }
    painter.setRasterOp(Qt::CopyROP);
}


int SequenceElement::getChildPosition(uint child)
{
    if (child < children.count()) {
        return children.at(child)->getX();
    }
    else {
        if (children.count() > 0) {
            return children.at(child-1)->getX() + children.at(child-1)->getWidth();
        }
        else {
            return 2;
        }
    }
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
void SequenceElement::moveLeft(FormulaCursor* cursor, BasicElement* from)
{
    // Our parent asks us for a cursor position. Found.
    if (from == getParent()) {
        cursor->setTo(this, children.count());
    }

    // We already owned the cursor. Ask next child then.
    else if (from == this) {
        if (cursor->getPos() > 0) {
            if (cursor->isSelectionMode()) {
                cursor->setTo(this, cursor->getPos()-1);

                // phantom elements are not visible so we move on.
                if (children.at(cursor->getPos())->isPhantom()) {
                    moveLeft(cursor, this);
                }
            }
            else {
                children.at(cursor->getPos()-1)->moveLeft(cursor, this);
            }
        }
        else {
            // Needed because FormulaElement derives this.
            if (getParent() != 0) {
                getParent()->moveLeft(cursor, this);
            }
        }
    }

    // The cursor came from one of our children or
    // something is wrong.
    else {
        int fromPos = children.find(from);
        cursor->setTo(this, fromPos);
        if (cursor->isSelectionMode()) {
            cursor->setMark(fromPos+1);
        }

        // phantom elements are not visible so we move on.
        if (from->isPhantom()) {
            moveLeft(cursor, this);
        }
    }
}

/**
 * Enters this element while moving to the right starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or to the right of it.
 */
void SequenceElement::moveRight(FormulaCursor* cursor, BasicElement* from)
{
    // Our parent asks us for a cursor position. Found.
    if (from == getParent()) {
        cursor->setTo(this, 0);
    }

    // We already owned the cursor. Ask next child then.
    else if (from == this) {
        uint pos = cursor->getPos();
        if (pos < children.count()) {
            if (cursor->isSelectionMode()) {
                cursor->setTo(this, pos+1);

                // phantom elements are not visible so we move on.
                if (children.at(pos)->isPhantom()) {
                    moveRight(cursor, this);
                }
            }
            else {
                children.at(pos)->moveRight(cursor, this);
            }
        }
        else {
            // Needed because FormulaElement derives this.
            if (getParent() != 0) {
                getParent()->moveRight(cursor, this);
            }
        }
    }

    // The cursor came from one of our children or
    // something is wrong.
    else {
        int fromPos = children.find(from);
        cursor->setTo(this, fromPos+1);
        if (cursor->isSelectionMode()) {
            cursor->setMark(fromPos);
        }

        // phantom elements are not visible so we move on.
        if (from->isPhantom()) {
            moveRight(cursor, this);
        }
    }
}


void SequenceElement::moveWordLeft(FormulaCursor* cursor)
{
    uint pos = cursor->getPos();
    if (pos > 0) {
        ElementType* type = children.at(pos-1)->getElementType();
        if (type != 0) {
            cursor->setTo(this, type->start());
        }
    }
    else {
        moveLeft(cursor, this);
    }
}


void SequenceElement::moveWordRight(FormulaCursor* cursor)
{
    uint pos = cursor->getPos();
    if (pos < children.count()) {
        ElementType* type = children.at(pos)->getElementType();
        if (type != 0) {
            cursor->setTo(this, type->end());
        }
    }
    else {
        moveRight(cursor, this);
    }
}


/**
 * Enters this element while moving up starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or above it.
 */
void SequenceElement::moveUp(FormulaCursor* cursor, BasicElement* from)
{
    if (from == getParent()) {
        moveRight(cursor, this);
    }
    else {
        if (getParent() != 0) {
            getParent()->moveUp(cursor, this);
        }
    }
}

/**
 * Enters this element while moving down starting inside
 * the element `from'. Searches for a cursor position inside
 * this element or below it.
 */
void SequenceElement::moveDown(FormulaCursor* cursor, BasicElement* from)
{
    if (from == getParent()) {
        moveRight(cursor, this);
    }
    else {
        if (getParent() != 0) {
            getParent()->moveDown(cursor, this);
        }
    }
}

/**
 * Moves the cursor to the first position in this sequence.
 * (That is before the first child.)
 */
void SequenceElement::moveHome(FormulaCursor* cursor)
{
    if (cursor->isSelectionMode()) {
        BasicElement* element = cursor->getElement();
        if (element != this) {
            while (element->getParent() != this) {
                element = element->getParent();
            }
            cursor->setMark(children.find(element)+1);
        }
    }
    cursor->setTo(this, 0);
}

/**
 * Moves the cursor to the last position in this sequence.
 * (That is behind the last child.)
 */
void SequenceElement::moveEnd(FormulaCursor* cursor)
{
    if (cursor->isSelectionMode()) {
        BasicElement* element = cursor->getElement();
        if (element != this) {
            while (element->getParent() != this) {
                element = element->getParent();
                if (element == 0) {
                    cursor->setMark(children.count());
                    break;
                }
            }
            if (element != 0) {
                cursor->setMark(children.find(element));
            }
        }
    }
    cursor->setTo(this, children.count());
}

/**
 * Sets the cursor inside this element to its start position.
 * For most elements that is the main child.
 */
void SequenceElement::goInside(FormulaCursor* cursor)
{
    cursor->setSelection(false);
    cursor->setTo(this, 0);
}


// children

/**
 * Removes the child. If this was the main child this element might
 * request its own removal.
 * The cursor is the one that caused the removal. It has to be moved
 * to the place any user expects the cursor after that particular
 * element has been removed.
 */
// void SequenceElement::removeChild(FormulaCursor* cursor, BasicElement* child)
// {
//     int pos = children.find(child);
//     formula()->elementRemoval(child, pos);
//     cursor->setTo(this, pos);
//     children.remove(pos);
//     /*
//         if len(self.children) == 0:
//             if self.parent != None:
//                 self.parent.removeChild(cursor, self)
//                 return
//     */
//     formula()->changed();
// }


/**
 * Inserts all new children at the cursor position. Places the
 * cursor according to the direction. The inserted elements will
 * be selected.
 *
 * The list will be emptied but stays the property of the caller.
 */
void SequenceElement::insert(FormulaCursor* cursor,
                             QList<BasicElement>& newChildren,
                             Direction direction)
{
    int pos = cursor->getPos();
    uint count = newChildren.count();
    for (uint i = 0; i < count; i++) {
        BasicElement* child = newChildren.take(0);
        child->setParent(this);
        children.insert(pos+i, child);
    }
    if (direction == beforeCursor) {
        cursor->setTo(this, pos+count, pos);
    }
    else {
        cursor->setTo(this, pos, pos+count);
    }
    formula()->changed();
    parse();
}


/**
 * Removes all selected children and returns them. Places the
 * cursor to where the children have been.
 *
 * The ownership of the list is passed to the caller.
 */
void SequenceElement::remove(FormulaCursor* cursor,
                             QList<BasicElement>& removedChildren,
                             Direction direction)
{
    if (cursor->isSelection()) {
        int from = cursor->getSelectionStart();
        int to = cursor->getSelectionEnd();
        for (int i = from; i < to; i++) {
            removeChild(removedChildren, from);
        }
        cursor->setTo(this, from);
        cursor->setSelection(false);
    }
    else {
        if (direction == beforeCursor) {
            int pos = cursor->getPos() - 1;
            if (pos >= 0) {
                while (pos >= 0) {
                    BasicElement* child = children.at(pos);
                    formula()->elementRemoval(child);
                    children.take(pos);
                    removedChildren.prepend(child);
                    if (!child->isPhantom()) {
                        break;
                    }
                    pos--;
                }
                cursor->setTo(this, pos);
                formula()->changed();
            }
        }
        else {
            uint pos = cursor->getPos();
            if (pos < children.count()) {
                while (pos < children.count()) {
                    BasicElement* child = children.at(pos);
                    formula()->elementRemoval(child);
                    children.take(pos);
                    removedChildren.append(child);
                    if (!child->isPhantom()) {
                        break;
                    }
                }
                // It is necessary to set the cursor to its old
                // position because it got a notification and
                // moved to the beginning of this sequence.
                cursor->setTo(this, pos);
                formula()->changed();
            }
        }
    }
    parse();
}


/**
 * Removes the children at pos and appends it to the list.
 */
void SequenceElement::removeChild(QList<BasicElement>& removedChildren, int pos)
{
    BasicElement* child = children.at(pos);
    formula()->elementRemoval(child);
    children.take(pos);
    removedChildren.append(child);
    //cerr << *removedChildren.at(0) << endl;
    formula()->changed();
}


/**
 * Moves the cursor to a normal place where new elements
 * might be inserted.
 */
void SequenceElement::normalize(FormulaCursor* cursor, Direction)
{
    cursor->setSelection(false);
}


/**
 * Returns the child at the cursor.
 * Does not care about the selection.
 */
BasicElement* SequenceElement::getChild(FormulaCursor* cursor, Direction direction)
{
    if (direction == beforeCursor) {
        int pos = cursor->getPos()-1;
        if (pos >= 0) {
            return children.at(pos);
        }
    }
    else {
        uint pos = cursor->getPos();
        if (pos < children.count()) {
            return children.at(pos);
        }
    }
    return 0;
}


/**
 * Sets the cursor to select the child. The mark is placed before,
 * the position behind it.
 */
void SequenceElement::selectChild(FormulaCursor* cursor, BasicElement* child)
{
    int pos = children.find(child);
    if (pos > -1) {
        cursor->setTo(this, pos+1, pos);
    }
}


QString SequenceElement::getCurrentName(FormulaCursor* cursor)
{
    uint pos = cursor->getPos();
    if (pos > 0) {
        ElementType* type = children.at(pos-1)->getElementType();
        if (type != 0) {
            QString name = type->getName();
            if (!name.isNull()) {
                cursor->setTo(this, type->start(), type->end());
                return name;
            }
        }
        if (pos == children.count()) {
            bool linear = cursor->getLinearMovement();
            cursor->setLinearMovement(false);
            cursor->moveRight();
            cursor->setLinearMovement(linear);
        }
    }
    return QString::null;
}


/**
 * Selects all children. The cursor is put behind, the mark before them.
 */
void SequenceElement::selectAllChildren(FormulaCursor* cursor)
{
    cursor->setTo(this, children.count(), 0);
}


/**
 * Stores the given childrens dom in the element.
 */
void SequenceElement::getChildrenDom(QDomDocument& doc, QDomElement& elem,
                                     uint from, uint to)
{
    for (uint i = from; i < to; i++) {
        QDomElement tmpEleDom=children.at(i)->getElementDom(doc);
	elem.appendChild(tmpEleDom);
    }
}


/**
 * Builds elements from the given node and its siblings and
 * puts them into the list.
 * Returns false if an error occures.
 */
bool SequenceElement::buildChildrenFromDom(QList<BasicElement>& list, QDomNode n)
{
    while (!n.isNull()) {
        if (n.isElement()) {
            QDomElement e = n.toElement();
            BasicElement* child = 0;
            QString tag = e.tagName().upper();

            child = createElement(tag);
            if (child != 0) {
                child->setParent(this);
                if (child->buildFromDom(e)) {
                    list.append(child);
                }
                else {
                    delete child;
                    return false;
                }
            }
            else {
                return false;
            }
        }
        n = n.nextSibling();
    }
    parse();
    return true;
}


BasicElement* SequenceElement::createElement(QString type)
{
    if      (type == "TEXT")       return new TextElement();
    else if (type == "ROOT")       return new RootElement();
    else if (type == "BRACKET")    return new BracketElement();
    else if (type == "MATRIX")     return new MatrixElement();
    else if (type == "INDEX")      return new IndexElement();
    else if (type == "FRACTION")   return new FractionElement();
    else if (type == "SYMBOL")     return new SymbolElement();
    else if (type == "SEQUENCE") {
        cerr << "malformed data: sequence inside sequence.\n";
        return 0;
    }
    return 0;
}

/**
 * Appends our attributes to the dom element.
 */
void SequenceElement::writeDom(QDomElement& element)
{
    BasicElement::writeDom(element);

    uint count = children.count();
    QDomDocument doc = element.ownerDocument();
    getChildrenDom(doc, element, 0, count);
}
    
/**
 * Reads our attributes from the element.
 * Returns false if it failed.
 */
bool SequenceElement::readAttributesFromDom(QDomElement& element)
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
bool SequenceElement::readContentFromDom(QDomNode& node)
{
    if (!BasicElement::readContentFromDom(node)) {
        return false;
    }
    
    return buildChildrenFromDom(children, node);
}


void SequenceElement::parse()
{
    delete parseTree;

    // Those types are gone. Make sure they won't
    // be used.
    for (uint i = 0; i < children.count(); i++) {
        children.at(i)->setElementType(0);
    }

    const SymbolTable& symbols = formula()->getSymbolTable();
    SequenceParser parser(symbols);
    parseTree = parser.parse(children);

    // debug
    //parseTree->output();
}

QString SequenceElement::toLatex()
{
    QString content;
    if (!isEmpty()) {
    
        uint count = children.count();
	if(count > 1) content+="{";
        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            if (child->isPhantom())
	    {
		content+="\\phantom{";
                content+=child->toLatex();
		content+="}";
    	    }
	    else
		content+=child->toLatex();
	
    	if(count>1)
	    content+=" ";             
	
	}
    	if(count > 1) content+="}";

    }
    return content;
}