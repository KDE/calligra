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
 * Boston, MA 02110-1301, USA.
*/

#include "glyphelement.h"

KFORMULA_NAMESPACE_BEGIN

GlyphElement::GlyphElement( BasicElement* parent ) : TextElement( ' ', false, parent ) {
}

bool GlyphElement::readAttributesFromMathMLDom( const QDomElement& element )
{
    if ( !BasicElement::readAttributesFromMathMLDom( element ) ) {
        return false;
    }

    // MathML Section 3.2.9.2
    m_fontFamily = element.attribute( "fontfamily" );
    if ( m_fontFamily.isNull() ) {
        kdWarning( DEBUGID ) << "Required attribute fontfamily not found in glyph element\n";
        return false;
    }
    QString indexStr = element.attribute( "index" );
    if ( indexStr.isNull() ) {
        kdWarning( DEBUGID ) << "Required attribute index not found in glyph element\n";
        return false;
    }
    bool ok;
    ushort index = indexStr.toUShort( &ok );
    if ( ! ok ) {
        kdWarning( DEBUGID ) << "Invalid index value in glyph element\n";
        return false;
    }
    m_char = QChar( index );

    m_alt = element.attribute( "alt" );
    if ( m_alt.isNull() ) {
        kdWarning( DEBUGID ) << "Required attribute alt not found in glyph element\n";
        return false;
    }

    return inherited::readAttributesFromMathMLDom( element );
}
    

void GlyphElement::writeMathML( QDomDocument& doc, QDomNode& parent, bool oasisFormat )
{
    QDomElement de = doc.createElement( oasisFormat ? "math:mglyph" : "mglyph" );
    writeMathMLAttributes( de );
    parent.appendChild( de );
}

void GlyphElement::writeMathMLAttributes( QDomElement& element )
{
    element.setAttribute( "fontfamily", m_fontFamily );
    element.setAttribute( "index", m_char.unicode() );
    element.setAttribute( "alt", m_alt );
}

KFORMULA_NAMESPACE_END
