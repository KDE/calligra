/* This file is part of the KDE project
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

#include "TokenElement.h"
#include "AttributeManager.h"
#include <KoXmlWriter.h>
#include <QPainter>
#include <QFontMetrics>

namespace FormulaShape {

TokenElement::TokenElement( BasicElement* parent ) : BasicElement( parent )
{}

void TokenElement::paint( QPainter& painter, const AttributeManager* )
{
     // Just draw the text
     painter.drawText( 0, 0, stringToRender( m_rawString ) );
}

void TokenElement::layout( const AttributeManager* am )
{
    QFontMetrics fm( am->font() );
    QRectF tmpRect = fm.boundingRect( stringToRender( m_rawString ) );
    setWidth( tmpRect.width() );
    setHeight( tmpRect.height() );
    setBaseLine( height() );  // TODO is this correct ??
}

bool TokenElement::readMathMLContent( const KoXmlElement& element )
{
    m_rawString = element.text();
    return true;
}

void TokenElement::writeMathMLContent( KoXmlWriter* writer ) const
{
    writer->addTextNode( m_rawString );
}

QString TokenElement::stringToRender( const QString& rawString ) const
{
    return rawString;
}

} // namespace FormulaShape
