/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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

#include "RootElement.h"
#include "AttributeManager.h"
#include "FormulaCursor.h"
#include "RowElement.h"
#include <KoXmlReader.h>
#include <QPainter>
#include <QPen>
#include <kdebug.h>

RootElement::RootElement( BasicElement* parent ) : FixedElement( parent )
{
    m_radicand = new BasicElement( this );
    m_exponent = new BasicElement( this );
}

RootElement::~RootElement()
{
    delete m_radicand;
    delete m_exponent;
}

void RootElement::paint( QPainter& painter, AttributeManager* am )
{
    QPen pen;
    pen.setWidth( m_lineThickness );
    painter.setPen( pen );
    painter.drawPath( m_rootSymbol );
}

void RootElement::layout( const AttributeManager* am )
{
    // Calculate values to layout the root symbol
    double thinSpace = am->layoutSpacing( this );
    double symbolHeight  = m_radicand->baseLine();
    if( m_radicand->height() > symbolHeight*1.3 ) symbolHeight = m_radicand->height();
    symbolHeight += thinSpace;
    double tickWidth = symbolHeight / 3.0;  // The width of the root symbol's tick part

    m_lineThickness = am->lineThickness(this);

    // The root symbol an xOffset and yOffset due to the exponent.
    double xOffset = m_exponent->width() - tickWidth/2;
    xOffset = xOffset < 0 ? 0 : xOffset; // no negative offset for the root symbol
    double yOffset =  m_exponent->height() - 2.0*symbolHeight/5.0;
    yOffset = yOffset < 0 ? 0 : yOffset;

    // Set the roots dimensions
    setBaseLine( yOffset + thinSpace + m_radicand->baseLine() );
    setHeight( yOffset + thinSpace + m_radicand->height() );
    setWidth( xOffset + tickWidth + m_radicand->width() + thinSpace );

    // Place the children in the correct place
    m_radicand->setOrigin( QPointF( xOffset+tickWidth+thinSpace, yOffset+thinSpace ) );
    m_exponent->setOrigin( QPointF( 0.0, 0.0 ) );

    // Draw the actual root symbol to a path as buffer
    m_rootSymbol = QPainterPath();
    m_rootSymbol.moveTo( xOffset+m_lineThickness, yOffset +  2.0 * symbolHeight / 3.0 );
    m_rootSymbol.lineTo( m_rootSymbol.currentPosition().x()+tickWidth*0.5, yOffset + symbolHeight - m_lineThickness/2 );
    m_rootSymbol.lineTo( m_rootSymbol.currentPosition().x()+tickWidth*0.5, yOffset + m_lineThickness/2 );
    m_rootSymbol.lineTo( width()-m_lineThickness/2, yOffset + m_lineThickness/2);
}

const QList<BasicElement*> RootElement::childElements() const
{
    QList<BasicElement*> tmp;
    tmp << m_exponent << m_radicand;
    return tmp;
}


QList< BasicElement* > RootElement::elementsBetween(int pos1, int pos2) const
{ 
    QList<BasicElement*> tmp;
    if (pos1==0 && pos2 >0) { 
        tmp.append(m_exponent);
    }
    if (pos1<3 && pos2==3) {
        tmp.append(m_radicand);
    }
    return tmp;
}


QLineF RootElement::cursorLine(int position) const
{
    QPointF top=absoluteBoundingRect().topLeft();
    QPointF bottom;
    switch (position) {
	case 0:
	    top+=m_exponent->origin();
	    break;
	case 1:
	    top+=m_exponent->origin()+QPointF(m_exponent->width(),0.0);
	    break;
	case 2:
	    top+=m_radicand->origin();
	    break;
	case 3:
	    top+=m_radicand->origin()+QPointF(m_radicand->width(),0.0);
	    break;
    }
    if (position<=1) {
	bottom=top+QPointF(0.0,m_exponent->height());
    }
    else {
	bottom=top+QPointF(0.0,m_radicand->height());
    }
    return QLineF(top, bottom);
}

int RootElement::positionOfChild(BasicElement* child) const 
{
    if (child==m_exponent) {
        return 0;
    } else if (child==m_radicand) {
        return 2;
    }
    return -1;
}

bool RootElement::setCursorTo(FormulaCursor* cursor, QPointF point) 
{
    if (cursor->isSelecting()) {
        if (point.x()<(m_exponent->boundingRect().right()+m_radicand->boundingRect().left())/2.) {
            //the point is left of the radicand
            if (point.x()<m_exponent->boundingRect().left()) {
                //the point is left of the exponent
                cursor->moveTo(this, 0);
                return true;
            } else {
                //the point is on the exponent
                if (cursor->mark() == 0) {
                    cursor->moveTo(this, 1);
                    return true;
                } else {
                    cursor->moveTo(this, 0);
                    return true;
                }
            }
        } else {
            //the point is right of the exponent
            if (point.x()>m_radicand->boundingRect().right()) {
                //the point is right of the radicand
                cursor->moveTo(this, 3);
                return true;
            } else {
                //the point is on the radicand
                if (cursor->mark() == 3) {
                    cursor->moveTo(this, 2);
                    return true;
                } else {
                    cursor->moveTo(this, 3);
                    return true;
                }
            }
        }
        //clean up the selectionStart
        fixSelection(cursor);
    } else {
        if (m_exponent->boundingRect().contains(point)) {
            return m_exponent->setCursorTo(cursor, point-m_exponent->origin());
        } else {
            return m_radicand->setCursorTo(cursor, point-m_radicand->origin());
        }
    }
}

bool RootElement::moveCursor(FormulaCursor* newcursor, FormulaCursor* oldcursor) 
{
    switch (newcursor->direction()) {
    case MoveLeft:
        switch (newcursor->position()) {
        case 0:
            return false;
        case 1:
            if (newcursor->isSelecting()) {
                newcursor->moveTo(this,0);
            } else {
                newcursor->moveTo(m_exponent, m_exponent->length());
            }
            break;
        case 2:
            if (newcursor->isSelecting()) {
                newcursor->moveTo(this,0);
            } else {
                newcursor->moveTo(this,1);
            }
            break;
        case 3:
            if (newcursor->isSelecting()) {
                newcursor->moveTo(this,2);
            } else {
                newcursor->moveTo(m_radicand,m_radicand->length());
            }
            break;
        }
        break;
    case MoveRight:
        switch (newcursor->position()) {
        case 3:
            return false;
        case 2:
            if (newcursor->isSelecting()) {
                newcursor->moveTo(this,3);
            } else {
                newcursor->moveTo(m_radicand,0);
            }
            break;
        case 1:
            if (newcursor->isSelecting()) {
                newcursor->moveTo(this,3);
            } else {
                newcursor->moveTo(this,2);
            }
            break;
        case 0:
            if (newcursor->isSelecting()) {
                newcursor->moveTo(this,1);
            } else {
                newcursor->moveTo(m_exponent,0);
            }
            break;
        }
        break;
    case MoveUp:
        if (newcursor->isSelecting()) {
            return false;
        }
        if (newcursor->position()>=2) {
            newcursor->moveTo(this,1);
            break;
        } else {
            return false;
        }
        break;
    case MoveDown:
        if (newcursor->isSelecting()) {
            return false;
        }
        if (newcursor->position()<=1) {
            newcursor->moveTo(this,2);
            break;
        } else {
            return false;
        }
        break;
    }
    fixSelection(newcursor);
    return true;
}


void RootElement::fixSelection ( FormulaCursor* cursor )
{
    if (cursor->isSelecting()) {
        if (cursor->position()<2 && cursor->mark()==2) {
            cursor->setSelectionStart(1);
        } else if (cursor->position()==2 && cursor->mark()<2) {
            cursor->setPosition(1);
        } else if (cursor->position()==1 && cursor->mark()>1) {
            cursor->setPosition(2);
        } else if (cursor->position()>1 && cursor->mark()==1) {
            cursor->setSelectionStart(2);
        }
    }
}


int RootElement::length() const
{
    return 3;
}


bool RootElement::replaceChild ( BasicElement* oldelement, BasicElement* newelement )
{
    if (oldelement==m_exponent) {
        m_exponent=newelement;
        return true;
    } else if (oldelement==m_radicand) {
        m_radicand=newelement;
        return true;
    }
    return false;
}

bool RootElement::acceptCursor( const FormulaCursor* cursor )
{
    return true;
}

ElementType RootElement::elementType() const
{
    return Root;
}

bool RootElement::readMathMLContent( const KoXmlElement& element )
{
    KoXmlElement tmp;
    forEachElement( tmp, element ) {
        if( m_radicand->elementType() == Basic ) {
            delete m_radicand;
            m_radicand = ElementFactory::createElement( tmp.tagName(), this );
            if( !m_radicand->readMathML( tmp ) )
                return false;
	}
        else if( m_exponent->elementType() == Basic ) {
            delete m_exponent;
            m_exponent = ElementFactory::createElement( tmp.tagName(), this );
            if( !m_exponent->readMathML( tmp ) )
                return false;
        }
        else {
            kDebug(39001) << "Too many arguments to mroot";
            return false;
        }
    }
    Q_ASSERT( m_radicand );
    Q_ASSERT( m_exponent );
    return true;
}

void RootElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    Q_ASSERT( m_radicand );
    Q_ASSERT( m_exponent );
    m_radicand->writeMathML( writer );
    m_exponent->writeMathML( writer );
}

