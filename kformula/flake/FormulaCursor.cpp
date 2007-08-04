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
}

void FormulaCursor::paint( QPainter& painter ) const
{
    QPointF top = m_currentElement->boundingRect().topLeft();
     
    if( m_currentElement->childElements().isEmpty() )  // set the cursor in the middle
        top += QPointF( m_currentElement->width()/2, 0 );
    else
    { 
        // determine the x coordinate by summing up the elements' width before the cursor
        for( int i = 0; i < m_positionInElement; i++ )
            top += QPointF( m_currentElement->childElements().value(i)->width(), 0 );
    }
    
    QPointF bottom = top + QPointF( 0, m_currentElement->height() );
    QPen pen;
    pen.setWidth( 1 );
    painter.setPen( pen );
    painter.drawLine( top, bottom );
}

void FormulaCursor::insertText( const QString& text )
{
}

void FormulaCursor::insert( BasicElement* element )
{
}

void FormulaCursor::remove( bool elementBeforePosition )
{
}

void FormulaCursor::moveLeft()
{
    BasicElement* tmp;
    if( isHome() )
    {
        tmp = m_currentElement->parentElement();
        while( tmp != tmp->acceptCursor( LeftToParent ) && tmp->elementType() != Formula )
            tmp = tmp->acceptCursor( LeftToParent );
    }
    else
    {
        tmp = m_currentElement;
        while( tmp != m_currentElement->acceptCursor( LeftToChild ) )
            tmp = tmp->acceptCursor( LeftToChild );
    }

    if( !isHome() && tmp == m_currentElement )
        m_positionInElement--;
    else
        m_currentElement = tmp;
}

void FormulaCursor::moveRight()
{
    BasicElement* tmp;
    if( isEnd() )
    {
        tmp = m_currentElement->parentElement();
        while( tmp != tmp->acceptCursor( RightToParent ) &&
               tmp->elementType() != Formula )
            tmp = tmp->acceptCursor( RightToParent );
    }
    else
    {
        tmp = m_currentElement;
        while( tmp != m_currentElement->acceptCursor( RightToChild ) )
            tmp = tmp->acceptCursor( RightToChild );
    }

    if( !isEnd() && tmp == m_currentElement )
        m_positionInElement++;
    else
        m_currentElement = tmp;
}

void FormulaCursor::moveUp()
{
}

void FormulaCursor::moveDown()
{
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



bool FormulaCursor::hasSelection() const
{
    return m_selecting;
}

void FormulaCursor::setSelecting( bool selecting )
{
    m_selecting = selecting;
}

void FormulaCursor::setWordMovement( bool wordMovement )
{
    m_wordMovement = wordMovement;
}
