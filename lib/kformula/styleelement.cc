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

#include "basicelement.h"
#include "styleelement.h"

KFORMULA_NAMESPACE_BEGIN

StyleElement::StyleElement( BasicElement* parent ) : SequenceElement( parent ),
                                                     m_mathSizeType ( NoSize ),
                                                     m_charStyle( anyChar ),
                                                     m_charFamily( anyFamily ),
                                                     m_mathColor( "#000000" ),
                                                     m_mathBackground( "#FFFFFF" ),
                                                     m_fontSizeType( NoSize ),
                                                     m_customMathVariant( false ),
                                                     m_customMathColor( false ),
                                                     m_customMathBackground( false ),
                                                     m_customFontWeight( false ),
                                                     m_customFontStyle( false ),
                                                     m_customFontFamily( false ),
                                                     m_customColor( false )
{
}

void StyleElement::calcSizes( const ContextStyle& context,
                              ContextStyle::TextStyle tstyle,
                              ContextStyle::IndexStyle istyle,
                              StyleAttributes& style )
{
    style.setSizeFactor( sizeFactor( context, style.sizeFactor() ) );
    setStyleVariant( style );
    setStyleColor( style );
    setStyleBackground( style );
    inherited::calcSizes( context, tstyle, istyle, style );
    style.reset();
}

void StyleElement::draw( QPainter& painter, const LuPixelRect& r,
                         const ContextStyle& context,
                         ContextStyle::TextStyle tstyle,
                         ContextStyle::IndexStyle istyle,
                         StyleAttributes& style,
                         const LuPixelPoint& parentOrigin )
{
    style.setSizeFactor( sizeFactor( context, style.sizeFactor() ) );
    setStyleVariant( style );
    setStyleColor( style );
    setStyleBackground( style );
    inherited::draw( painter, r, context, tstyle, istyle, style, parentOrigin );
    style.reset();
}

// TODO: Support for deprecated attributes
bool StyleElement::readAttributesFromMathMLDom( const QDomElement& element )
{
    if ( !BasicElement::readAttributesFromMathMLDom( element ) ) {
        return false;
    }

    // MathML Section 3.2.2
    QString variantStr = element.attribute( "mathvariant" );
    if ( !variantStr.isNull() ) {
        if ( variantStr == "normal" ) {
            setCharStyle( normalChar );
            setCharFamily( normalFamily );
        }
        else if ( variantStr == "bold" ) {
            setCharStyle( boldChar );
            setCharFamily( normalFamily );
        }
        else if ( variantStr == "italic" ) {
            setCharStyle( italicChar );
            setCharFamily( normalFamily );
        }
        else if ( variantStr == "bold-italic" ) {
            setCharStyle( boldItalicChar );
            setCharFamily( normalFamily );
        }
        else if ( variantStr == "double-struck" ) {
            setCharStyle( normalChar );
            setCharFamily( doubleStruckFamily );
        }
        else if ( variantStr == "bold-fraktur" ) {
            setCharStyle( boldChar );
            setCharFamily( frakturFamily );
        }
        else if ( variantStr == "script" ) {
            setCharStyle( normalChar );
            setCharFamily( scriptFamily );
        }
        else if ( variantStr == "bold-script" ) {
            setCharStyle( boldChar );
            setCharFamily( scriptFamily );
        }
        else if ( variantStr == "fraktur" ) {
            setCharStyle( boldChar );
            setCharFamily( frakturFamily );
        }
        else if ( variantStr == "sans-serif" ) {
            setCharStyle( normalChar );
            setCharFamily( sansSerifFamily );
        }
        else if ( variantStr == "bold-sans-serif" ) {
            setCharStyle( boldChar );
            setCharFamily( sansSerifFamily );
        }
        else if ( variantStr == "sans-serif-italic" ) {
            setCharStyle( italicChar );
            setCharFamily( sansSerifFamily );
        }
        else if ( variantStr == "sans-serif-bold-italic" ) {
            setCharStyle( boldItalicChar );
            setCharFamily( sansSerifFamily );
        }
        else if ( variantStr == "monospace" ) {
            setCharStyle( normalChar );
            setCharFamily( monospaceFamily );
        }
    }

    QString sizeStr = element.attribute( "mathsize" );
    if ( !sizeStr.isNull() ) {
        if ( sizeStr == "small" ) {
            setRelativeSize( 0.8 ); // ### Arbitrary size
        }
        else if ( sizeStr == "normal" ) {
            setRelativeSize( 1.0 );
        }
        else if ( sizeStr == "big" ) {
            setRelativeSize( 1.2 ); // ### Arbitrary size
        }
        else {
            double s = getSize( sizeStr, &m_mathSizeType );
            switch ( m_mathSizeType ) {
            case AbsoluteSize:
                setAbsoluteSize( s );
                break;
            case RelativeSize:
                setRelativeSize( s );
                break;
            case PixelSize:
                setPixelSize( s );
                break;
            }
        }
    }

    QString colorStr = element.attribute( "mathcolor" );
    if ( !colorStr.isNull() ) {
        // TODO: Named colors differ from those Qt supports. See section 3.2.2.2
        kdWarning() << "Setting color: " << colorStr << endl;
        setMathColor( QColor( colorStr ) );
	}
    QString backgroundStr = element.attribute( "mathbackground" );
    if ( !backgroundStr.isNull() ) {
        // TODO: Named colors differ from those Qt supports. See section 3.2.2.2
        setMathBackground( QColor( backgroundStr ) );
	}

    // Deprecated attributes. See Section 3.2.2.1

    sizeStr = element.attribute( "fontsize" );
    if ( ! sizeStr.isNull() ) {
        if ( sizeStr == "small" ) {
            setRelativeSize( 0.8, true ); // ### Arbitrary size
        }
        else if ( sizeStr == "normal" ) {
            setRelativeSize( 1.0, true );
        }
        else if ( sizeStr == "big" ) {
            setRelativeSize( 1.2, true ); // ### Arbitrary size
        }
        else {
            double s = getSize( sizeStr, &m_fontSizeType );
            switch ( m_fontSizeType ) {
            case AbsoluteSize:
                setAbsoluteSize( s, true );
                break;
            case RelativeSize:
                setRelativeSize( s, true );
                break;
            case PixelSize:
                setPixelSize( s, true );
                break;
            }
        }
    }

    QString styleStr = element.attribute( "fontstyle" );
    if ( ! styleStr.isNull() ) {
        if ( styleStr.lower() == "italic" ) {
            setFontStyle( true );
        }
        else {
            setFontStyle( false );
        }
    }

    QString weightStr = element.attribute( "fontweight" );
    if ( ! weightStr.isNull() ) {
        if ( weightStr.lower() == "bold" ) {
            setFontWeight( true );
        }
        else {
            setFontWeight( false );
        }
    }

    QString familyStr =  element.attribute( "fontfamily" );
    if ( ! familyStr.isNull() ) {
        setFontFamily( familyStr );
    }

    colorStr = element.attribute( "color" );
    if ( ! colorStr.isNull() ) {
        // TODO: Named colors differ from those Qt supports. See section 3.2.2.2
        setColor( QColor( colorStr  ) );
	}

    return true;
}

void StyleElement::writeMathML( QDomDocument& doc, QDomNode& parent, bool oasisFormat )
{
    QDomElement de = doc.createElement( oasisFormat ? "math:mstyle" : "mstyle" );
    writeMathMLAttributes( de );
    inherited::writeMathML( doc, de, oasisFormat );
    parent.appendChild( de );
}

void StyleElement::writeMathMLAttributes( QDomElement& element )
{
    // mathvariant attribute
    if ( customMathVariant() ) {
        if ( charFamily() == normalFamily ) {
            if ( charStyle() == normalChar ) {
                element.setAttribute( "mathvariant", "normal" );
            }
            else if ( charStyle() == boldChar ) {
                element.setAttribute( "mathvariant", "bold" );
            }
            else if ( charStyle() == italicChar ) {
                element.setAttribute( "mathvariant", "italic" );
            }
            else if ( charStyle() == boldItalicChar ) {
                element.setAttribute( "mathvariant", "bold-italic" );
            }
            else { // anyChar or unknown, it's always an error
                kdWarning( DEBUGID ) << "Mathvariant: unknown style for normal family\n";
            }
        }
        else if ( charFamily() == doubleStruckFamily ) {
            if ( charStyle() == normalChar ) {
                element.setAttribute( "mathvariant", "double-struck" );
            }
            else { // Shouldn't happen, it's a bug
                kdWarning( DEBUGID ) << "Mathvariant: unknown style for double-struck family\n";
            }
        }
        else if ( charFamily() == frakturFamily ) {
            if ( charStyle() == normalChar ) {
                element.setAttribute( "mathvariant", "fraktur" );
            }
            else if ( charStyle() == boldChar ) {
                element.setAttribute( "mathvariant", "bold-fraktur" );
            }
            else {
                kdWarning( DEBUGID ) << "Mathvariant: unknown style for fraktur family\n";
            }
        }
        else if ( charFamily() == scriptFamily ) {
            if ( charStyle() == normalChar ) {
                element.setAttribute( "mathvariant", "script" );
            }
            else if ( charStyle() == boldChar ) {
                element.setAttribute( "mathvariant", "bold-script" );
            }
            else { // Shouldn't happen, it's a bug
                kdWarning( DEBUGID ) << "Mathvariant: unknown style for script family\n";
            }
        }
        else if ( charFamily() == sansSerifFamily ) {
            if ( charStyle() == normalChar ) {
                element.setAttribute( "mathvariant", "sans-serif" );
            }
            else if ( charStyle() == boldChar ) {
                element.setAttribute( "mathvariant", "bold-sans-serif" );
            }
            else if ( charStyle() == italicChar ) {
                element.setAttribute( "mathvariant", "sans-serif-italic" );
            }
            else if ( charStyle() == boldItalicChar ) {
                element.setAttribute( "mathvariant", "sans-serif-bold-italic" );
            }
            else {
                kdWarning( DEBUGID ) << "Mathvariant: unknown style for sans serif family\n";
            }
        }
        else if ( charFamily() == monospaceFamily ) {
            if ( charStyle() == normalChar ) {
                element.setAttribute( "mathvariant", "monospace" );
            }
            else {
                kdWarning( DEBUGID ) << "Mathvariant: unknown style for monospace family\n";
            }
        }
        else {
            kdWarning( DEBUGID ) << "Mathvariant: unknown family\n";
        }
    }

    // mathsize attribute
    switch ( m_mathSizeType ) {
    case AbsoluteSize:
        element.setAttribute( "mathsize", QString( "%1pt" ).arg( m_mathSize ) );
        break;
    case RelativeSize:
        element.setAttribute( "mathsize", QString( "%1%" ).arg( m_mathSize * 100.0 ) );
        break;
    case PixelSize:
        element.setAttribute( "mathsize", QString( "%3px" ).arg( m_mathSize ) );
        break;
    }

    // mathcolor attribute
    if ( customMathColor() ) {
        element.setAttribute( "mathcolor", mathColor().name() );
    }
    
    // mathbackground attribute
    if ( customMathBackground() ) {
        element.setAttribute( "mathbackground", mathBackground().name() );
    }
}

void StyleElement::setAbsoluteSize( double s, bool fontsize )
{ 
        kdDebug( DEBUGID) << "Setting absolute size: " << s << endl;
        if ( fontsize ) {
            m_fontSizeType = AbsoluteSize;
            m_fontSize = s;
        }
        else {
            m_mathSizeType = AbsoluteSize;
            m_mathSize = s;
        }
}

void StyleElement::setRelativeSize( double f, bool fontsize )
{ 
        kdDebug( DEBUGID) << "Setting relative size: " << f << endl;
        if ( fontsize ) {
            m_fontSizeType = RelativeSize;
            m_fontSize = f;
        }
        else {
            m_mathSizeType = RelativeSize;
            m_mathSize = f;
        }
}

void StyleElement::setPixelSize( double px, bool fontsize )
{
        kdDebug( DEBUGID) << "Setting pixel size: " << px << endl;
        if ( fontsize ) {
            m_fontSizeType = PixelSize;
            m_fontSize = px;
        }
        else {
            m_mathSizeType = PixelSize;
            m_mathSize = px;
        }
}

void StyleElement::setStyleVariant( StyleAttributes& style )
{
    if ( customMathVariant() ) {
        style.setCharFamily ( charFamily() );
        style.setCustomMathVariant ( true );
        style.setCustomFontWeight( false );
        style.setCustomFont( false );
    }
    else {
        style.setCustomMathVariant( false );
        if ( customFontFamily() ) {
            style.setCustomFont( true );
            style.setFont( QFont(fontFamily()) );
        }

        bool fontweight = false;
        if ( customFontWeight() || style.customFontWeight() ) {
            style.setCustomFontWeight( true );
            if ( customFontWeight() ) {
                style.setFontWeight( fontWeight() );
                fontweight = fontWeight();
            }
            else {
                fontweight = style.customFontWeight();
            }
        }
        else {
            style.setCustomFontWeight( false );
        }

        bool fontstyle = false;
        if ( customFontStyle() ) {
            fontstyle = fontStyle();
        }

        if ( fontweight && fontstyle ) {
            style.setCharStyle( boldItalicChar );
        }
        else if ( fontweight && ! fontstyle ) {
            style.setCharStyle( boldChar );
        }
        else if ( ! fontweight && fontstyle ) {
            style.setCharStyle( italicChar );
        }
        else {
            style.setCharStyle( normalChar );
        }
    }
}

void StyleElement::setStyleColor( StyleAttributes& style )
{
    if ( customMathColor() ) {
        style.setColor( mathColor() );
    }
    else if ( customColor() ) {
        style.setColor( color() );
    }
    else {
        style.setColor( style.color() );
    }
}

void StyleElement::setStyleBackground( StyleAttributes& style )
{
    if ( customMathBackground() ) {
        style.setBackground( mathBackground() );
    }
    else {
        style.setBackground( style.background() );
    }
}

double StyleElement::sizeFactor( const ContextStyle& context, double factor )
{
    switch ( m_mathSizeType ) {
    case AbsoluteSize:
        return factor * m_mathSize / context.baseSize();
    case RelativeSize:
        return factor * m_mathSize;
    case PixelSize:
        // 3.2.2 says v-unit insteado of h-unit, that's why we use Y and not X
//        kdDebug( DEBUGID ) 
        return factor * context.pixelYToPt( m_mathSize ) / context.baseSize(); 
    case NoSize:
        switch ( m_fontSizeType ) {
        case AbsoluteSize:
            return factor * m_fontSize / context.baseSize();
        case RelativeSize:
            return factor * m_fontSize;
        case PixelSize:
            return factor * context.pixelYToPt( m_fontSize ) / context.baseSize();
        case NoSize:
            return factor;
        }
    }
    kdWarning( DEBUGID ) << k_funcinfo << " Unknown SizeType\n";
    return factor;
}

double StyleElement::str2size( const QString& str, SizeType *st, uint index, SizeType type )
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

double StyleElement::getSize( const QString& str, SizeType* st )
{
    int index = str.find( "%" );
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
    kdWarning( DEBUGID ) << "Unknown mathsize unit type\n";
    return -1;
}

KFORMULA_NAMESPACE_END
