/* This file is part of thShapee KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2006-2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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

#include "UnderOverElement.h"
#include "AttributeManager.h"
#include <KoXmlReader.h>
#include <kdebug.h>
#include <QPainter>

UnderOverElement::UnderOverElement( BasicElement* parent ) : BasicElement( parent )
{
    m_baseElement = new BasicElement( this );
    m_underElement = new BasicElement( this );
    m_overElement = new BasicElement( this );
}

UnderOverElement::~UnderOverElement()
{
    delete m_baseElement;
    delete m_underElement;
    delete m_overElement;
}

const QList<BasicElement*> UnderOverElement::childElements()
{
    QList<BasicElement*> tmp;
    return tmp << m_baseElement << m_underElement << m_overElement;
}

void UnderOverElement::paint( QPainter& painter, AttributeManager* am)
{
    /*do nothing as UnderOverElement has no visual representance*/
    Q_UNUSED(painter)
    Q_UNUSED(am)
}

void UnderOverElement::layout( const AttributeManager* am )
{
    double thinSpace   = am->layoutSpacing( this );
    double accent      = am->boolOf( "accent", this );     //Whether to add a space above
    double accentUnder = am->boolOf( "accentunder", this );//Whether to add a space below

    double largestWidth = m_baseElement->width();
    largestWidth = qMax( m_underElement->width(), largestWidth );
    largestWidth = qMax( m_overElement->width(), largestWidth );

    QPointF origin( ( largestWidth - m_overElement->width() ) / 2.0, 0.0 );
    m_overElement->setOrigin( origin );

    origin.setX( ( largestWidth - m_baseElement->width() ) / 2.0 );
    origin.setY( ( accent && m_overElement->height() != 0 ) ? 2*thinSpace : thinSpace );
    m_baseElement->setOrigin( origin );

    origin.setX( ( largestWidth - m_underElement->width())/2.0 );
    origin.setY( origin.y() + accentUnder ? 2*thinSpace : thinSpace );
    m_underElement->setOrigin( origin );

    setWidth( largestWidth );
    setHeight( origin.y() + m_underElement->height() );
    setBaseLine( m_baseElement->origin().y() + m_baseElement->baseLine() );
}

BasicElement* UnderOverElement::acceptCursor( const FormulaCursor* cursor )
{
    return 0;
}

QString UnderOverElement::attributesDefaultValue( const QString& attribute ) const
{
/*    if( m_overElement->elementType() == Operator )
    else if( m_underElement->elementType() == Operator )*/
    return "false";  // the default for accent and
}

bool UnderOverElement::readMathMLContent( const KoXmlElement& parent )
{
    QString name = parent.tagName().toLower();
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;
    forEachElement( tmp, parent ) { 
        tmpElement = ElementFactory::createElement( tmp.tagName(), this );
        if( !tmpElement->readMathML( tmp ) )
            return false;

        if( m_baseElement->elementType() == Basic ) {
            delete m_baseElement; 
            m_baseElement = tmpElement;
        }
        else if( name.contains( "under" ) && m_underElement->elementType() == Basic ) {
            delete m_underElement;
            m_underElement = tmpElement;
        }
        else if( name.contains( "over" ) && m_overElement->elementType() == Basic ) {
            delete m_overElement;
            m_overElement = tmpElement;
        }
        else
            return false;
    }
    Q_ASSERT( m_baseElement );  // We should have at least a BasicElement for the base
    Q_ASSERT( m_underElement || m_overElement );
    return true;
} 

void UnderOverElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    m_baseElement->writeMathML( writer );   // Just save the children in
    m_underElement->writeMathML( writer );  // the right order
    m_overElement->writeMathML( writer );
}

ElementType UnderOverElement::elementType() const
{
    if( m_underElement->elementType() != Basic && m_overElement->elementType() != Basic )
        return UnderOver;
    else if( m_underElement->elementType() != Basic )
        return Under;
    else if( m_overElement->elementType() != Basic )
        return Over;
    else
        return UnderOver;
}
