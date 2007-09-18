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
    BasicElement::paint(painter, am);
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

    double thinspace = am->mathSpaceValue( "thinmathspace" );

    double sqrtHeight = baseLine();
    double totalHeight = height();

    sqrtHeight += thinspace;
    totalHeight += thinspace;
    setHeight(totalHeight);
    setBaseLine(sqrtHeight);
   
    double tickWidth = sqrtHeight / 3.0;  //The width of the tick part of the square root symbol

    //Place the child in the correct place
    QPointF childOffset(tickWidth, thinspace);
    foreach( BasicElement* element, childElements() )
        element->setOrigin( element->origin() + childOffset );

    m_rootSymbol = QPainterPath();
    
    //Draw the root symbol bit
    m_rootSymbol.moveTo( 0, 2.0 * sqrtHeight / 3.0 );
    m_rootSymbol.lineTo( 0 + tickWidth/2.0, sqrtHeight);
    m_rootSymbol.lineTo( 0 + tickWidth, 0.0 );
    
    //Draw a line over the child (aka radicand)
    double totalWidth = tickWidth + width();
    m_rootSymbol.lineTo( totalWidth, 0.0 );
   
    setWidth( totalWidth );
}

ElementType SquareRootElement::elementType() const
{
    return SquareRoot;
}

