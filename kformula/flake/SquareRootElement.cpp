/* This file is part of the KDE project
   Copyright (C) 2007 Martin Pfeiffer <hubipete@gmx.net>

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

#include "SquareRootElement.h"
#include "AttributeManager.h"
#include <QPainter>
#include <QPen>
#include <kdebug.h>

SquareRootElement::SquareRootElement( BasicElement* parent ) : RowElement( parent )
{
}

SquareRootElement::~SquareRootElement()
{
}

void SquareRootElement::paint( QPainter& painter, AttributeManager* am )
{
    QPen pen ( am->mathColor( this ) );
    pen.setWidth( 1 );
    painter.setPen( pen );
    painter.drawPath( m_rootSymbol );
}

void SquareRootElement::layout( const AttributeManager* am )
{
    RowElement::layout( am );

    kDebug() << "Width: " << width();
    kDebug() << "Height: " << height();

    double distY = am->mathSpaceValue( "thinmathspace" );
    setHeight( 2*distY + height() );

    m_rootSymbol = QPainterPath();
    m_rootSymbol.moveTo( 0.0, 2.0 * height() /3.0 );
    m_rootSymbol.lineTo( height() / 6.0, height());
    m_rootSymbol.lineTo( height() / 3.0, 0 );
   
    QPointF tmp(height() / 3.0, distY);

    foreach( BasicElement* element, childElements() )
        element->setOrigin( element->origin() + tmp );

    m_rootSymbol.lineTo( height()/3.0 + width(), 0 );
    
    setWidth( m_rootSymbol.boundingRect().width() );
    setBaseLine( baseLine() + origin().y() );
}

ElementType SquareRootElement::elementType() const
{
    return SquareRoot;
}

