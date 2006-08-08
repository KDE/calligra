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

#include "paddedelement.h"

KFORMULA_NAMESPACE_BEGIN

PaddedElement::PaddedElement( BasicElement* parent ) : SequenceElement( parent ),
                                                       m_widthType( NoSize ),
                                                       m_lspaceType( NoSize ),
                                                       m_heightType( NoSize ),
                                                       m_depthType( NoSize ),
                                                       m_relative( false )
{
}

bool PaddedElement::readAttributesFromMathMLDom(const QDomElement& element)
{
    if ( ! BasicElement::readAttributesFromMathMLDom( element ) ) {
        return false;
    }

    QString widthStr = element.attribute( "width" ).stripWhiteSpace().lower();
    if ( ! widthStr.isNull() ) {
        readSizeAttribute( widthStr, &m_widthType, &m_width );
    }
    QString lspaceStr = element.attribute( "lspace" ).stripWhiteSpace().lower();
    if ( ! lspaceStr.isNull() ) {
        readSizeAttribute( lspaceStr, &m_lspaceType, &m_lspace );
    }
    QString heightStr = element.attribute( "height" ).stripWhiteSpace().lower();
    if ( ! heightStr.isNull() ) {
        readSizeAttribute( heightStr, &m_heightType, &m_height );
    }
    QString depthStr = element.attribute( "depth" ).stripWhiteSpace().lower();
    if ( ! depthStr.isNull() ) {
        readSizeAttribute( depthStr, &m_depthType, &m_depth );
    }

    return true;
}

void PaddedElement::writeMathML( QDomDocument& doc, QDomNode& parent, bool oasisFormat )
{
    QDomElement de = doc.createElement( oasisFormat ? "math:mpadded" : "mpadded" );
    writeMathMLAttributes( de );
    writeMathMLContent( doc, de, oasisFormat );
    parent.appendChild( de );
}

void PaddedElement::writeMathMLAttributes( QDomElement& element )
{
    writeSizeAttribute( element, "width", m_widthType, m_width );
    writeSizeAttribute( element, "lspace", m_lspaceType, m_lspace );
    writeSizeAttribute( element, "height", m_heightType, m_height );
    writeSizeAttribute( element, "depth", m_depthType, m_depth );
}

void PaddedElement::writeMathMLContent( QDomDocument& doc, QDomElement& element, bool oasisFormat )
{
    for ( iterator it = begin(); it != end(); it++ ) {
        it->writeMathML( doc, element, oasisFormat );
    }
}


void PaddedElement::readSizeAttribute( const QString& str, SizeType* st, double* s )
{
    if ( st == 0 || s == 0 ){
        return;
    }
    if ( str[0] == '+' || str[0] == '-' ) {
        m_relative = true;
    }
    int index = str.find( "width" );
    if ( index != -1 ) {
        int index2 = str.find( "%" );
        if ( index2 != -1 ) {
            *s = str2size( str, st, index2, WidthRelativeSize ) / 100.0;
        }
        *s = str2size( str, st, index, WidthRelativeSize );
    }
    index = str.find( "height" );
    if ( index != -1 ) {
        int index2 = str.find( "%" );
        if ( index2 != -1 ) {
            *s = str2size( str, st, index2, HeightRelativeSize ) / 100.0;
        }
        *s = str2size( str, st, index, HeightRelativeSize );
    }
    index = str.find( "%" );
    if ( index != -1 ) {
        *s = str2size( str, st, index, RelativeSize ) / 100.0;
    }
    index = str.find( "pt", 0, false );
    if ( index != -1 ) {
        *s = str2size( str, st, index, AbsoluteSize );
    }
    index = str.find( "mm", 0, false );
    if ( index != -1 ) {
        *s = str2size( str, st, index, AbsoluteSize ) * 72.0 / 20.54;
    }
    index = str.find( "cm", 0, false );
    if ( index != -1 ) {
        *s = str2size( str, st, index, AbsoluteSize ) * 72.0 / 2.54;
    }
    index = str.find( "in", 0, false );
    if ( index != -1 ) {
        *s = str2size( str, st, index, AbsoluteSize ) * 72.0;
    }
    index = str.find( "em", 0, false );
    if ( index != -1 ) {
        *s = str2size( str, st, index, RelativeSize );
    }
    index = str.find( "ex", 0, false );
    if ( index != -1 ) {
        *s = str2size( str, st, index, RelativeSize );
    }
    index = str.find( "pc", 0, false );
    if ( index != -1 ) {
        *s = str2size( str, st, index, AbsoluteSize ) * 12.0;
    }
    index = str.find( "px", 0, false );
    if ( index != -1 ) {
        *s = str2size( str, st, index, PixelSize );
    }
    // If there's no unit, assume 'pt'
    *s = str2size( str, st, str.length(),AbsoluteSize );
}

double PaddedElement::str2size( const QString& str, SizeType *st, uint index, SizeType type )
{
    QString num = str.left( index );
    bool ok;
    double size = num.toDouble( &ok );
    if ( ok ) {
        if ( st ) {
            *st = type;
        }
        return size;
    }
    if ( st ) {
        *st = NoSize;
    }
    return -1;
}

void PaddedElement::writeSizeAttribute( QDomElement element, const QString& str, SizeType st, double s )
{
    QString prefix;
    if ( m_relative ) {
        s < 0 ? prefix = "-" : prefix = "+" ;
    }
    switch ( st ) {
    case WidthRelativeSize:
        element.setAttribute( str, prefix + QString( "%1 width" ).arg( s ) );
        break;
    case HeightRelativeSize:
        element.setAttribute( str, prefix + QString( "%1 height" ).arg( s ) );
    case AbsoluteSize:
        element.setAttribute( str, prefix + QString( "%1pt" ).arg( s ) );
        break;
    case RelativeSize:
        element.setAttribute( str, prefix + QString( "%1%" ).arg( s * 100.0 ) );
        break;
    case PixelSize:
        element.setAttribute( str, prefix + QString( "%1px" ).arg( s ) );
        break;
    }
}

KFORMULA_NAMESPACE_END
