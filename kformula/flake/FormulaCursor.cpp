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

    QPointF top = m_currentElement->boundingRect().topLeft();
     
    if( m_currentElement->childElements().isEmpty() )  // set the cursor in the middle
        top += QPointF( m_currentElement->width()/2, 0 );
    else
    { 
        // determine the x coordinate by summing up the elements' width before the cursor
        for( int i = 0; i < m_positionInElement; i++ )
            top += QPointF( m_currentElement->childElements()[ i ]->width(), 0.0 );
    }
   
    // setup a 1px pen and draw the cursor line with it 
    QPointF bottom = top + QPointF( 0.0, m_currentElement->height() );
    QPen pen;
    pen.setWidth( 0 );
    painter.setPen( pen );
    painter.drawLine( top, bottom );
}

void FormulaCursor::insertText( const QString& text )
{
/*    if ( text == "\\" && m_currentElement != TokenElement )
       -> go to latex processing

    if( text.size() != 1 ) // check for single char input after key press
        // TODO check for text excapting element due to paste of text


    QChar tmpChar( text );
    if( tmpChar.isNumber() ) {
        if( m_currentElement->elementType() == Number )
            // TODO insert stuff somehow

    }
    else if( tmpChar.isSpace() )
        // what to do ???
    else if( tmpChar.isLetter() )
 Filters for things that can be typed in with the keyboard
- most important: numbers
- second operators like: / *-+ |^%( )
- text input is only allowed inside of text accepting elements
*/
}

void FormulaCursor::insert( BasicElement* element )
{
}

void FormulaCursor::remove( bool elementBeforePosition )
{
}

void FormulaCursor::move( CursorDirection direction )
{
    m_direction = direction;
    BasicElement* tmp = m_currentElement;

    // loop through the element tree and try to find an element that accepts the cursor
    while( tmp ) {
        if( tmp->acceptCursor( this ) == tmp )  // an element accepts the cursor
            break;
        else if( tmp->acceptCursor( this ) == tmp->parentElement() )
            m_ascending = true;
        else
            m_ascending = false;
     
        tmp = tmp->acceptCursor( this );        
    }

    if( !tmp )    // no element accepted or error so quit
        return;
    else if( tmp == m_currentElement ) {       // alter the position inside the element
        if( ( isHome() && m_direction == MoveLeft ) ||
            ( isEnd() && m_direction == MoveRight ) ||
            m_direction == MoveUp || m_direction == MoveDown )
            return;
    }
    else {
        m_currentElement = tmp;           // asign the new element to the cursor
        if( m_direction == MoveRight )    // and set position according to movement
            moveEnd();
        else
            moveHome();
    }
}

void FormulaCursor::moveHome()
{
    m_positionInElement = 0;
}

void FormulaCursor::moveEnd()
{
    if( m_currentElement->elementType() == Row )
	m_positionInElement = m_currentElement->childElements().count();
    else
        m_positionInElement = 1;
}

bool FormulaCursor::isHome() const
{
    return m_positionInElement == 0;
}

bool FormulaCursor::isEnd() const
{
    if( currentElement()->elementType() == Row )
        return ( m_positionInElement == m_currentElement->childElements().count() );
    else
	return ( m_positionInElement == 1 );
}

BasicElement* FormulaCursor::currentElement() const
{
    return m_currentElement;
}

int FormulaCursor::position() const
{
    return m_positionInElement;
}

CursorDirection FormulaCursor::direction() const
{
    return m_direction;
}

bool FormulaCursor::ascending() const
{
    return m_ascending;
}

bool FormulaCursor::hasSelection() const
{
    return m_selecting;
}

void FormulaCursor::setSelecting( bool selecting )
{
    m_selecting = selecting;
}

