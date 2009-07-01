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

#include "FormulaCursor.h"
#include "BasicElement.h"
#include "NumberElement.h"
#include "ElementFactory.h"
#include "OperatorElement.h"
#include "IdentifierElement.h"
#include "FractionElement.h"
#include <QPainter>
#include <QPen>

#include <kdebug.h>

FormulaCursor::FormulaCursor( BasicElement* element )
              : m_currentElement( element )
{
    m_positionInElement = 0;
    m_selectionStartPosition = 0;
    m_selecting = false;
    m_direction = NoDirection;
}

void FormulaCursor::paint( QPainter& painter ) const
{
    kDebug() << "Drawing cursor with selecting: "<< hasSelection() << " from "
	<< selectionStartPosition()<<" to " << position();
    if( !m_currentElement )
        return;
    
    QPointF origin=m_currentElement->absoluteBoundingRect().topLeft();
    double baseline=m_currentElement->baseLine();
    QPen pen;
    pen.setWidthF( 0.5 );
    painter.setPen( pen );
    painter.drawLine(m_currentElement->cursorLine( m_positionInElement ));
    pen.setWidth( 0);
    pen.setColor(Qt::blue);
    pen.setStyle(Qt::DashLine);
    painter.setPen( pen );
    painter.drawLine( origin+QPointF(0.0,baseline),
		      origin+QPointF(m_currentElement->width(), baseline) );
    pen.setStyle(Qt::DotLine);
    switch(m_currentElement->elementType()) {
	case Number:
	   pen.setColor(Qt::red);
	   break;
	case Identifier:
	   pen.setColor(Qt::darkMagenta);
	   break;
	case Row:
	   pen.setColor(Qt::green);
	   break;
	case Fraction:
	   pen.setColor(Qt::blue);
	   break;
	default:
	   pen.setColor(Qt::magenta);
	   break;
    }
    painter.setPen(pen);
    painter.drawRect( m_currentElement->absoluteBoundingRect() );
    //draw the selection rectangle
    if ( m_selecting ) {
	kDebug() << "Selection is " << selectionRect();
	QBrush brush;
	QColor color(Qt::blue);
 	color.setAlpha(128);
	brush.setColor(color);
	brush.setStyle(Qt::SolidPattern);
	painter.setBrush(brush);
	painter.setPen(Qt::NoPen);
	int p1=position()<selectionStartPosition()? position() : selectionStartPosition();
	int p2=position()<selectionStartPosition()? selectionStartPosition() : position() ;
	painter.drawPath(m_currentElement->selectionRegion(p1,p2));
// 	painter.drawRect(selectionRect());
    }
}

void FormulaCursor::insertText( const QString& text )
{
//    if ( text == "\\" && m_currentElement != TokenElement )
//    TODO   -> go to latex processing

//    if( text.size() != 1 ) // check for single char input after key press
        // TODO check for text excapting element due to paste of text

    // Filter for things that can be typed in with the keyboard
    // - operators like: / * + - | ^ % ( )
    // - other text is put into text accepting elements
    // - lim, max min
    m_inputBuffer = text;
    QChar tmpChar( text[ 0 ] );
    QString tagname; 
    ElementType tokentype;
    if (tmpChar.isNumber()) {
	tagname="mn";
	tokentype=Number;
    }
    else if ( tmpChar == '*' || tmpChar == '+' || tmpChar == '-' ||
              tmpChar == '=' || tmpChar == '>' || tmpChar == '<' ||
              tmpChar == '!' || tmpChar == '.' || tmpChar == '/' ||
              tmpChar == '?' || tmpChar == ',' || tmpChar == ':' ) {
	tagname="mo";
	tokentype=Operator;
    }
    else if (tmpChar.isLetter()) {
	tagname="mi";
	tokentype=Identifier;
    }
    if (m_currentElement->elementType() != tokentype) {
	// TODO: frac, ... will need special treatment too
	if (insideToken()) {
	    m_positionInElement=m_currentElement->parentElement()->positionOfChild(m_currentElement)+1;
	    m_currentElement=m_currentElement->parentElement();
	}
	BasicElement* tmpToken = ElementFactory::createElement(tagname,m_currentElement);
	m_currentElement->insertChild( this, tmpToken );
	m_currentElement = tmpToken;
	m_positionInElement = 0; //the cursor is at the beginning of our new token
    }
    m_currentElement->insertChild( this, 0 );
    m_positionInElement+=text.length(); //potentially more than one character is added
}

void FormulaCursor::insertData( const QString& data )
{
    BasicElement* elementToInsert = 0;

    // MathML data to load
    if( data.startsWith( '<' ) ) {
        // TODO
    }
    // special behaviour for table columns
    else if( data == "mtd" ) {
        // TODO
    }
    else
        elementToInsert = ElementFactory::createElement( data, m_currentElement );

    Q_ASSERT( elementToInsert );
    
    insertElement( elementToInsert );
}

void FormulaCursor::insertElement( BasicElement* element )
{
    if( m_currentElement->elementType() == Basic ) {
        m_currentElement->parentElement()->insertChild( this, element );
        delete m_currentElement;
        m_currentElement = element;
        m_positionInElement = 1;
    }
    else if( insideInferredRow() ) {
        m_currentElement->insertChild( this, element );
        m_positionInElement++;
    }
    else if( insideToken() ) {
        // TODO implement token splitting
    }
    else {
        BasicElement* r = ElementFactory::createElement( "mrow",
                                                    m_currentElement->parentElement() );
        m_currentElement->parentElement()->insertChild( this, r );

        r->insertChild( this, ( m_positionInElement==0 ) ? element : m_currentElement );
        r->insertChild( this, ( m_positionInElement==0 ) ? m_currentElement : element );
        m_currentElement = r;
    }
}

void FormulaCursor::remove( bool elementBeforePosition )
{
     
}

void FormulaCursor::move( CursorDirection direction )
{
    FormulaCursor oldcursor(*this);
    m_direction = direction;
    //the cursor is moved vertically
    while ( m_currentElement ) {
	if ( m_currentElement->moveCursor( this, &oldcursor ) ) {
	    if (m_currentElement->acceptCursor(this)) {
		m_direction = NoDirection;
		return;
	    }
	} else {
	    if ( m_currentElement->parentElement() ) {
		if (hasSelection()) {
		    m_selectionStartPosition=m_currentElement->parentElement()->positionOfChild(m_currentElement);
		}
		m_positionInElement=m_currentElement->parentElement()->positionOfChild(m_currentElement);
		m_currentElement=m_currentElement->parentElement();
		if (m_direction==MoveRight) {
		    m_positionInElement++;
		    if (m_currentElement->acceptCursor(this)) {
			m_direction = NoDirection;
			return;
		    }
		} else if (m_direction==MoveLeft) {
		    if (hasSelection()) {
			m_selectionStartPosition++;
		    }
		    if (m_currentElement->acceptCursor(this)) {
			m_direction = NoDirection;
			return;
		    }
		}
	    } else {
		m_currentElement=m_currentElement->parentElement();
	    }
	}
    }
    (*this)=oldcursor;
}

bool FormulaCursor::moveCloseTo(BasicElement* element, FormulaCursor* cursor) 
{    
    return element->setCursorTo(this,cursor->getCursorPosition()-element->absoluteBoundingRect().topLeft());
}


QPointF FormulaCursor::getCursorPosition() 
{
    return ( m_currentElement->cursorLine(m_positionInElement).p1()
           + m_currentElement->cursorLine(m_positionInElement).p2())/2.;
}


void FormulaCursor::moveTo( BasicElement* element, int position )
{
        m_currentElement = element;
        m_positionInElement = position;
}

void FormulaCursor::setCursorTo( const QPointF& point )
{
    if (m_selecting) {
	while (!m_currentElement->absoluteBoundingRect().contains(point)) {
	    if ( m_currentElement->parentElement() ) {
		
		m_positionInElement=0;
		if (point.x()<m_currentElement->cursorLine(m_selectionStartPosition).p1().x()) {
		    //the point is left of the old selection start, so we move the selection 
		    //start after the old current element
		    m_selectionStartPosition=m_currentElement->parentElement()->positionOfChild(m_currentElement)+1;
		} else {
		    m_selectionStartPosition=m_currentElement->parentElement()->positionOfChild(m_currentElement);
		}
		m_currentElement=m_currentElement->parentElement();
	    }
	    else {
// 		setSelecting(false);
		return;
	    }
	}
	while (!m_currentElement->setCursorTo(this,point-m_currentElement->absoluteBoundingRect().topLeft())) {
	    if ( m_currentElement->parentElement() ) {
		m_selectionStartPosition=m_currentElement->parentElement()->positionOfChild(m_currentElement);
		m_positionInElement=0;
		if (point.x()<m_currentElement->cursorLine(m_selectionStartPosition).p1().x()) {
		    //the point is left of the old selection start, so we move the selection 
		    //start after the old current element
		    m_selectionStartPosition++;
		}
		m_currentElement=m_currentElement->parentElement();
	    }
	    else {
		return;
	    }
	}
	
    } else {
	BasicElement* formulaElement = m_currentElement;
	while( formulaElement->parentElement() != 0 )
	    formulaElement = formulaElement->parentElement();
	formulaElement->setCursorTo(this,point);
    }
}

int FormulaCursor::selectionStartPosition() const 
{
    return m_selectionStartPosition;
}

QRectF FormulaCursor::selectionRect() const 
{
	QLineF l1=m_currentElement->cursorLine(m_positionInElement);
	QLineF l2=m_currentElement->cursorLine(m_selectionStartPosition);
	//TODO: find out why doesn't work
	//QRectF r1(l1.p1(),l1.p2());
	//QRectF r2(l2.p1(),l2.p2());
	
	QRectF r1(l1.p1(),l2.p2());
	QRectF r2(l2.p1(),l1.p2());
//	kDebug()<<r1<<r2;
// 	kDebug()<<r1.united(r2).topLeft()<<" --- "<<r1.united(r2).bottomRight();
	return r1.unite(r2);
}


void FormulaCursor::moveHome()
{
    m_positionInElement = 0;
}

void FormulaCursor::moveEnd()
{
    m_positionInElement=m_currentElement->length();
}

bool FormulaCursor::isHome() const
{
    return m_positionInElement == 0;
}

bool FormulaCursor::isEnd() const
{
    return m_positionInElement == m_currentElement->length();
}

bool FormulaCursor::insideToken() const
{
    if( m_currentElement->elementType() == Number ||
        m_currentElement->elementType() == Operator ||
        m_currentElement->elementType() == Identifier )
        return true;

    return false;
}

bool FormulaCursor::insideInferredRow() const
{
    if( m_currentElement->elementType() == Row ||
        m_currentElement->elementType() == SquareRoot ||
        m_currentElement->elementType() == Formula ||
        m_currentElement->elementType() == Fenced )
        return true;

    return false;
}

BasicElement* FormulaCursor::currentElement() const
{
    return m_currentElement;
}

int FormulaCursor::position() const
{
    return m_positionInElement;
}

void FormulaCursor::setCurrentElement(BasicElement* element) {
    m_currentElement=element;
}

void FormulaCursor::setPosition(int position) {
    m_positionInElement=position;
}

CursorDirection FormulaCursor::direction() const
{
    return m_direction;
}

QString FormulaCursor::inputBuffer() const
{
    return m_inputBuffer;
}

bool FormulaCursor::hasSelection() const
{
    return m_selecting;
}

void FormulaCursor::setSelecting( bool selecting )
{
    if (selecting) {
	if (!m_selecting) {
	    //we start a new selection
	    m_selecting = selecting;
	    m_selectionStartPosition=m_positionInElement;
	}
    } else {
	m_selecting = selecting;
	m_selectionStartPosition=0;
    }
}

void FormulaCursor::setSelectionStart(int position) {
    m_selectionStartPosition=position;
}

