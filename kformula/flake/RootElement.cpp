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
    Q_UNUSED(am)
    QPen pen ( am->mathColor( this ) );
    pen.setWidth( 1 );
    painter.setPen( pen );
    painter.drawPath( m_rootSymbol );
}

void RootElement::layout( const AttributeManager* am )
{
    Q_UNUSED(am)
    kDebug() << "Radicand height: " << m_radicand->height();
    kDebug() << "Radicand width: " << m_radicand->width();
    kDebug() << "Exponent height: " << m_exponent->height();
    kDebug() << "Exponent width: " << m_exponent->width();

    double distY = am->mathSpaceValue( "thinmathspace" );
    
    //In the unlikely case that the exponent is actually taller than the radicand, try to cope with that
    double largestheight = qMax( 2*distY + m_radicand->height(), m_exponent->height() );
    setHeight( largestheight );

    m_rootSymbol = QPainterPath();
    
    double offset = m_exponent->width() - height() / 6.0;
    if(offset < 0) offset = 0;

    //Draw the root symbol bit
    m_rootSymbol.moveTo( offset, 2.0 * height() / 3.0 );
    m_rootSymbol.lineTo( offset + height() / 6.0, height());
    m_rootSymbol.lineTo( offset + height() / 3.0, 0 );
    
    //Place the child in the correct place
    m_radicand->setOrigin( QPointF(offset + height() / 3.0, distY) );
    //Draw a line over the child
    m_rootSymbol.lineTo( offset + height() / 3.0 + m_radicand->width(), 0.0 );
   
    //Place the exponent in the correct place
    m_exponent->setOrigin( QPointF(0,0) );
    setWidth( offset + height() / 3.0 + m_radicand->width() );
    setBaseLine( m_radicand->baseLine() + m_radicand->origin().y() ); 
}

const QList<BasicElement*> RootElement::childElements()
{
    QList<BasicElement*> tmp;
    tmp << m_radicand << m_exponent;
    return tmp;
}

/*
void RootElement::insertChild( FormulaCursor* cursor, BasicElement* child )
{
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
    }
}
*/

BasicElement* RootElement::acceptCursor( CursorDirection direction )
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

