/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "RowElement.h"
#include "FormulaCursor.h"
#include <KoXmlWriter.h>
#include <KoXmlReader.h>
#include <QPainter>

#include <kdebug.h>

RowElement::RowElement( BasicElement* parent ) : BasicElement( parent )
{}

RowElement::~RowElement()
{
    qDeleteAll( m_childElements );
}

void RowElement::paint( QPainter& painter, AttributeManager* )
{ /* RowElement has no visual representance so paint nothing */ }

void RowElement::layout( const AttributeManager* am )
{
    Q_UNUSED( am )          // there are no attributes that can be processed here

    if( m_childElements.isEmpty() )  // do not do anything if there are no children
        return;

    QPointF origin;
    double width = 0.0;
    double topToBaseline = 0.0;
    double baselineToBottom = 0.0;
    foreach( BasicElement* child, m_childElements ) // iterate through the children and
        topToBaseline = qMax( topToBaseline, child->baseLine() );  // find max baseline

    foreach( BasicElement* child, m_childElements )  // iterate through the children
    {
        child->setOrigin( QPointF( width, topToBaseline - child->baseLine() ) );
        baselineToBottom = qMax( baselineToBottom, child->height()-child->baseLine() );
        width += child->width();       // add their width
    }

    setWidth( width );
    setHeight( topToBaseline + baselineToBottom );
    setBaseLine( topToBaseline );
    setChildrenBoundingRect(QRectF(0,0, width, height()));
}

void RowElement::stretch()
{
    //The elements can grow vertically, so make sure we reposition their vertical 
    //origin appropriately
    foreach( BasicElement* tmpElement, childElements() ) {
        tmpElement->stretch();
        //Set the origin.  Note that we ignore the baseline and center the object
        //vertically
        //I think we need to FIXME for symmetric situations or something?
        tmpElement->setOrigin( QPointF(tmpElement->origin().x(), childrenBoundingRect().y() + (childrenBoundingRect().height() - tmpElement->height())/2 ));
    }
}

int RowElement::length() const
{
    return m_childElements.count();
}

const QList<BasicElement*> RowElement::childElements() const
{
    return m_childElements;
}

bool RowElement::insertChild( int position, BasicElement* child )
{
    if (0<=position && position<=length()) {
        m_childElements.insert( position, child );
        return true;
    } else {
        return false;
    }
}

bool RowElement::removeChild( BasicElement* child )
{
    return m_childElements.removeOne(child);
}

bool RowElement::acceptCursor( const FormulaCursor* cursor )
{
        return true;
}
bool RowElement::moveCursor(FormulaCursor* newcursor, FormulaCursor* oldcursor) 
{
    if ( (newcursor->direction()==MoveUp) ||
        (newcursor->direction()==MoveDown) ||
        (newcursor->isHome() && newcursor->direction()==MoveLeft) ||
        (newcursor->isEnd() && newcursor->direction()==MoveRight) ) {
        //the newcursor can't be moved vertically
        //TODO: check what happens with linebreaks in <mspace> elements
        return false;
    }
    if (newcursor->isSelecting()) {
        switch(newcursor->direction()) {
        case MoveLeft:
            newcursor->setPosition(newcursor->position()-1);
            break;
        case MoveRight:
            newcursor->setPosition(newcursor->position()+1);
            break;
        }
    } else {
        switch(newcursor->direction()) {
        case MoveLeft:
            newcursor->setCurrentElement(m_childElements[newcursor->position()-1]);
            newcursor->moveEnd();
            break;
        case MoveRight:
            newcursor->setCurrentElement(m_childElements[newcursor->position()]);
            newcursor->moveHome();
            break;
        }
    }
    return true;
}

QLineF RowElement::cursorLine(int position) const {
    QPointF top=absoluteBoundingRect().topLeft();
    if( childElements().isEmpty() ) {
        // center cursor in elements that have no children
        top += QPointF( width()/2, 0 );
    } else { 
        if ( position==length()) {
            top += QPointF(width(),0.0);
        } else {
            top += QPointF( childElements()[ position ]->boundingRect().left(), 0.0 );
        }
    }
    QPointF bottom = top + QPointF( 0.0, height() );
    return QLineF(top, bottom);
}

bool RowElement::setCursorTo(FormulaCursor* cursor, QPointF point)
{
    if (m_childElements.isEmpty() || point.x()<m_childElements[0]->origin().x()) {
        cursor->setCurrentElement(this);
        cursor->setPosition(0);
        return true;
    }
    int i;
    for (i=0; i<m_childElements.count(); i++) {
        //Find the child element the point is in
        if (m_childElements[i]->boundingRect().right()>=point.x()) {
            break;
        }
    }
    //check if the point is behind all child elements
    if (i==m_childElements.count()) {
        cursor->setCurrentElement(this);
        cursor->setPosition(length());
        return true;
    } else {
        if (cursor->isSelecting()) {
            //we don't need to change current element because we are already in this element
            if (cursor->mark()<=i) {
                cursor->setPosition(i+1);
            } else {
                cursor->setPosition(i);
            }
            return true;
        } else {
            point-=m_childElements[i]->origin();
            return m_childElements[i]->setCursorTo(cursor,point);
        }
    }
}


ElementType RowElement::elementType() const
{
    return Row;
}

bool RowElement::readMathMLContent( const KoXmlElement& parent )
{
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;
    forEachElement( tmp, parent )
    {
        tmpElement = ElementFactory::createElement( tmp.tagName(), this );
        Q_ASSERT( tmpElement );
        m_childElements << tmpElement;
        if( !tmpElement->readMathML( tmp ) )
            return false;
    }
    return true;
}

int RowElement::positionOfChild(BasicElement* child) const {
    return m_childElements.indexOf(child);
}

void RowElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    foreach( BasicElement* tmp, m_childElements )
        tmp->writeMathML( writer );
}


BasicElement* RowElement::elementAfter ( int position )
{
    if (position<length()) {
        return m_childElements[position];
    } else {
        return 0;
    }
}

BasicElement* RowElement::elementBefore ( int position )
{
    if (position>1) {
        return m_childElements[position-1];
    } else {
        return 0;
    }
}

QList< BasicElement* > RowElement::elementsBetween ( int pos1, int pos2 ) const
{
    return m_childElements.mid(pos1,pos2-pos1);
}


bool RowElement::replaceChild ( BasicElement* oldelement, BasicElement* newelement )
{
        m_childElements.replace(m_childElements.indexOf(oldelement),newelement);
}








