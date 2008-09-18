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

GlyphElement::GlyphElement( BasicElement* parent ) : TokenElement( parent )
{}

void GlyphElement::renderToPath( const QString& raw, QPainterPath& path )
{
    // try to lookup the char in the font database
    AttributeManager am;
    QString fontFamily = am.stringOf( "fontfamily", this );
    QFontDatabase db;
    QFont tmpFont;

    // determine if the specified font and glyph can be found
    if( db.families().contains( fontFamily ) )
    {
        tmpFont.setFamily( fontFamily );
        path.addText( path.currentPosition(), tmpFont,
                      QChar( am.stringOf( "index", this ).toInt() ) ); 
    }
    else // if not found use alt text
        path.addText( path.currentPosition(), font(), am.stringOf( "alt", this ) );
}

ElementType GlyphElement::elementType() const
{
    return Glyph;
}
