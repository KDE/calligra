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

#include <stdlib.h>
#include <math.h>

#include <qpainter.h>
#include <qpaintdevice.h>

#include <kdebug.h>

#include "bracketelement.h"
#include "elementtype.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "fractionelement.h"
#include "indexelement.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "matrixelement.h"
#include "rootelement.h"
#include "sequenceelement.h"
#include "sequenceparser.h"
#include "spaceelement.h"
#include "symbolelement.h"
#include "symboltable.h"
#include "textelement.h"


KFORMULA_NAMESPACE_BEGIN
using namespace std;


SequenceElement::SequenceElement(BasicElement* parent)
        : BasicElement(parent), parseTree(0), textSequence(true)
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
BasicElement* SequenceElement::goToPos( FormulaCursor* cursor, bool& handled,
                                        const LuPoint& point, const LuPoint& parentOrigin )
{
    BasicElement* e = BasicElement::goToPos(cursor, handled, point, parentOrigin);
    if (e != 0) {
        LuPoint myPos(parentOrigin.x() + getX(),
                      parentOrigin.y() + getY());

        uint count = children.count();
        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            e = child->goToPos(cursor, handled, point, myPos);
            if (e != 0) {
                if (!handled) {
                    handled = true;
                    if ((point.x() - myPos.x()) < (e->getX() + e->getWidth()*2/3)) {
                        cursor->setTo(this, children.find(e));
                    }
                    else {
                        cursor->setTo(this, children.find(e)+1);
                    }
                }
                return e;
            }
        }

        double dx = point.x() - myPos.x();
        //int dy = point.y() - myPos.y();

        for (uint i = 0; i < count; i++) {
            BasicElement* child = children.at(i);
            if (dx < child->getX()) {
                cursor->setTo( this, i );
                handled = true;
                return children.at( i );
            }
        }

        cursor->setTo(this, countChildren());
        handled = true;
        return this;
    }
    return 0;
}


bool SequenceElement::isEmpty()
{
    uint count = children.count();
    for (uint i = 0; i < count; i++) {
        BasicElement* child = children.at(i);
        if (!child->isInvisible()) {
            return false;
        }
    }
    return true;
}


/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void SequenceElement::calcSizes(const ContextStyle& context, ContextStyle::TextStyle tstyle, ContextStyle::IndexStyle istyle)
{
    if (!isEmpty()) {
        lu mySize = context.getAdjustedSize( tstyle );
        lu width = 0;
        lu toBaseline = 0;
        lu fromBaseline = 0;

        QFont font = context.getDefaultFont();
        font.setPointSizeFloat(mySize);
        QFontMetrics fm(font);
        lu fromMidline = fm.strikeOutPos();

        //uint count = children.count();

        // Let's do all normal elements that have a base line.
        QPtrListIterator<BasicElement> it( children );
        for ( ; it.current(); ++it ) {
            BasicElement* child = it.current();

            lu spaceBefore = 0;
            if ( isFirstOfToken( child ) ) {
                spaceBefore = child->getElementType()->getSpaceBefore( context, tstyle );
            }

            if ( !child->isInvisible() ) {
                child->calcSizes( context, tstyle, istyle );
                child->setX( width + spaceBefore );
                width += child->getWidth() + spaceBefore;

                if ( child->getBaseline() > -1 ) {
                    toBaseline = QMAX( toBaseline, child->getBaseline() );
                    fromBaseline = QMAX( fromBaseline, child->getHeight()-child->getBaseline() );
                }
            }
            else {
                width += spaceBefore;
                child->setX( width );
            }
        }

        bool noBaseline = toBaseline == 0;

        // Now all normal elements without a base line.
        it.toFirst();
        for ( ; it.current(); ++it ) {
            BasicElement* child = it.current();
            if (!child->isInvisible()) {
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
        lu w = context.getEmptyRectWidth();
        lu h = context.getEmptyRectHeight();
        setWidth( w );
        setHeight( h );
        setBaseline( h );
        setMidline( h*.5 );
    }
}


void SequenceElement::setChildrenPositions()
{
    QPtrListIterator<BasicElement> it( children );
    for ( ; it.current(); ++it ) {
        BasicElement* child = it.current();
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
void SequenceElement::draw( QPainter& painter, const LuRect& r,
                            const ContextStyle& context,
                            ContextStyle::TextStyle tstyle,
                            ContextStyle::IndexStyle istyle,
                            const LuPoint& parentOrigin )
{
    LuPoint myPos( parentOrigin.x() + getX(), parentOrigin.y() + getY() );
    if ( !LuRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
        return;

    if (!isEmpty()) {
        QPtrListIterator<BasicElement> it( children );
        for ( ; it.current(); ++it ) {
            BasicElement* child = it.current();
	    if (!child->isInvisible()) {
                child->draw(painter, r, context, tstyle, istyle, myPos);
            }
	    // Debug
            //painter.setPen(Qt::green);
            //painter.drawRect(parentOrigin.x() + getX(), parentOrigin.y() + getY(),
            //                 getWidth(), getHeight());
        }
    }
    else {
        drawEmptyRect( painter, context, myPos );
    }
}

void SequenceElement::drawEmptyRect( QPainter& painter, const ContextStyle& context,
                                     const LuPoint& upperLeft )
{
    if ( painter.device()->devType() != QInternal::Printer ) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen( QPen( context.getEmptyColor(),
                              context.layoutUnitToPixelX( context.getLineWidth() ) ) );
        painter.drawRect( context.layoutUnitToPixelX( upperLeft.x() ),
                          context.layoutUnitToPixelY( upperLeft.y() ),
                          context.layoutUnitToPixelX( getWidth() ),
                          context.layoutUnitToPixelY( getHeight() ) );
    }
}

void SequenceElement::calcCursorSize( const ContextStyle& context,
                                      FormulaCursor* cursor, bool smallCursor )
{
    LuPoint point = widgetPos();
    uint pos = cursor->getPos();

    lu posX = getChildPosition( context, pos );
    lu height = getHeight();

    lu unit = context.ptToLayoutUnitPt( 1 );

    // Here are those evil constants that describe the cursor size.

    if ( cursor->isSelection() ) {
        uint mark = cursor->getMark();
        lu markX = getChildPosition( context, mark );
        lu x = QMIN(posX, markX);
        lu width = abs(posX - markX);

        if ( smallCursor ) {
            cursor->cursorSize.setRect( point.x()+x, point.y(), width, height );
        }
        else {
            cursor->cursorSize.setRect( point.x()+x, point.y() - 2*unit,
                                        width + unit, height + 4*unit );
        }
        cursor->selectionArea = cursor->cursorSize;
    }
    else {
        if ( smallCursor ) {
            cursor->cursorSize.setRect( point.x()+posX, point.y(),
                                        unit, height );
        }
        else {
            cursor->cursorSize.setRect( point.x(), point.y() - 2*unit,
                                        getWidth() + unit, height + 4*unit );
        }
    }

    cursor->cursorPoint.setX( point.x()+posX );
    cursor->cursorPoint.setY( point.y()+getHeight()/2 );
}


/**
 * If the cursor is inside a sequence it needs to be drawn.
 */
void SequenceElement::drawCursor( QPainter& painter, const ContextStyle& context,
                                  FormulaCursor* cursor, bool smallCursor )
{
    painter.setRasterOp( Qt::XorROP );
    if ( cursor->isSelection() ) {
        const LuRect& r = cursor->selectionArea;
        painter.fillRect( context.layoutUnitToPixelX( r.x() ),
                          context.layoutUnitToPixelY( r.y() ),
                          context.layoutUnitToPixelX( r.width() ),
                          context.layoutUnitToPixelY( r.height() ),
                          Qt::white );
    }
    else {
        painter.setPen( QPen( Qt::white,
                              context.layoutUnitToPixelX( context.getLineWidth()/2 ) ) );
        const LuPoint& point = cursor->getCursorPoint();
        const LuRect& size = cursor->getCursorSize();
        if ( smallCursor ) {
            painter.drawLine( context.layoutUnitToPixelX( point.x() ),
                              context.layoutUnitToPixelY( size.top() ),
                              context.layoutUnitToPixelX( point.x() ),
                              context.layoutUnitToPixelY( size.bottom() ) );
        }
        else {
            painter.drawLine( context.layoutUnitToPixelX( point.x() ),
                              context.layoutUnitToPixelY( size.top() ),
                              context.layoutUnitToPixelX( point.x() ),
                              context.layoutUnitToPixelY( size.bottom()-1 ) );
            painter.drawLine( context.layoutUnitToPixelX( size.left() ),
                              context.layoutUnitToPixelY( size.bottom() ),
                              context.layoutUnitToPixelX( size.right() ),
                              context.layoutUnitToPixelY( size.bottom() ) );
        }
    }
    // This might be wrong but probably isn't.
    painter.setRasterOp( Qt::CopyROP );
}


lu SequenceElement::getChildPosition( const ContextStyle& context, uint child )
{
    if (child < children.count()) {
        return children.at(child)->getX();
    }
    else {
        if (children.count() > 0) {
            return children.at(child-1)->getX() + children.at(child-1)->getWidth();
        }
        else {
            return context.ptToLayoutUnitPt( 2 );
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

                // invisible elements are not visible so we move on.
                if (children.at(cursor->getPos())->isInvisible()) {
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

        // invisible elements are not visible so we move on.
        if (from->isInvisible()) {
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

                // invisible elements are not visible so we move on.
                if (children.at(pos)->isInvisible()) {
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

        // invisible elements are not visible so we move on.
        if (from->isInvisible()) {
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
                             QPtrList<BasicElement>& newChildren,
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
                             QPtrList<BasicElement>& removedChildren,
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
                    if (!child->isInvisible()) {
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
                    if (!child->isInvisible()) {
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
void SequenceElement::removeChild(QPtrList<BasicElement>& removedChildren, int pos)
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
BasicElement* SequenceElement::getChild( FormulaCursor* cursor, Direction direction )
{
    if ( direction == beforeCursor ) {
        if ( cursor->getPos() > 0 ) {
            return children.at( cursor->getPos() - 1 );
        }
    }
    else {
        if ( cursor->getPos() < static_cast<int>( children.count() ) ) {
            return children.at( cursor->getPos() );
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

void SequenceElement::childWillVanish(FormulaCursor* cursor, BasicElement* child)
{
    int childPos = children.find(child);
    if (childPos > -1) {
        int pos = cursor->getPos();
        if (pos > childPos) {
            pos--;
        }
        int mark = cursor->getMark();
        if (mark > childPos) {
            mark--;
        }
        cursor->setTo(this, pos, mark);
    }
}


/**
 * Selects all children. The cursor is put behind, the mark before them.
 */
void SequenceElement::selectAllChildren(FormulaCursor* cursor)
{
    cursor->setTo(this, children.count(), 0);
}

bool SequenceElement::onlyTextSelected( FormulaCursor* cursor )
{
    if ( cursor->isSelection() ) {
        uint from = QMIN( cursor->getPos(), cursor->getMark() );
        uint to = QMAX( cursor->getPos(), cursor->getMark() );
        for ( uint i = from; i < to; i++ ) {
            BasicElement* element = getChild( i );
            if ( element->getCharacter() == QChar::null ) {
                return false;
            }
        }
    }
    return true;
}


void SequenceElement::input( Container* container, QChar ch )
{
    int latin1 = ch.latin1();
    switch (latin1) {
    case '(':
        container->document()->addDefaultBracket();
        break;
    case '[':
        container->addSquareBracket();
        break;
    case '{':
        container->addCurlyBracket();
        break;
    case '|':
        container->addLineBracket();
        break;
    case '^':
        container->addUpperRightIndex();
        break;
    case '_':
        container->addLowerRightIndex();
        break;
    case ' ':
        container->compactExpression();
        break;
    case '}':
    case ']':
    case ')':
        break;
    case '\\':
        container->addNameSequence();
        break;
    default:
        container->addText(ch);
    }
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
bool SequenceElement::buildChildrenFromDom(QPtrList<BasicElement>& list, QDomNode n)
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


BasicElement* SequenceElement::createElement( QString type )
{
    if      ( type == "TEXT" )         return new TextElement();
    else if ( type == "SPACE" )        return new SpaceElement();
    else if ( type == "ROOT" )         return new RootElement();
    else if ( type == "BRACKET" )      return new BracketElement();
    else if ( type == "MATRIX" )       return new MatrixElement();
    else if ( type == "INDEX" )        return new IndexElement();
    else if ( type == "FRACTION" )     return new FractionElement();
    else if ( type == "SYMBOL" )       return new SymbolElement();
    else if ( type == "NAMESEQUENCE" ) return new NameSequence();
    else if ( type == "SEQUENCE" ) {
        kdDebug( 40000 ) << "malformed data: sequence inside sequence." << endl;
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

    textSequence = true;
    for (BasicElement* element = children.first();
         element != 0;
         element = children.next()) {

        // Those types are gone. Make sure they won't
        // be used.
        element->setElementType(0);

        if (element->getCharacter() == QChar::null) {
            textSequence = false;
        }
    }

    const SymbolTable& symbols = formula()->getSymbolTable();
    SequenceParser parser(symbols);
    parseTree = parser.parse(children);

    // debug
    //parseTree->output();
}


bool SequenceElement::isFirstOfToken( BasicElement* child )
{
    return ( child->getElementType() != 0 ) && isChildNumber( child->getElementType()->start(), child );
}


QString SequenceElement::toLatex()
{
    QString content;
    content += "{";
    uint count = children.count();
    for ( uint i = 0; i < count; i++ ) {
        BasicElement* child = children.at( i );
        if ( isFirstOfToken( child ) ) {
            content += " ";
        }
        content += child->toLatex();
    }
    content += "}";
    return content;
}



NameSequence::NameSequence( BasicElement* parent )
    : SequenceElement( parent )
{
}

void NameSequence::calcCursorSize( const ContextStyle& context,
                                   FormulaCursor* cursor, bool smallCursor )
{
    inherited::calcCursorSize( context, cursor, smallCursor );
    LuPoint point = widgetPos();
    lu unit = context.ptToLayoutUnitPt( 1 );
    cursor->addCursorSize( LuRect( point.x()-unit, point.y()-unit,
                                   getWidth()+2*unit, getHeight()+2*unit ) );
}

void NameSequence::drawCursor( QPainter& painter, const ContextStyle& context,
                               FormulaCursor* cursor, bool smallCursor )
{
    LuPoint point = widgetPos();
    painter.setPen( QPen( context.getEmptyColor(),
                          context.layoutUnitToPixelX( context.getLineWidth()/2 ) ) );
    lu unit = context.ptToLayoutUnitPt( 1 );
    painter.drawRect( context.layoutUnitToPixelX( point.x()-unit ),
                      context.layoutUnitToPixelY( point.y()-unit ),
                      context.layoutUnitToPixelX( getWidth()+2*unit ),
                      context.layoutUnitToPixelY( getHeight()+2*unit ) );

    inherited::drawCursor( painter, context, cursor, smallCursor );
}

void NameSequence::moveWordLeft( FormulaCursor* cursor )
{
    uint pos = cursor->getPos();
    if ( pos > 0 ) {
        cursor->setTo( this, 0 );
    }
    else {
        moveLeft( cursor, this );
    }
}

void NameSequence::moveWordRight( FormulaCursor* cursor )
{
    int pos = cursor->getPos();
    if ( pos < countChildren() ) {
        cursor->setTo( this, countChildren() );
    }
    else {
        moveRight( cursor, this );
    }
}

void NameSequence::input( Container* container, QChar ch )
{
    int latin1 = ch.latin1();
    switch (latin1) {
    case '(':
    case '[':
    case '|':
    case '^':
    case '_':
    case '}':
    case ']':
    case ')':
    case '\\':
        break;
    case '{':
    case ' ':
        container->compactExpression();
        break;
    default:
        container->addText( ch );
    }
}

void NameSequence::setElementType( ElementType* t )
{
    inherited::setElementType( t );
    parse();
}

BasicElement* NameSequence::replaceElement( const SymbolTable& table )
{
    QString name = buildName();
    QChar ch = table.unicode( name );
    if ( !ch.isNull() ) return new TextElement( ch, true );

    if ( name == "," )    return new SpaceElement( THIN );
    if ( name == ">" )    return new SpaceElement( MEDIUM );
    if ( name == ";" )    return new SpaceElement( THICK );
    if ( name == "quad" ) return new SpaceElement( QUAD );

    if ( name == "frac" ) return new FractionElement();
    if ( name == "atop" ) {
        FractionElement* frac = new FractionElement();
        frac->showLine( false );
        return frac;
    }
    if ( name == "sqrt" ) return new RootElement();

    return 0;
}

BasicElement* NameSequence::createElement( QString type )
{
    if      ( type == "TEXT" )         return new TextElement();
    return 0;
}

void NameSequence::parse()
{
    // A name sequence is known as name and so are its children.
    // Caution: this is fake!
    for ( int i = 0; i < countChildren(); i++ ) {
        getChild( i )->setElementType( getElementType() );
    }
}

QString NameSequence::buildName()
{
    QString name;
    for ( int i = 0; i < countChildren(); i++ ) {
        name += getChild( i )->getCharacter();
    }
    return name;
}

bool NameSequence::isValidSelection( FormulaCursor* cursor )
{
    SequenceElement* sequence = cursor->normal();
    if ( sequence == 0 ) {
        return false;
    }
    return sequence->onlyTextSelected( cursor );
}

KFORMULA_NAMESPACE_END
