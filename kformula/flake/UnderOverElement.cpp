/* This file is part of thShapee KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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
#include "ElementFactory.h"
#include <KoXmlWriter.h>

namespace FormulaShape {

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

void UnderOverElement::paint( QPainter& painter, const AttributeManager* am )
{
}

void UnderOverElement::layout( const AttributeManager* am )
{
}

void UnderOverElement::moveLeft( FormulaCursor* cursor, BasicElement* from )
{
}

void UnderOverElement::moveRight( FormulaCursor* cursor, BasicElement* from )
{
}

void UnderOverElement::moveUp( FormulaCursor* cursor, BasicElement* from )
{
}

void UnderOverElement::moveDown( FormulaCursor* cursor, BasicElement* from )
{
}

bool UnderOverElement::readMathMLContent( const KoXmlElement& element )
{
    BasicElement* tmpElement = 0;
    KoXmlElement tmp;

    forEachElement( tmp, element )
    {
        tmpElement = ElementFactory::createElement( tmp.tagName(), this );
        if( !m_baseElement )
            m_baseElement = tmpElement;
        else if( !m_underElement && element.tagName().contains( "under" ) )
            m_underElement = tmpElement;
        else if( !m_overElement && element.tagName().contains( "over" ) )
            m_overElement = tmpElement;
    }
}

void UnderOverElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    m_baseElement->writeMathML( writer );        // Just save the children in
                                                 // the right order
    if( m_underElement->elementType() != Basic )
        m_underElement->writeMathML( writer );
    
    if( m_overElement->elementType() != Basic )
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
}

} // namespace FormulaShape
