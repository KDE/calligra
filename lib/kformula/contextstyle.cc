/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

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

#include <qfontmetrics.h>
#include <qstring.h>

#include <kdebug.h>
#include <KoGlobal.h>

#include "contextstyle.h"
#include "fontstyle.h"


KFORMULA_NAMESPACE_BEGIN


ContextStyle::ContextStyle()
    : symbolFont( "Symbol" ),
      defaultColor(Qt::black), numberColor(Qt::blue),
      operatorColor(Qt::darkGreen), errorColor(Qt::darkRed),
      emptyColor(Qt::blue), helpColor( Qt::gray ), m_sizeFactor( 0 )
{
//     kdDebug() << "ContextStyle::ContextStyle" << endl
//               << "defaultFont: " << defaultFont.rawName() << endl
//               << "nameFont: " << nameFont.rawName() << endl
//               << "numberFont: " << numberFont.rawName() << endl
//               << "operatorFont: " << operatorFont.rawName() << endl
//               << "symbolFont: " << symbolFont.rawName() << endl;

    textStyleValues[ displayStyle      ].setup( 1. );
    textStyleValues[ textStyle         ].setup( 1. );
    textStyleValues[ scriptStyle       ].setup( .7 );
    textStyleValues[ scriptScriptStyle ].setup( .49 );

    m_baseTextStyle = displayStyle;

    lineWidth = 1;
    linearMovement = false;
    centerSymbol = true;
    m_syntaxHighlighting = true;

    m_fontStyle = 0;
}


ContextStyle::~ContextStyle()
{
    delete m_fontStyle;
}


void ContextStyle::init( bool init )
{
    setup();
    setFontStyle( m_fontStyleName, init );
}


void ContextStyle::setFontStyle( const QString& fontStyle, bool init )
{
    delete m_fontStyle;
    m_fontStyleName = fontStyle;
    m_fontStyle = new FontStyle();
    m_fontStyle->init( this, init );
}


const SymbolTable& ContextStyle::symbolTable() const
{
    return *( m_fontStyle->symbolTable() );
}


void ContextStyle::readConfig( KConfig* config, bool init )
{
    config->setGroup( "kformula Font" );
    QString fontName = config->readEntry( "defaultFont", "Times,12,-1,5,50,1,0,0,0,0" );
    defaultFont.fromString( fontName );
    fontName = config->readEntry( "nameFont", "Times,12,-1,5,50,0,0,0,0,0" );
    nameFont.fromString( fontName );
    fontName = config->readEntry( "numberFont", "Times,12,-1,5,50,0,0,0,0,0" );
    numberFont.fromString( fontName );
    fontName = config->readEntry( "operatorFont", "Times,12,-1,5,50,0,0,0,0,0" );
    operatorFont.fromString( fontName );
    QString baseSize = config->readEntry( "baseSize", "20" );
    m_baseSize = baseSize.toInt();

    if ( ! FontStyle::missingFonts( init ).isEmpty() ) {
        kdWarning( DEBUGID) << "Not all basic fonts found\n";
    }
    mathFont.fromString("Arev Sans");
    bracketFont.fromString("cmex10");


    // There's no gui right anymore but I'll leave it here...
    config->setGroup( "kformula Color" );
    defaultColor  = config->readColorEntry( "defaultColor",  &defaultColor );
    numberColor   = config->readColorEntry( "numberColor",   &numberColor );
    operatorColor = config->readColorEntry( "operatorColor", &operatorColor );
    emptyColor    = config->readColorEntry( "emptyColor",    &emptyColor );
    errorColor    = config->readColorEntry( "errorColor",    &errorColor );
    helpColor     = config->readColorEntry( "helpColor",     &helpColor );

    m_syntaxHighlighting = config->readBoolEntry( "syntaxHighlighting", true );
}

void ContextStyle::setZoomAndResolution( int zoom, int dpiX, int dpiY )
{
    KoZoomHandler::setZoomAndResolution( zoom, dpiX, dpiY );
}

bool ContextStyle::setZoomAndResolution( int zoom, double zoomX, double zoomY, bool, bool )
{
    bool changes = m_zoom != zoom || m_zoomedResolutionX != zoomX || m_zoomedResolutionY != zoomY;
    m_zoom = zoom;
    m_zoomedResolutionX = zoomX;
    m_zoomedResolutionY = zoomY;
    return changes;
}

QColor ContextStyle::getNumberColor()   const
{
    if ( edit() && syntaxHighlighting() ) {
        return numberColor;
    }
    return getDefaultColor();
}

QColor ContextStyle::getOperatorColor() const
{
    if ( edit() && syntaxHighlighting() ) {
        return operatorColor;
    }
    return getDefaultColor();
}

QColor ContextStyle::getErrorColor()    const
{
    if ( edit() && syntaxHighlighting() ) {
        return errorColor;
    }
    return getDefaultColor();
}

QColor ContextStyle::getEmptyColor()    const
{
    if ( edit() && syntaxHighlighting() ) {
        return emptyColor;
    }
    return getDefaultColor();
}

QColor ContextStyle::getHelpColor()     const
{
    if ( edit() && syntaxHighlighting() ) {
        return helpColor;
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
void ContextStyle::setHelpColor( const QColor& color )
{
    helpColor = color;
}

#if 0
const QStringList& ContextStyle::requestedFonts() const
{
    return m_requestedFonts;
}

void ContextStyle::setRequestedFonts( const QStringList& list )
{
    m_requestedFonts = list;
    //table.init( this );
}
#endif

double ContextStyle::getReductionFactor( TextStyle tstyle ) const
{
    return textStyleValues[ tstyle ].reductionFactor;
}

luPt ContextStyle::getAdjustedSize( TextStyle tstyle, double factor ) const
{
    return qRound( ptToLayoutUnitPt( m_sizeFactor 
                                     * m_baseSize 
                                     * getReductionFactor( tstyle )
                                     * factor ) );
}

luPixel ContextStyle::getSpace( TextStyle tstyle, SpaceWidth space, double factor ) const
{
    switch ( space ) {
    case NEGTHIN: return -getThinSpace( tstyle, factor );
    case THIN:    return getThinSpace( tstyle, factor );
    case MEDIUM:  return getMediumSpace( tstyle, factor );
    case THICK:   return getThickSpace( tstyle, factor );
    case QUAD:    return getQuadSpace( tstyle, factor );
    }
    return 0;
}

luPixel ContextStyle::getThinSpace( TextStyle tstyle, double factor ) const
{
    return ptToPixelX( m_sizeFactor
                       * textStyleValues[ tstyle ].thinSpace( quad )
                       * factor );
}

luPixel ContextStyle::getMediumSpace( TextStyle tstyle, double factor ) const
{
    return ptToPixelX( m_sizeFactor
                       * textStyleValues[ tstyle ].mediumSpace( quad ) 
                       * factor );
}

luPixel ContextStyle::getThickSpace( TextStyle tstyle, double factor ) const
{
    return ptToPixelX( m_sizeFactor
                       * textStyleValues[ tstyle ].thickSpace( quad ) 
                       * factor );
}

luPixel ContextStyle::getQuadSpace( TextStyle tstyle, double factor ) const
{
    return ptToPixelX( m_sizeFactor
                       * textStyleValues[ tstyle ].quadSpace( quad ) 
                       * factor );
}

luPixel ContextStyle::axisHeight( TextStyle tstyle, double factor ) const
{
    //return ptToPixelY( textStyleValues[ tstyle ].axisHeight( m_axisHeight ) );
    return static_cast<luPixel>( m_sizeFactor
                                 * textStyleValues[ tstyle ].axisHeight( m_axisHeight ) 
                                 * factor );
}

luPt ContextStyle::getBaseSize() const
{
    return static_cast<luPt>( ptToLayoutUnitPt( m_sizeFactor*m_baseSize ) );
}

void ContextStyle::setBaseSize( int size )
{
    //kdDebug( 40000 ) << "ContextStyle::setBaseSize" << endl;
    if ( size != m_baseSize ) {
        m_baseSize = size;
        setup();
    }
}

void ContextStyle::setSizeFactor( double factor )
{
    m_sizeFactor = factor;
}


luPixel ContextStyle::getLineWidth( double factor ) const
{
    return ptToLayoutUnitPixX( m_sizeFactor*lineWidth*factor );
}

luPixel ContextStyle::getEmptyRectWidth( double factor ) const
{
    return ptToLayoutUnitPixX( m_sizeFactor*m_baseSize*factor/1.8 );
}

luPixel ContextStyle::getEmptyRectHeight( double factor ) const
{
    return ptToLayoutUnitPixX( m_sizeFactor*m_baseSize*factor/1.8 );
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
    luPt size = static_cast<luPt>( m_baseSize );
    QFont font = symbolFont;
    font.setPointSize( size );
    QFontMetrics fm( font );

    // Or better the real space required? ( boundingRect )
    quad = ptToLayoutUnitPt( fm.width( 'M' ) );

    font = QFont(defaultFont);
    font.setPointSize( size );
    QFontMetrics fm2( font );
    //m_axisHeight = ptToLayoutUnitPt( fm2.strikeOutPos() );
    //ptToLayoutUnitPixY
    //m_axisHeight = ptToLayoutUnitPt( pixelYToPt( fm2.strikeOutPos() ) );
    m_axisHeight = ptToLayoutUnitPixY( pixelYToPt( fm2.strikeOutPos() ) );
}


double StyleAttributes::sizeFactor() const
{
    if ( m_size.empty() ) {
//        kdWarning( DEBUGID ) << "SizeFactor stack is empty.\n";
        return 1.0;
    }
    return m_size.top();
}

bool StyleAttributes::customMathVariant() const
{
    if ( m_customMathVariant.empty() ) {
        return false;
    }
    return m_customMathVariant.top();
}

CharStyle StyleAttributes::charStyle() const
{
    if ( m_charStyle.empty() ) {
//        kdWarning( DEBUGID ) << "CharStyle stack is empty.\n";
        return anyChar;
    }
    return m_charStyle.top();
}

CharFamily StyleAttributes::charFamily() const
{
    if ( m_charFamily.empty() ) {
//        kdWarning( DEBUGID ) << "CharFamily stack is empty.\n";
        return anyFamily;
    }
    return m_charFamily.top();
}

QColor StyleAttributes::color() const
{
    if ( m_color.empty() ) {
//        kdWarning( DEBUGID ) << "Color stack is empty.\n";
        return QColor( Qt::black );
        //return getDefaultColor();
    }
    return m_color.top();
}

QColor StyleAttributes::background() const
{
    if ( m_background.empty() ) {
//        kdWarning( DEBUGID ) << "Background stack is empty.\n";
        return QColor( Qt::white );
    }
    return m_background.top();
}

QFont StyleAttributes::font() const
{
    if ( m_font.empty() ) {
        return QFont();
    }
    return m_font.top();
}

bool StyleAttributes::fontWeight() const
{
    if ( m_fontWeight.empty() ) {
        return false;
    }
    return m_fontWeight.top();
}

bool StyleAttributes::customFontWeight() const
{
    if ( m_customFontWeight.empty() ) {
        return false;
    }
    return m_customFontWeight.top();
}

bool StyleAttributes::fontStyle() const
{
    if ( m_fontStyle.empty() ) {
        return false;
    }
    return m_fontStyle.top();
}

bool StyleAttributes::customFontStyle() const
{
    if ( m_customFontStyle.empty() ) {
        return false;
    }
    return m_customFontStyle.top();
}

bool StyleAttributes::customFont() const
{
    if ( m_customFontFamily.empty() ) {
        return false;
    }
    return m_customFontFamily.top();
}

void StyleAttributes::reset()
{
    if ( ! m_size.empty() ) {
        m_size.pop();
    }
    if ( ! m_charStyle.empty() ) {
        m_charStyle.pop();
    }
    if ( ! m_charFamily.empty() ) {
        m_charFamily.pop();
    }
    if ( ! m_color.empty() ) {
        m_color.pop();
    }
    if ( ! m_background.empty() ) {
        m_background.pop();
    }
    if ( ! m_customFontFamily.empty() ) {
        if ( m_customFontFamily.pop() ) {
            if ( ! m_font.empty() ) {
                m_font.pop();
            }
        }
    }
    if ( ! m_customFontWeight.empty() ) {
        if ( m_customFontWeight.pop() ) {
            if ( ! m_fontWeight.empty() ) {
                m_fontWeight.pop();
            }
        }
    }
    if ( ! m_customFontStyle.empty() ) {
        if ( m_customFontStyle.pop() ) {
            if ( ! m_fontStyle.empty() ) {
                m_fontStyle.pop();
            }
        }
    }
}



KFORMULA_NAMESPACE_END
