/* This file is part of the KDE project
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

#include "EncloseElement.h"

namespace FormulaShape {

EncloseElement::EncloseElement( BasicElement* parent ) : BasicElement( parent )
{
}

void EncloseElement::paint( QPainter& painter, const AttributeManager* )
{
    painter.drawPath( m_enclosePath );
}

void EncloseElement::layout( AttributeManager* am )
{
    qreal width = m_enclosedRow.width();      
    qreal height = m_enclosedRow.height();
 


    // TODO: actuarial (how does it look?) - radical - circle (how to determine extends )

    // longdiv
        m_enclosePath.moveTo();
        m_enclosePath.lineTo();

    // left
        m_enclosePath.moveTo( 0, 0 );
        m_enclosePath.lineTo( 0, m_enclosedRow.height() );

    // right
        m_enclosePath.moveTo( m_enclosedRow.width(), 0 );
        m_enclosePath.lineTo( m_enclosedRow.width(), m_enclosedPath.height() );

    // top
        m_enclosePath.moveTo( 0, 0 );
        m_enclosePath.lineTo( m_enclosedRow.width(), 0 );

    // bottom
        m_enclosePath.moveTo( m_enclosedRow.height(), 0 );
        m_enclosePath.lineTo( m_enclosedRow.height(), m_enclosedRow.width() );

    // box - TODO spacing is missing - might look odd
        m_enclosePath.addRect( 0, 0, m_enclosedRow.width(), m_enclosedRow.height() );

    // roundedbox - TODO spacing is missing - might look odd
        m_enclosePath.addRoundedRect( 0, 0, m_enclosedRow.width(),
                                            m_enclosedRow.height(), 25 );

    // updiagonalstrike
        m_enclosePath.moveTo( 0, m_enclosedRow.height() );
        m_enclosePath.lineTo( m_enclosedRow.width(), 0 );

    // downdiagonalstrike
        m_enclosePath.moveTo( 0, 0 );
        m_enclosePath.lineTo( m_enclosedRow.width(), m_enclosedRow.height() );

    // verticalstrike
        m_enclosePath.moveTo( m_enclosedRow.width()/2, 0 );
        m_enclosePath.lineTo( m_enclosedRow.width()/2, m_enclosedRow.height() );

    // horizontalstrike
        m_enclosePath.moveTo( 0, m_encloseRow.height()/2 );
        m_enclosePath.lineTo( m_encloseRow.width(), m_encloseRow.height()/2 );

    setWidth( width );
    setHeight( height );
    m_enclosedRow.
    m_enclosePath.
}

void EncloseElement::readMathMLAttributes(const QDomElement& element)
{
    m_notation = element.attribute( "notation" );
}

void EncloseElement::writeMathMLAttributes( QDomElement& element ) const
{
    if ( ! m_notation.isNull() ) {
        element.setAttribute( "notation", m_notation );
    }
}

} // namespace FormulaShape
