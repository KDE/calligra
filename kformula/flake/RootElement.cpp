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
#include <KoXmlReader.h>
#include <QPainter>
#include <QPen>
#include <kdebug.h>

RootElement::RootElement( BasicElement* parent ) : BasicElement( parent )
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
    BasicElement::paint(painter, am); // For debugging
    QPen pen;
    pen.setWidth( 1 );
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
    double linethickness = 1;

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
    m_rootSymbol.moveTo( xOffset+linethickness, yOffset +  2.0 * symbolHeight / 3.0 );
    m_rootSymbol.lineTo( m_rootSymbol.currentPosition().x()+tickWidth*0.5, yOffset + symbolHeight - linethickness/2 );
    m_rootSymbol.lineTo( m_rootSymbol.currentPosition().x()+tickWidth*0.5, yOffset + linethickness/2 );
    m_rootSymbol.lineTo( width()-linethickness/2, yOffset +linethickness/2);
}

const QList<BasicElement*> RootElement::childElements()
{
    QList<BasicElement*> tmp;
    tmp << m_radicand << m_exponent;
    return tmp;
}


void RootElement::insertChild( FormulaCursor* cursor, BasicElement* child )
{/*
    BasicElement* tmp = cursor->currentElement();
    if( tmp == m_radicand && m_radicand->elementType == Basic )
    {
        m_radicand = child;
        cursor->moveCursorTo( m_radicand, 1 );
        delete tmp;
    }
    else if( tmp == m_radicand && m_radicant->elementType != Basic )
        m_radicand = m_cursor->enqueueElement( child );
    {
        BasicElement* tmp = m_radicand;
        m_radicand = new RowElement( this );
        m_radicand->insertElement( tmp );
    }
    else if( tmp == m_exponent && m_exponent->elementType == Basic )
    {
        m_exponent = child;
        cursor->moveCursorTo( m_exponent, 1 );
        delete tmp;
    }*/
}

void RootElement::removeChild( FormulaCursor* cursor, BasicElement* child )
{
}

BasicElement* RootElement::acceptCursor( const FormulaCursor* cursor )
{
    return 0;
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
            kDebug() << "Too many arguments to mroot" << endl;
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

