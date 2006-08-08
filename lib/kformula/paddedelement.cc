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

#include "elementtype.h"
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

/**
 * Calculates our width and height and
 * our children's parentPosition.
 */
void PaddedElement::calcSizes( const ContextStyle& context,
                               ContextStyle::TextStyle tstyle,
                               ContextStyle::IndexStyle istyle,
                               StyleAttributes& style )
{
    double factor = style.sizeFactor();

    luPixel width = 0;
    luPixel height = 0;
    luPixel depth = 0;
    luPixel spaceBefore = 0;

    if ( !isEmpty() ) {
        // First, get content height and width
        for ( iterator it = begin(); it != end(); ++it ) {
            if ( it == begin() ) {
                spaceBefore =
                    context.ptToPixelX( it->getElementType()->getSpaceBefore( context,
                                                                              tstyle,
                                                                              factor ) );
            }
            it->calcSizes( context, tstyle, istyle, style );
            width += it->getWidth() + spaceBefore;
            luPixel baseline = it->getBaseline();
            if ( baseline > -1 ) {
                height = QMAX( height, baseline );
                depth = QMAX( depth, it->getHeight() - baseline );
            }
            else {
                luPixel bl = it->getHeight()/2 + context.axisHeight( tstyle, factor );
                height = QMAX( height, bl );
                depth = QMAX( depth, it->getHeight() - bl );
            }
        }
    }
    else {
        width = context.getEmptyRectWidth( factor );
        height = context.getEmptyRectHeight( factor );
        depth = 0;
    }

    luPixel left = calcSize( context, m_lspaceType, m_lspace, width, height, 0 );
    luPixel right = calcSize( context, m_widthType, m_width, width, height, width ) + left;
    luPixel down = calcSize( context, m_depthType, m_depth, width, height, depth );
    luPixel up = calcSize( context, m_heightType, m_height, width, height, height );

    // Check borders
    if ( right < 0 ) right = 0;
    if ( up + down < 0 ) up = down = 0;
    
    if ( ! isEmpty() ) {
        width = left;
        // Let's do all normal elements that have a base line.
        for ( iterator it = begin(); it != end(); ++it ) {
            it->calcSizes( context, tstyle, istyle, style );
            it->setX( width + spaceBefore );
            width += it->getWidth() + spaceBefore;
        }

        setWidth( right );
        setHeight( up + down );
        setBaseline( up );
        setChildrenPositions();
    }
    else {
        setWidth( right );
        setHeight( up + down );
        setBaseline( up );
    }
}

bool PaddedElement::readAttributesFromMathMLDom(const QDomElement& element)
{
    if ( ! BasicElement::readAttributesFromMathMLDom( element ) ) {
        return false;
    }

    QString widthStr = element.attribute( "width" ).stripWhiteSpace().lower();
    if ( ! widthStr.isNull() ) {
        m_width = readSizeAttribute( widthStr, &m_widthType );
    }
    QString lspaceStr = element.attribute( "lspace" ).stripWhiteSpace().lower();
    if ( ! lspaceStr.isNull() ) {
        m_lspace = readSizeAttribute( lspaceStr, &m_lspaceType );
    }
    QString heightStr = element.attribute( "height" ).stripWhiteSpace().lower();
    if ( ! heightStr.isNull() ) {
        m_height = readSizeAttribute( heightStr, &m_heightType );
    }
    QString depthStr = element.attribute( "depth" ).stripWhiteSpace().lower();
    if ( ! depthStr.isNull() ) {
        m_depth = readSizeAttribute( depthStr, &m_depthType );
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


double PaddedElement::readSizeAttribute( const QString& str, SizeType* st )
{
    if ( st == 0 ){
        return -1;
    }
    if ( str[0] == '+' || str[0] == '-' ) {
        m_relative = true;
    }
    int index = str.find( "width" );
    if ( index != -1 ) {
        int index2 = str.find( "%" );
        if ( index2 != -1 ) {
            return str2size( str, st, index2, WidthRelativeSize ) / 100.0;
        }
        return str2size( str, st, index, WidthRelativeSize );
    }
    index = str.find( "height" );
    if ( index != -1 ) {
        int index2 = str.find( "%" );
        if ( index2 != -1 ) {
            return str2size( str, st, index2, HeightRelativeSize ) / 100.0;
        }
        return str2size( str, st, index, HeightRelativeSize );
    }
    index = str.find( "%" );
    if ( index != -1 ) {
        return str2size( str, st, index, RelativeSize ) / 100.0;
    }
    index = str.find( "pt", 0, false );
    if ( index != -1 ) {
        return str2size( str, st, index, AbsoluteSize );
    }
    index = str.find( "mm", 0, false );
    if ( index != -1 ) {
        return str2size( str, st, index, AbsoluteSize ) * 72.0 / 20.54;
    }
    index = str.find( "cm", 0, false );
    if ( index != -1 ) {
        return str2size( str, st, index, AbsoluteSize ) * 72.0 / 2.54;
    }
    index = str.find( "in", 0, false );
    if ( index != -1 ) {
        return str2size( str, st, index, AbsoluteSize ) * 72.0;
    }
    index = str.find( "em", 0, false );
    if ( index != -1 ) {
        return str2size( str, st, index, RelativeSize );
    }
    index = str.find( "ex", 0, false );
    if ( index != -1 ) {
        return str2size( str, st, index, RelativeSize );
    }
    index = str.find( "pc", 0, false );
    if ( index != -1 ) {
        return str2size( str, st, index, AbsoluteSize ) * 12.0;
    }
    index = str.find( "px", 0, false );
    if ( index != -1 ) {
        return str2size( str, st, index, PixelSize );
    }
    // If there's no unit, assume 'pt'
    return str2size( str, st, str.length(), AbsoluteSize );
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

luPixel PaddedElement::calcSize( const ContextStyle& context, SizeType type, 
                                 double length, luPixel width, 
                                 luPixel height, luPixel defvalue )
{
    luPixel value = defvalue;
    switch ( type ) {
    case AbsoluteSize:
        if ( m_relative )
            value += context.ptToLayoutUnitPt ( length );
        else
            value = context.ptToLayoutUnitPt( length );
        break;
    case RelativeSize:
        if ( m_relative )
            value += length * value;
        else
            value *= length;
    case WidthRelativeSize:
        if ( m_relative )
            value += length * width;
        else
            value = length * width;
        break;
    case HeightRelativeSize:
        if ( m_relative )
            value += length * height;
        else
            value = length * height;
        break;
    case PixelSize:
        if ( m_relative )
            value += context.pixelToLayoutUnitX( length );
        else
            value = context.pixelToLayoutUnitX( length );
        break;
    default:
        break;
    }
    return value;
}

KFORMULA_NAMESPACE_END
