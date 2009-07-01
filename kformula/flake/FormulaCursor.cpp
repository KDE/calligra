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
    m_direction = NoDirection;
    m_ascending = false;
}

void FormulaCursor::paint( QPainter& painter ) const
{
    if( !m_currentElement )
        return;
 
    // setup a 1px pen and draw the cursor line with it 
    QPen pen;
    pen.setWidthF( 0.5 );
    painter.setPen( pen );
    painter.drawLine(m_currentElement->cursorLine( this ));
    pen.setWidth( 0);
    switch(m_currentElement->elementType()) {
	case Number:
	   painter.setPen(Qt::red);
	   break;
	case Identifier:
	   painter.setPen(Qt::darkMagenta);
	   break;
	case Row:
	   painter.setPen(Qt::green);
	   break;
	case Fraction:
	   painter.setPen(Qt::blue);
	   break;
	default:
	   painter.setPen(Qt::magenta);
	   break;
    }
    QPointF origin=m_currentElement->absoluteBoundingRect().topLeft();
    double baseline=m_currentElement->baseLine();
    painter.drawRect( m_currentElement->absoluteBoundingRect() );
    painter.setPen( QPen( Qt::blue, 0, Qt::DashLine ) );
    painter.drawLine( origin+QPointF(0.0,baseline),
		      origin+QPointF(m_currentElement->width(), baseline) );
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
    m_direction = direction;
    BasicElement* oldCurrentElement= m_currentElement;
    int oldPosition=m_positionInElement;
    
    if ( m_direction==MoveLeft || m_direction==MoveRight ) {
	//the cursor is moved horizontally
	while (m_currentElement->moveCursor( this )) {
	    if ( m_currentElement->acceptCursor( this )==m_currentElement ) {
		return;
	    }
	}
    } else {
	//the cursor is moved vertically
	QPointF point=(m_currentElement->cursorLine(this).p1()+m_currentElement->cursorLine(this).p2())/2.;
	while ( m_currentElement ) {
	    if ( m_currentElement->moveCursor( this ) ) {
		//setCursorTo wants the point in coordinates relative to the element, so we transform 
 		point-=m_currentElement->absoluteBoundingRect().topLeft();
		m_currentElement->setCursorTo( this, point );
		return;
		//TODO: Find out what to do, if the current position is not accepted
	    }
	    if ( m_currentElement->parentElement() ) {
		//TODO: check if this is really needed, it is just there to keep the cursor in a valid state
		m_positionInElement=m_currentElement->parentElement()->positionOfChild(m_currentElement);
	    }
	    m_currentElement=m_currentElement->parentElement();
	}
    }
    m_currentElement=oldCurrentElement;
    m_positionInElement=oldPosition;
    m_direction = NoDirection;
}

void FormulaCursor::moveTo( BasicElement* element, int position )
{
    if( m_direction == NoDirection ) {
        m_currentElement = element;
        m_positionInElement = position;
    }
    m_direction = NoDirection;
}

void FormulaCursor::setCursorTo( const QPointF& point )
{
    BasicElement* formulaElement = m_currentElement;
    while( formulaElement->parentElement() != 0 )
	formulaElement = formulaElement->parentElement();
    formulaElement->setCursorTo(this,point);
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

bool FormulaCursor::ascending() const
{
    return m_ascending;
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
    m_selecting = selecting;
}

