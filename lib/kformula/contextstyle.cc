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
                                           double baseSize,
                                           double reduction )
{
    reductionFactor = reduction;

    font.setPointSizeFloat( baseSize );
    QFontMetrics fm( font );

    // Or better the real space required? ( boundingRect )
    quad = fm.width( 'M' );
}


ContextStyle::ContextStyle()
        : m_zoomedResolutionX(1.0), m_zoomedResolutionY(1.0),
          defaultFont("times"), nameFont("times"), numberFont("times"),
          operatorFont("times"), symbolFont("symbol",12,QFont::Normal,FALSE,QFont::AnyCharSet),
          defaultColor(Qt::black), numberColor(Qt::blue),
          operatorColor(Qt::darkGreen), errorColor(Qt::darkRed),
          emptyColor(Qt::blue)
{
    defaultFont.setItalic(true);

    m_baseTextStyle = displayStyle;

    lineWidth = 1;
    emptyRectWidth = 10;
    emptyRectHeight = 10;
    //distance = 4;
    //operatorSpace = 4;
    baseSize = 18;

    linearMovement = false;

    centerSymbol = false;
    syntaxHighlighting = true;

    setup();
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



void ContextStyle::setResolution(double zX, double zY)
{
    m_zoomedResolutionX = zX;
    m_zoomedResolutionY = zY;
}

double ContextStyle::getReductionFactor( TextStyle tstyle ) const
{
    return textStyleValues[ tstyle ].reductionFactor;
}

double ContextStyle::getAdjustedSize( TextStyle tstyle ) const
{
    return zoomItY( baseSize*getReductionFactor( tstyle ) );
}

double ContextStyle::getThinSpace( TextStyle tstyle ) const
{
    return zoomItX( textStyleValues[ tstyle ].thinSpace() );
}

double ContextStyle::getMediumSpace( TextStyle tstyle ) const
{
    return zoomItX( textStyleValues[ tstyle ].mediumSpace() );
}

double ContextStyle::getThickSpace( TextStyle tstyle ) const
{
    return zoomItX( textStyleValues[ tstyle ].thickSpace() );
}

// double ContextStyle::getDistanceX(TextStyle tstyle) const
// {
//     return zoomItX( distance*getReductionFactor( tstyle ))+.5;
// }

// double ContextStyle::getDistanceY(TextStyle tstyle) const
// {
//     return zoomItY( distance*getReductionFactor( tstyle ))+.5;
// }

double ContextStyle::getBaseSize() const
{
    return zoomItY( baseSize );
}

double ContextStyle::getLineWidth() const
{
    return zoomItY( lineWidth );
}

double ContextStyle::getEmptyRectWidth() const
{
    return zoomItY( emptyRectWidth );
}

double ContextStyle::getEmptyRectHeight() const
{
    return zoomItY( emptyRectHeight );
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
    textStyleValues[ displayStyle      ].setup( getSymbolFont(), baseSize, 1. );
    textStyleValues[ textStyle         ].setup( getSymbolFont(), baseSize, 1. );
    textStyleValues[ scriptStyle       ].setup( getSymbolFont(), baseSize, .7 );
    textStyleValues[ scriptScriptStyle ].setup( getSymbolFont(), baseSize, .49 );
}

KFORMULA_NAMESPACE_END
