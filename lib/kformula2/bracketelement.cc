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

#include <qlist.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpointarray.h>

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
    
    left->calcSizes(style, mySize, contentHeight, false);
    right->calcSizes(style, mySize, contentHeight, true);

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

    left->draw(painter, style, mySize, myPos, false);
    content->draw(painter, style, mySize, myPos);
    right->draw(painter, style, mySize, myPos, true);
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
BracketElement::Bracket* BracketElement::createBracket(char bracket)
{
    switch (bracket) {
    case '[':
    case ']':
        return new SquareBracket;
    case '{':
    case '}':
        return new CurlyBracket;
    case '|':
        return new LineBracket;
    case '<':
    case '>':
        return new CornerBracket;
    case '(':
    case ')':
        return new RoundBracket;
    default:
        return new EmptyBracket;
    }
}


// '(' and ')'
void BracketElement::RoundBracket::calcSizes(ContextStyle& style, int parentSize,
                                             int contentHeight, bool right)
{
    int height = contentHeight > 21 ? contentHeight+2 : 24;
    size.setWidth(8);
    size.setHeight(height);
}

void BracketElement::RoundBracket::draw(QPainter& painter, ContextStyle& style,
                                        int parentSize, const QPoint& origin,
                                        bool right)
{
    int x = origin.x() + point.x();
    int y = origin.y() + point.y();
    int thickness = size.width()/4+1;
    
    //painter.setBrush(style.getDefaultColor());
    painter.setPen(QPen(style.getDefaultColor(), 2));
    
    if (right) {
        painter.drawArc(x-thickness, y, size.width(), size.height(), 270*16, 180*16);
    }
    else {
        painter.drawArc(x+thickness, y, size.width(), size.height(), 90*16, 180*16);
    }
}


// '[' and ']'
void BracketElement::SquareBracket::calcSizes(ContextStyle& style, int parentSize,
                                              int contentHeight, bool right)
{
    int height = contentHeight > 21 ? contentHeight+2 : 24;
    size.setWidth(8);
    size.setHeight(height);
}

void BracketElement::SquareBracket::draw(QPainter& painter, ContextStyle& style,
                                         int parentSize, const QPoint& origin,
                                         bool right)
{
    int x = origin.x() + point.x() + 1;
    int y = origin.y() + point.y();
    int width = getWidth()-2;
    int thickness = width/4+1;
    int unit = width - thickness;
        
    painter.setBrush(style.getDefaultColor());
    painter.setPen(Qt::NoPen);
    QPointArray points(10);
    
    if (right) {
        points.setPoint(1, x, y);
        points.setPoint(2, x + width, y);
        points.setPoint(3, x + width, y + getHeight());
        points.setPoint(4, x, y + getHeight());
        points.setPoint(5, x, y + getHeight() - thickness);
        points.setPoint(6, x + unit, y + getHeight() - thickness);
        points.setPoint(7, x + unit, y + thickness);
        points.setPoint(8, x, y + thickness);
    }
    else {
        points.setPoint(1, x, y);
        points.setPoint(2, x + width, y);
        points.setPoint(3, x + width, y + thickness);
        points.setPoint(4, x + thickness, y + thickness);
        points.setPoint(5, x + thickness, y + getHeight() - thickness);
        points.setPoint(6, x + width, y + getHeight() - thickness);
        points.setPoint(7, x + width, y+getHeight());
        points.setPoint(8, x, y+getHeight());
    }
    painter.drawPolygon(points, false, 1, 8);
}


// '{' and '}'
void BracketElement::CurlyBracket::calcSizes(ContextStyle& style, int parentSize,
                                             int contentHeight, bool right)
{
    size.setWidth(contentHeight/4);
    size.setHeight(contentHeight);
}

void BracketElement::CurlyBracket::draw(QPainter& painter, ContextStyle& style,
                                        int parentSize, const QPoint& origin,
                                        bool right)
{
    // todo
}


// '|'
void BracketElement::LineBracket::calcSizes(ContextStyle& style, int parentSize,
                                            int contentHeight, bool right)
{
    int height = contentHeight > 21 ? contentHeight+2 : 24;
    size.setWidth(8);
    size.setHeight(height);
}

void BracketElement::LineBracket::draw(QPainter& painter, ContextStyle& style,
                                       int parentSize, const QPoint& origin,
                                       bool right)
{
    int x = origin.x() + point.x() + 1;
    int y = origin.y() + point.y();
    int width = getWidth()-2;
    int thickness = width/4+1;
    int unit = width - thickness;
        
    painter.setBrush(style.getDefaultColor());
    painter.setPen(Qt::NoPen);
    QPointArray points(5);
    
    points.setPoint(1, x+unit/2, y);
    points.setPoint(2, x+unit/2+thickness, y);
    points.setPoint(3, x+unit/2+thickness, y+getHeight());
    points.setPoint(4, x+unit/2, y+getHeight());
    
    painter.drawPolygon(points, false, 1, 4);
}


// '<' and '>'
void BracketElement::CornerBracket::calcSizes(ContextStyle& style, int parentSize,
                                              int contentHeight, bool right)
{
    int height = contentHeight > 21 ? contentHeight+2 : 24;
    size.setWidth(8);
    size.setHeight(height);
}

void BracketElement::CornerBracket::draw(QPainter& painter, ContextStyle& style,
                                         int parentSize, const QPoint& origin, bool right)
{
    int x = origin.x() + point.x() + 1;
    int y = origin.y() + point.y();
    int width = getWidth()-2;
    int thickness = width/4+1;
    int unit = width - thickness;
        
    painter.setBrush(style.getDefaultColor());
    painter.setPen(Qt::NoPen);
    QPointArray points(5);
    
    if (right) {
        points.setPoint(1, x+unit, y);
        points.setPoint(2, x, y + getHeight()/2);
        points.setPoint(3, x+unit, y+getHeight());
        //points.setPoint(4, x, y+getHeight());
    }
    else {
        points.setPoint(1, x, y);
        points.setPoint(2, x+unit, y + getHeight()/2);
        points.setPoint(3, x, y+getHeight());
        //points.setPoint(4, x, y+getHeight());
    }
    painter.drawPolyline(points, 1, /*4*/3);
}

QDomElement BracketElement::getElementDom(QDomDocument *doc)
{
    QDomElement de=doc->createElement("BRACKET");
    int sz=getRelativeSize();
    if(sz!=0) {
        de.setAttribute("SIZE",sz);
    }
    QDomElement con=doc->createElement("CONTENT");
    con.appendChild(content->getElementDom(doc));
    de.appendChild(con);

#warning TODO bracket type attribute

    de.setAttribute("LEFT","(");
    de.setAttribute("RIGHT",")");
    return de;
}


void BracketElement::buildFromDom(QDomElement *elem)
{
    if(!elem) return;

//Only set size.
    BasicElement::buildFromDom(elem);

#warning TODO bracket type attribute

    QDomNode n = elem->firstChild();
    while ( !n.isNull() ) {
        if ( n.isElement() ) {
             QDomElement e = n.toElement();
	    // BasicElement *child=0;
	     QString tag=e.tagName();
	     tag=tag.upper();
	
	     QDomElement e1;
	     QDomNode n1 = e.firstChild();
	     while ( !n1.isNull() ) {
    	 	 if ( n1.isElement() ) {
                     e1 = n1.toElement();
		     if(e1.tagName()=="SEQUENCE") //First sequence            
		         break;
		 }
		 
	         n1 = n1.nextSibling();
	     }
	     
	     
	     if(tag=="CONTENT") {
		 content=new SequenceElement(this);
		 content->buildFromDom(&e1);
	     }
	 }
        
        n = n.nextSibling();
    }    
    

}
