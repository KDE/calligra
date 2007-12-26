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

#include "GlyphElement.h"
#include "AttributeManager.h"
#include <QFontDatabase>
#include <QPainter>

GlyphElement::GlyphElement( BasicElement* parent ) : BasicElement( parent )
{}

void GlyphElement::paint( QPainter& painter, AttributeManager* )
{
    painter.drawPath( m_glyphPath );
}

void GlyphElement::layout( const AttributeManager* am )
{
    QString fontFamily = am->stringOf( "fontfamily", this );
    QFontDatabase db;
    QFont tmpFont;
    QString tmpString;

    // determine if the specified font and glyph can be found
    if( db.families().contains( fontFamily ) )
    {
        tmpFont.setFamily( fontFamily );
        tmpString += QChar( am->intOf( "index", this ) ); 
    }
    else // if not found paint alt text
    {
        tmpString = am->stringOf( "alt", this );
    }

    m_glyphPath = QPainterPath();
    m_glyphPath.addText( QPointF( 0, 0 ), tmpFont, tmpString );
    setHeight( m_glyphPath.boundingRect().height() );
    setWidth( m_glyphPath.boundingRect().width() );
    setBaseLine( m_glyphPath.boundingRect().height() );
}

ElementType GlyphElement::elementType() const
{
    return Glyph;
}
