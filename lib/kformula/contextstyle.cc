/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qfontmetrics.h>
#include <qstring.h>

#include <koGlobal.h>

#include "contextstyle.h"

KFORMULA_NAMESPACE_BEGIN


void ContextStyle::TextStyleValues::setup( QFont font,
                                           luPt baseSize,
                                           double reduction )
{
    reductionFactor = reduction;

    font.setPointSize( baseSize );
    QFontMetrics fm( font );

    // Or better the real space required? ( boundingRect )
    quad = fm.width( 'M' );
}


ContextStyle::ContextStyle()
    : defaultFont("times"), nameFont("times"), numberFont("times"),
      operatorFont("times"), symbolFont("symbol",12,QFont::Normal,FALSE),
      defaultColor(Qt::black), numberColor(Qt::blue),
      operatorColor(Qt::darkGreen), errorColor(Qt::darkRed),
      emptyColor(Qt::blue)
{
    defaultFont.setItalic(true);

    m_baseTextStyle = displayStyle;

    lineWidth = 1;
    emptyRectWidth = 10;
    emptyRectHeight = 10;
    baseSize = 18;

    linearMovement = false;

    centerSymbol = false;
    syntaxHighlighting = true;

    setup();
}

void ContextStyle::readConfig( KConfig* config )
{
    config->setGroup( "kformula Color" );
    defaultColor  = config->readColorEntry( "defaultColor",  &defaultColor );
    numberColor   = config->readColorEntry( "numberColor",   &numberColor );
    operatorColor = config->readColorEntry( "operatorColor", &operatorColor );
    emptyColor    = config->readColorEntry( "emptyColor",    &emptyColor );
    errorColor    = config->readColorEntry( "errorColor",    &errorColor );
}

bool ContextStyle::setZoom( double zoomX, double zoomY, bool, bool )
{
    bool changes = m_zoomedResolutionX != zoomX || m_zoomedResolutionY != zoomY;
    m_zoom = 100;
    m_zoomedResolutionX = zoomX;
    m_zoomedResolutionY = zoomY;
    return changes;
}

QColor ContextStyle::getNumberColor()   const
{
    if (syntaxHighlighting) {
        return numberColor;
    }
    return getDefaultColor();
}

QColor ContextStyle::getOperatorColor() const
{
    if (syntaxHighlighting) {
        return operatorColor;
    }
    return getDefaultColor();
}

QColor ContextStyle::getErrorColor()    const
{
    if (syntaxHighlighting) {
        return errorColor;
    }
    return getDefaultColor();
}

QColor ContextStyle::getEmptyColor()    const
{
    if (syntaxHighlighting) {
        return emptyColor;
    }
    return getDefaultColor();
}

void ContextStyle::setDefaultColor( const QColor& color )
{
    defaultColor = color;
}
void ContextStyle::setNumberColor( const QColor& color )
{
    numberColor = color;
}
void ContextStyle::setOperatorColor( const QColor& color )
{
    operatorColor = color;
}
void ContextStyle::setErrorColor( const QColor& color )
{
    errorColor = color;
}
void ContextStyle::setEmptyColor( const QColor& color )
{
    emptyColor = color;
}

double ContextStyle::getReductionFactor( TextStyle tstyle ) const
{
    return textStyleValues[ tstyle ].reductionFactor;
}

luPt ContextStyle::getAdjustedSize( TextStyle tstyle ) const
{
    return ptToLayoutUnitPt( static_cast<luPt>( baseSize*getReductionFactor( tstyle ) ) );
}

luPt ContextStyle::getSpace( TextStyle tstyle, SpaceWidths space ) const
{
    switch ( space ) {
    case THIN:   return getThinSpace( tstyle );
    case MEDIUM: return getMediumSpace( tstyle );
    case THICK:  return getThickSpace( tstyle );
    case QUAD:   return getQuadSpace( tstyle );
    }
    return 0;
}

luPt ContextStyle::getThinSpace( TextStyle tstyle ) const
{
    return textStyleValues[ tstyle ].thinSpace();
}

luPt ContextStyle::getMediumSpace( TextStyle tstyle ) const
{
    return textStyleValues[ tstyle ].mediumSpace();
}

luPt ContextStyle::getThickSpace( TextStyle tstyle ) const
{
    return textStyleValues[ tstyle ].thickSpace();
}

luPt ContextStyle::getQuadSpace( TextStyle tstyle ) const
{
    return textStyleValues[ tstyle ].quadSpace();
}


luPt ContextStyle::getBaseSize() const
{
    return static_cast<luPt>( ptToLayoutUnitPt( baseSize ) );
}

void ContextStyle::setBaseSize( pt size )
{
    pt newSize = size;
    if ( newSize != baseSize ) {
        baseSize = newSize;
        setup();
    }
}


luPixel ContextStyle::getLineWidth() const
{
    return ptToLayoutUnitPixX( lineWidth );
}

luPixel ContextStyle::getEmptyRectWidth() const
{
    return ptToLayoutUnitPixX( emptyRectWidth );
}

luPixel ContextStyle::getEmptyRectHeight() const
{
    return ptToLayoutUnitPixX( emptyRectHeight );
}


ContextStyle::TextStyle ContextStyle::convertTextStyleFraction( TextStyle tstyle ) const
{
    TextStyle result;

    switch ( tstyle ){
    case displayStyle:
	result = textStyle;
	break;
    case textStyle:
	result = scriptStyle;
	break;
    default:
	result = scriptScriptStyle;
	break;
    }

    return result;
}


ContextStyle::TextStyle ContextStyle::convertTextStyleIndex( TextStyle tstyle ) const
{
    TextStyle result;

    switch ( tstyle ){
    case displayStyle:
	result = scriptStyle;
	break;
    case textStyle:
	result = scriptStyle;
	break;
    default:
	result = scriptScriptStyle;
	break;
    }

    return result;
}


void ContextStyle::setup()
{
    //double size = getBaseSize();
    luPt size = static_cast<luPt>( ptToLayoutUnitPt( baseSize ) );
    textStyleValues[ displayStyle      ].setup( getSymbolFont(), size, 1. );
    textStyleValues[ textStyle         ].setup( getSymbolFont(), size, 1. );
    textStyleValues[ scriptStyle       ].setup( getSymbolFont(), size, .7 );
    textStyleValues[ scriptScriptStyle ].setup( getSymbolFont(), size, .49 );
}

KFORMULA_NAMESPACE_END
