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
#include "ElementFactory.h"
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
    QPen pen( painter.pen() );
    pen.setWidthF( am->doubleOf( "linethickness", this ) );

    painter.drawPath( m_rootSymbol );
}

void RootElement::layout( const AttributeManager* am )
{
    QPointF tmp;
    double distY = am->mathSpaceValue( "thinmathspace" );
    setHeight( 2*distY + m_radicand->height() );

    m_rootSymbol = QPainterPath();
    tmp += QPointF( 0.0, 2/3 * height() );
    m_rootSymbol.moveTo( tmp );
    tmp += QPointF( m_exponent->width(), 2/3 * height() );
    m_rootSymbol.moveTo( tmp );
    tmp += QPointF( 1/6 * height(), 1/3 * height() );
    m_rootSymbol.moveTo( tmp );
    tmp = QPointF( tmp.x() + 1/6 * height(), 0.0 );
    m_rootSymbol.moveTo( tmp );
    m_radicand->setOrigin( tmp + QPointF( 0.0, distY ) );
    tmp += QPointF( m_radicand->width(), 0.0 );
    m_rootSymbol.moveTo( tmp );
    
    m_exponent->setOrigin( QPointF( 0.0, distY ) );
    setWidth( m_rootSymbol.boundingRect().width() );
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

void RootElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    m_radicand->writeMathML( writer );
    if ( m_exponent )
        m_exponent->writeMathML( writer );
}

bool RootElement::readMathMLContent( const KoXmlElement& element )
{
    KoXmlElement tmp;
    forEachElement( tmp, element ) {
        if( m_radicand->elementType() == Basic ) {
            delete m_radicand;
            m_radicand = ElementFactory::createElement( tmp.tagName(), this );
        }
        else if( m_exponent->elementType() == Basic ) {
            delete m_exponent;
            m_exponent = ElementFactory::createElement( tmp.tagName(), this );
        }
    }
    return true;
}
