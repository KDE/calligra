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

#include "RootElement.h"
#include "RowElement.h"
#include "FormulaCursor.h"

#include <kdebug.h>

RootElement::RootElement( BasicElement* parent ) : BasicElement( parent )
{
    m_radicand = new RowElement( this );
    m_exponent = 0;
}

RootElement::~RootElement()
{
    delete m_radicand;
    delete m_exponent;
}

const QList<BasicElement*> RootElement::childElements()
{
    QList<BasicElement*> tmp;
    if( m_exponent )
	tmp << m_exponent;
    return tmp << m_radicand;
}

/* FIXME
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
    {
        m_radicand = new SequenceElement;
    }
    else if( tmp == m_exponent && m_exponent->elementType == Basic )
    {
        m_exponent = child;
        cursor->moveCursorTo( m_exponent, 1 );
        delete tmp;
    }
}
 */

void RootElement::removeChild( BasicElement* element )
{
}

void RootElement::paint( QPainter& painter, const AttributeManager* am )
{
}

void RootElement::layout( const AttributeManager* am )
{
    double indexWidth = 0.0;
    double indexHeight = 0.0;
    if ( m_exponent) {
        indexWidth = m_exponent->width();
        indexHeight = m_exponent->height();
    }

    /* FIXME
    double factor = style.sizeFactor();
    luPixel distX = context.ptToPixelX( context.getThinSpace( tstyle, factor ) );
    luPixel distY = context.ptToPixelY( context.getThinSpace( tstyle, factor ) );
    luPixel unit = (m_radicand->getHeight() + distY)/ 3;
    */
    double distX = 0.0;
    double distY = 0.0;
    double unit = m_radicand->height() / 3;

    QPointF exponentOrigin(0, 0);
    m_rootOffset = QPointF(0, 0);
    if (m_exponent) {
        if (indexWidth > unit) {
            m_rootOffset.setX( indexWidth - unit );
        }
        else {
            exponentOrigin.setX( ( unit - indexWidth )/2 );
        }
        if (indexHeight > unit) {
            m_rootOffset.setY( indexHeight - unit );
        }
        else {
            exponentOrigin.setY( unit - indexHeight );
        }
        m_exponent->setOrigin( exponentOrigin );
    }

    setWidth( m_radicand->width() + unit + unit/3 + m_rootOffset.x() + distX/2 );
    setHeight( m_radicand->height() + distY*2 + m_rootOffset.y() );

    m_radicand->setOrigin( QPointF( m_rootOffset.x() + unit + unit/3, m_rootOffset.y() + distY ) );
    setBaseLine(m_radicand->baseLine() + m_radicand->origin().y());
}

void RootElement::moveLeft( FormulaCursor* cursor, BasicElement* from )
{
}

void RootElement::moveRight( FormulaCursor* cursor, BasicElement* from )
{
}

void RootElement::moveUp( FormulaCursor* cursor, BasicElement* from )
{
    if( from == m_radicand )
        cursor->moveCursorTo( m_exponent, -1 );
    else
        parentElement()->moveUp( cursor, from );
}

void RootElement::moveDown( FormulaCursor* cursor, BasicElement* from )
{
    if( from == m_exponent )
        cursor->moveCursorTo( m_radicand, 0 );
    else
        parentElement()->moveDown( cursor, this );
}

ElementType RootElement::elementType() const
{
    return Root;
}

/*
void RootElement::draw( QPainter& painter, const LuPixelRect& r,
                        const ContextStyle& context,
                        ContextStyle::TextStyle tstyle,
                        ContextStyle::IndexStyle istyle,
                        StyleAttributes& style,
                        const LuPixelPoint& parentOrigin )
{
    LuPixelPoint myPos( parentOrigin.x()+getX(), parentOrigin.y()+getY() );
    //if ( !LuPixelRect( myPos.x(), myPos.y(), getWidth(), getHeight() ).intersects( r ) )
    //    return;

    m_radicand->draw( painter, r, context, tstyle,
                      context.convertIndexStyleLower( istyle ), style, myPos);
    if ( m_exponent ) {
        m_exponent->draw( painter, r, context,
                          context.convertTextStyleIndex(tstyle),
                          context.convertIndexStyleUpper(istyle), style, myPos);
    }

    luPixel x = myPos.x() + m_rootOffset.x();
    luPixel y = myPos.y() + m_rootOffset.y();
    //int distX = style.getDistanceX(tstyle);
    double factor = style.sizeFactor();
    luPixel distY = context.ptToPixelY( context.getThinSpace( tstyle, factor ) );
    luPixel unit = (m_radicand->getHeight() + distY)/ 3;

    painter.setPen( QPen( style.color(),
                          context.layoutUnitToPixelX( 2*context.getLineWidth( factor ) ) ) );
    painter.drawLine( context.layoutUnitToPixelX( x+unit/3 ),
                      context.layoutUnitToPixelY( y+unit+distY/3 ),
                      context.layoutUnitToPixelX( x+unit/2+unit/3 ),
                      context.layoutUnitToPixelY( myPos.y()+getHeight() ) );

    painter.setPen( QPen( style.color(),
                          context.layoutUnitToPixelY( context.getLineWidth( factor ) ) ) );

    painter.drawLine( context.layoutUnitToPixelX( x+unit+unit/3 ),
                      context.layoutUnitToPixelY( y+distY/3 ),
                      context.layoutUnitToPixelX( x+unit/2+unit/3 ),
                      context.layoutUnitToPixelY( myPos.y()+getHeight() ) );
    painter.drawLine( context.layoutUnitToPixelX( x+unit+unit/3 ),
                      context.layoutUnitToPixelY( y+distY/3 ),
                      context.layoutUnitToPixelX( x+unit+unit/3+m_radicand->getWidth() ),
                      context.layoutUnitToPixelY( y+distY/3 ) );
    painter.drawLine( context.layoutUnitToPixelX( x+unit/3 ),
                      context.layoutUnitToPixelY( y+unit+distY/2 ),
                      context.layoutUnitToPixelX( x ),
                      context.layoutUnitToPixelY( y+unit+unit/2 ) );
}
*/

/*
void RootElement::writeMathML( KoXmlWriter* writer, bool oasisFormat )
{
    if( m_exponent->elementType() == Basic )
        writer->startElement( oasisFormat ? "math:msqrt" : "msqrt" );
    else
        writer->startElement( oasisFormat ? "math:mroot" : "mroot" );

    writeMathMLAttributes( writer );
    m_radicand->writeMathML( writer, oasisFormat );
    if( m_exponent->elementType() != Basic )
        m_exponent->writeMathML( writer, oasisFormat );

    writer->endElement();
}
*/

bool RootElement::readMathMLContent( const KoXmlElement& element )
{
    kWarning() << "Element name: " << element.tagName() << endl;
}
