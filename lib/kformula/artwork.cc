/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qbitmap.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qstring.h>

#include <kdebug.h>

#include "artwork.h"
#include "contextstyle.h"
#include "symboltable.h"


KFORMULA_NAMESPACE_BEGIN

const QChar spaceChar = 0x0020;
const QChar leftParenthesisChar = 0x0028;
const QChar rightParenthesisChar = 0x0029;
const QChar leftSquareBracketChar = 0x005B;
const QChar rightSquareBracketChar = 0x005D;
const QChar leftCurlyBracketChar = 0x007B;
const QChar verticalLineChar = 0x007C;
const QChar rightCurlyBracketChar = 0x007D;
const QChar leftAngleBracketChar = 0x2329;
const QChar rightAngleBracketChar = 0x232A;
const QChar integralChar = 0x222B;
const QChar summationChar = 0x2211;
const QChar productChar = 0x220F;

// We claim that all chars come from the same font.
// It's up to the font tables to ensure this.
const QChar leftRoundBracket[] = {
    0xF8EB, // uppercorner
    0xF8ED, // lowercorner
    0xF8EC  // line
};
const QChar leftSquareBracket[] = {
    0xF8EE, // uppercorner
    0xF8F0, // lowercorner
    0xF8EF  // line
};
const QChar leftCurlyBracket[] = {
    0xF8F1, // uppercorner
    0xF8F3, // lowercorner
    0xF8F4, // line
    0xF8F2  // middle
};

const QChar leftLineBracket[] = {
    0xF8EF, // line
    0xF8EF, // line
    0xF8EF  // line
};
const QChar rightLineBracket[] = {
    0xF8FA, // line
    0xF8FA, // line
    0xF8FA  // line
};

const QChar rightRoundBracket[] = {
    0xF8F6, // uppercorner
    0xF8F8, // lowercorner
    0xF8F7  // line
};
const QChar rightSquareBracket[] = {
    0xF8F9, // uppercorner
    0xF8FB, // lowercorner
    0xF8FA  // line
};
const QChar rightCurlyBracket[] = {
    0xF8FC, // uppercorner
    0xF8FE, // lowercorner
    0xF8F4, // line
    0xF8FD  // middle
};


const char esstixseven_LeftSquareBracket = 0x3f;
const char esstixseven_RightSquareBracket = 0x40;
const char esstixseven_LeftCurlyBracket = 0x41;
const char esstixseven_RightCurlyBracket = 0x42;
const char esstixseven_LeftCornerBracket = 0x43;
const char esstixseven_RightCornerBracket = 0x44;
const char esstixseven_LeftRoundBracket = 0x3d;
const char esstixseven_RightRoundBracket = 0x3e;
//const char esstixseven_SlashBracket = '/';
//const char esstixseven_BackSlashBracket = '\\';
const char esstixseven_LeftLineBracket = 0x4b;
const char esstixseven_RightLineBracket = 0x4b;


// esstixseven is a special font with symbols in three sizes.
static char esstixseven_nextchar( char ch )
{
    switch ( ch ) {
        // small
    case 61: return 33;
    case 62: return 35;
    case 63: return 36;
    case 64: return 37;
    case 65: return 38;
    case 66: return 40;
    case 67: return 41;
    case 68: return 42;
    case 69: return 43;
    case 70: return 44;
    case 75: return 45;
    case 76: return 47;

        // middle
    case 33: return 48;
    case 35: return 49;
    case 36: return 50;
    case 37: return 51;
    case 38: return 52;
    case 40: return 53;
    case 41: return 54;
    case 42: return 55;
    case 43: return 56;
    case 44: return 57;
    case 45: return 58;
    case 46: return 59;
    case 47: return 60;
    }
    return 0;
}


Artwork::Artwork(SymbolType t)
    : baseline( -1 ), esstixChar( -1 ), type(t)
{
}


inline bool doSimpleRoundBracket( luPt height, luPt baseHeight )
{
    return height < 1.5*baseHeight;
}

inline bool doSimpleSquareBracket( luPt height, luPt baseHeight )
{
    return height < 1.5*baseHeight;
}

inline bool doSimpleCurlyBracket( luPt height, luPt baseHeight )
{
    return height < 2*baseHeight;
}


void Artwork::calcSizes( const ContextStyle& style,
                         ContextStyle::TextStyle tstyle,
                         luPt parentSize )
{
    setBaseline( -1 );
    esstixChar = -1;
    luPt mySize = style.getAdjustedSize( tstyle );
    const SymbolTable& symbolTable = style.symbolTable();
    switch (type) {
    case LeftSquareBracket:
        if ( symbolTable.esstixDelimiter() ) {
            if ( calcEsstixDelimiterSize( style, esstixseven_LeftSquareBracket,
                                          mySize, parentSize ) ) {
                return;
            }
        }
        if ( doSimpleSquareBracket( parentSize, mySize ) ) {
            calcCharSize( style, mySize, leftSquareBracketChar );
            return;
        }
        calcRoundBracket( style, leftSquareBracket, parentSize, mySize );
        break;
    case RightSquareBracket:
        if ( symbolTable.esstixDelimiter() ) {
            if ( calcEsstixDelimiterSize( style, esstixseven_RightSquareBracket,
                                          mySize, parentSize ) ) {
                return;
            }
        }
        else if ( doSimpleSquareBracket( parentSize, mySize ) ) {
            calcCharSize(style, mySize, rightSquareBracketChar);
            return;
        }
        calcRoundBracket( style, rightSquareBracket, parentSize, mySize );
        break;
    case LeftLineBracket:
        if ( symbolTable.esstixDelimiter() ) {
            if ( calcEsstixDelimiterSize( style, esstixseven_LeftLineBracket,
                                          mySize, parentSize ) ) {
                return;
            }
        }
        else if ( doSimpleSquareBracket( parentSize, mySize ) ) {
            calcCharSize(style, mySize, verticalLineChar);
            return;
        }
        calcRoundBracket( style, leftLineBracket, parentSize, mySize );
        break;
    case RightLineBracket:
        if ( symbolTable.esstixDelimiter() ) {
            if ( calcEsstixDelimiterSize( style, esstixseven_RightLineBracket,
                                          mySize, parentSize ) ) {
                return;
            }
        }
        else if ( doSimpleSquareBracket( parentSize, mySize ) ) {
            calcCharSize(style, mySize, verticalLineChar);
            return;
        }
        calcRoundBracket( style, rightLineBracket, parentSize, mySize );
        break;
    case SlashBracket:
        //calcCharSize(style, mySize, '/');
        break;
    case BackSlashBracket:
        //calcCharSize(style, mySize, '\\');
        break;
    case LeftCornerBracket:
        if ( symbolTable.esstixDelimiter() ) {
            if ( calcEsstixDelimiterSize( style, esstixseven_LeftCornerBracket,
                                          mySize, parentSize ) ) {
                return;
            }
        }
        else calcCharSize(style, mySize, leftAngleBracketChar);
        break;
    case RightCornerBracket:
        if ( symbolTable.esstixDelimiter() ) {
            if ( calcEsstixDelimiterSize( style, esstixseven_RightCornerBracket,
                                          mySize, parentSize ) ) {
                return;
            }
        }
        else calcCharSize(style, mySize, rightAngleBracketChar);
        break;
    case LeftRoundBracket:
        if ( symbolTable.esstixDelimiter() ) {
            if ( calcEsstixDelimiterSize( style, esstixseven_LeftRoundBracket,
                                          mySize, parentSize ) ) {
                return;
            }
        }
        else if ( doSimpleRoundBracket( parentSize, mySize ) ) {
            calcCharSize(style, mySize, leftParenthesisChar);
            return;
        }
        calcRoundBracket( style, leftRoundBracket, parentSize, mySize );
        break;
    case RightRoundBracket:
        if ( symbolTable.esstixDelimiter() ) {
            if ( calcEsstixDelimiterSize( style, esstixseven_RightRoundBracket,
                                          mySize, parentSize ) ) {
                return;
            }
        }
        else if ( doSimpleRoundBracket( parentSize, mySize ) ) {
            calcCharSize(style, mySize, rightParenthesisChar);
            return;
        }
        calcRoundBracket( style, rightRoundBracket, parentSize, mySize );
        break;
    case EmptyBracket:
        setHeight(parentSize);
        //setWidth(style.getEmptyRectWidth());
        setWidth(0);
        break;
    case LeftCurlyBracket:
        if ( symbolTable.esstixDelimiter() ) {
            if ( calcEsstixDelimiterSize( style, esstixseven_LeftCurlyBracket,
                                          mySize, parentSize ) ) {
                return;
            }
        }
        else if ( doSimpleCurlyBracket( parentSize, mySize ) ) {
            calcCharSize(style, mySize, leftCurlyBracketChar);
            return;
        }
        calcCurlyBracket( style, leftCurlyBracket, parentSize, mySize );
        break;
    case RightCurlyBracket:
        if ( symbolTable.esstixDelimiter() ) {
            if ( calcEsstixDelimiterSize( style, esstixseven_RightCurlyBracket,
                                          mySize, parentSize ) ) {
                return;
            }
        }
        else if ( doSimpleCurlyBracket( parentSize, mySize ) ) {
            calcCharSize(style, mySize, rightCurlyBracketChar);
            return;
        }
        calcCurlyBracket( style, rightCurlyBracket, parentSize, mySize );
        break;
    case Integral:
        calcCharSize( style, qRound( 1.5*mySize ), integralChar );
        break;
    case Sum:
        calcCharSize( style, qRound( 1.5*mySize ), summationChar );
        break;
    case Product:
        calcCharSize( style, qRound( 1.5*mySize ), productChar );
        break;
    }
}


void Artwork::calcSizes( const ContextStyle& style,
                         ContextStyle::TextStyle tstyle )
{
    luPt mySize = style.getAdjustedSize( tstyle );
    switch (type) {
    case LeftSquareBracket:
        calcCharSize(style, mySize, leftSquareBracketChar);
        break;
    case RightSquareBracket:
        calcCharSize(style, mySize, rightSquareBracketChar);
        break;
    case LeftLineBracket:
    case RightLineBracket:
        calcCharSize(style, mySize, verticalLineChar);
        break;
    case SlashBracket:
        //calcCharSize(style, mySize, '/');
        break;
    case BackSlashBracket:
        //calcCharSize(style, mySize, '\\');
        break;
    case LeftCornerBracket:
        calcCharSize(style, mySize, leftAngleBracketChar);
        break;
    case RightCornerBracket:
        calcCharSize(style, mySize, rightAngleBracketChar);
        break;
    case LeftRoundBracket:
        calcCharSize(style, mySize, leftParenthesisChar);
        break;
    case RightRoundBracket:
        calcCharSize(style, mySize, rightParenthesisChar);
        break;
    case EmptyBracket:
        //calcCharSize(style, mySize, spaceChar);
        setHeight(0);
        //setWidth(style.getEmptyRectWidth());
        setWidth(0);
        break;
    case LeftCurlyBracket:
        calcCharSize(style, mySize, leftCurlyBracketChar);
        break;
    case RightCurlyBracket:
        calcCharSize(style, mySize, rightCurlyBracketChar);
        break;
    case Integral:
    case Sum:
    case Product:
        break;
    }
}


void Artwork::draw(QPainter& painter, const LuPixelRect& r,
                   const ContextStyle& style, ContextStyle::TextStyle tstyle,
                   luPt parentSize, const LuPixelPoint& origin)
{
    luPt mySize = style.getAdjustedSize( tstyle );
    luPixel myX = origin.x() + getX();
    luPixel myY = origin.y() + getY();
    if ( !LuPixelRect( myX, myY, getWidth(), getHeight() ).intersects( r ) )
        return;

    painter.setPen(style.getDefaultColor());
    const SymbolTable& symbolTable = style.symbolTable();

    switch (type) {
    case LeftSquareBracket:
        if ( symbolTable.esstixDelimiter() && ( esstixChar != -1 ) ) {
            drawEsstixDelimiter( painter, style, myX, myY, mySize );
        }
        else if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawBigRoundBracket( painter, style, leftSquareBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, leftSquareBracketChar);
        }
        break;
    case RightSquareBracket:
        if ( symbolTable.esstixDelimiter() && ( esstixChar != -1 ) ) {
            drawEsstixDelimiter( painter, style, myX, myY, mySize );
        }
        else if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawBigRoundBracket( painter, style, rightSquareBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, rightSquareBracketChar);
        }
        break;
    case LeftCurlyBracket:
        if ( symbolTable.esstixDelimiter() && ( esstixChar != -1 ) ) {
            drawEsstixDelimiter( painter, style, myX, myY, mySize );
        }
        else if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            drawBigCurlyBracket( painter, style, leftCurlyBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, leftCurlyBracketChar);
        }
        break;
    case RightCurlyBracket:
        if ( symbolTable.esstixDelimiter() && ( esstixChar != -1 ) ) {
            drawEsstixDelimiter( painter, style, myX, myY, mySize );
        }
        else if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            drawBigCurlyBracket( painter, style, rightCurlyBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, rightCurlyBracketChar);
        }
        break;
    case LeftLineBracket:
        if ( symbolTable.esstixDelimiter() && ( esstixChar != -1 ) ) {
            drawEsstixDelimiter( painter, style, myX, myY, mySize );
        }
        else if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawBigRoundBracket( painter, style, leftLineBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, verticalLineChar);
        }
        break;
    case RightLineBracket:
        if ( symbolTable.esstixDelimiter() && ( esstixChar != -1 ) ) {
            drawEsstixDelimiter( painter, style, myX, myY, mySize );
        }
        else if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawBigRoundBracket( painter, style, rightLineBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, verticalLineChar);
        }
        break;
    case SlashBracket:
        //drawCharacter(painter, style, myX, myY, mySize, '/');
        break;
    case BackSlashBracket:
        //drawCharacter(painter, style, myX, myY, mySize, '\\');
        break;
    case LeftCornerBracket:
        if ( symbolTable.esstixDelimiter() && ( esstixChar != -1 ) ) {
            drawEsstixDelimiter( painter, style, myX, myY, mySize );
        }
        else drawCharacter(painter, style, myX, myY, mySize, leftAngleBracketChar);
        break;
    case RightCornerBracket:
        if ( symbolTable.esstixDelimiter() && ( esstixChar != -1 ) ) {
            drawEsstixDelimiter( painter, style, myX, myY, mySize );
        }
        else drawCharacter(painter, style, myX, myY, mySize, rightAngleBracketChar);
        break;
    case LeftRoundBracket:
        if ( symbolTable.esstixDelimiter() && ( esstixChar != -1 ) ) {
            drawEsstixDelimiter( painter, style, myX, myY, mySize );
        }
        else if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            drawBigRoundBracket( painter, style, leftRoundBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, leftParenthesisChar);
        }
        break;
    case RightRoundBracket:
        if ( symbolTable.esstixDelimiter() && ( esstixChar != -1 ) ) {
            drawEsstixDelimiter( painter, style, myX, myY, mySize );
        }
        else if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            drawBigRoundBracket( painter, style, rightRoundBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, rightParenthesisChar);
        }
        break;
    case EmptyBracket:
        break;
    case Integral:
        drawCharacter(painter, style, myX, myY, qRound( 1.5*mySize ), integralChar);
        break;
    case Sum:
        drawCharacter(painter, style, myX, myY, qRound( 1.5*mySize ), summationChar);
        break;
    case Product:
        drawCharacter(painter, style, myX, myY, qRound( 1.5*mySize ), productChar);
        break;
    }

    // debug
    //painter.setBrush(Qt::NoBrush);
    //painter.setPen(Qt::green);
    //painter.drawRect(myX, myY, getWidth(), getHeight());
}


void Artwork::draw(QPainter& painter, const LuPixelRect& r,
                   const ContextStyle& style, ContextStyle::TextStyle tstyle,
                   const LuPixelPoint& parentOrigin)
{
    luPt mySize = style.getAdjustedSize( tstyle );
    luPixel myX = parentOrigin.x() + getX();
    luPixel myY = parentOrigin.y() + getY();
    if ( !LuPixelRect( myX, myY, getWidth(), getHeight() ).intersects( r ) )
        return;

    painter.setPen(style.getDefaultColor());

    switch (type) {
    case LeftSquareBracket:
        drawCharacter(painter, style, myX, myY, mySize, leftSquareBracketChar);
        break;
    case RightSquareBracket:
        drawCharacter(painter, style, myX, myY, mySize, rightSquareBracketChar);
        break;
    case LeftCurlyBracket:
        drawCharacter(painter, style, myX, myY, mySize, leftCurlyBracketChar);
        break;
    case RightCurlyBracket:
        drawCharacter(painter, style, myX, myY, mySize, rightCurlyBracketChar);
        break;
    case LeftLineBracket:
    case RightLineBracket:
        drawCharacter(painter, style, myX, myY, mySize, verticalLineChar);
        break;
    case SlashBracket:
        //drawCharacter(painter, style, myX, myY, mySize, '/');
        break;
    case BackSlashBracket:
        //drawCharacter(painter, style, myX, myY, mySize, '\\');
        break;
    case LeftCornerBracket:
        drawCharacter(painter, style, myX, myY, mySize, leftAngleBracketChar);
        break;
    case RightCornerBracket:
        drawCharacter(painter, style, myX, myY, mySize, rightAngleBracketChar);
        break;
    case LeftRoundBracket:
        drawCharacter(painter, style, myX, myY, mySize, leftParenthesisChar);
        break;
    case RightRoundBracket:
        drawCharacter(painter, style, myX, myY, mySize, rightParenthesisChar);
        break;
    case EmptyBracket:
        break;
    case Integral:
    case Sum:
    case Product:
        break;
    }
}


void Artwork::calcCharSize( const ContextStyle& style, luPt height, QChar ch )
{
    //QFont f = style.getSymbolFont();
    uchar c = style.symbolTable().character( ch );
    QFont f = style.symbolTable().font( ch );
    f.setPointSizeFloat( style.layoutUnitToFontSize( height, false ) );
    //f.setPointSize( height );

    QFontMetrics fm(f);
    setWidth( style.ptToLayoutUnitPt( fm.width( c ) ) );
    LuPixelRect bound = fm.boundingRect( c );
    setHeight( style.ptToLayoutUnitPt( bound.height() ) );
    setBaseline( style.ptToLayoutUnitPt( -bound.top() ) );
}


void Artwork::drawCharacter( QPainter& painter, const ContextStyle& style, luPixel x, luPixel y,
                             luPt height, QChar ch )
{
    uchar c = style.symbolTable().character( ch );
    QFont f = style.symbolTable().font( ch );
    f.setPointSizeFloat( style.layoutUnitToFontSize( height, false ) );

    painter.setFont( f );
    painter.drawText( style.layoutUnitToPixelX( x ),
                      style.layoutUnitToPixelY( y+getBaseline() ),
                      QString( QChar( c ) ) );
}


bool Artwork::calcEsstixDelimiterSize( const ContextStyle& context,
                                       char c,
                                       luPt fontSize,
                                       luPt parentSize )
{
    QFont f( "esstixseven" );

    for ( char i=1; c != 0; ++i ) {
        f.setPointSizeFloat( context.layoutUnitToFontSize( i*fontSize, false ) );
        QFontMetrics fm( f );
        LuPixelRect bound = fm.boundingRect( c );

        luPt height = context.ptToLayoutUnitPt( bound.height() );
        if ( height >= parentSize ) {
            luPt width = context.ptToLayoutUnitPt( fm.width( c ) );
            luPt baseline = context.ptToLayoutUnitPt( -bound.top() );

            setHeight( height );
            setWidth( width );
            setBaseline( baseline );
            esstixChar = c;
            fontSizeFactor = i;
            return true;
        }
        c = esstixseven_nextchar( c );
    }

    // Build it up from pieces.
    return false;
}


void Artwork::drawEsstixDelimiter( QPainter& painter, const ContextStyle& style,
                                   luPixel x, luPixel y,
                                   luPt height )
{
    QFont f( "esstixseven" );
    f.setPointSizeFloat( style.layoutUnitToFontSize( fontSizeFactor*height, false ) );

    painter.setFont( f );
    painter.drawText( style.layoutUnitToPixelX( x ),
                      style.layoutUnitToPixelY( y + getBaseline() ),
                      QString( QChar( esstixChar ) ) );
}


void Artwork::calcRoundBracket( const ContextStyle& style, const QChar chars[],
                                luPt height, luPt charHeight )
{
    uchar uppercorner = style.symbolTable().character( chars[0] );
    uchar lowercorner = style.symbolTable().character( chars[1] );
    //uchar line = style.symbolTable().character( chars[2] );

    QFont f = style.symbolTable().font( chars[0] );
    f.setPointSizeFloat( style.layoutUnitPtToPt( charHeight ) );
    QFontMetrics fm( f );
    LuPtRect upperBound = fm.boundingRect( uppercorner );
    LuPtRect lowerBound = fm.boundingRect( lowercorner );
    //LuPtRect lineBound = fm.boundingRect( line );

    setWidth( style.ptToLayoutUnitPt( fm.width( QChar( uppercorner ) ) ) );
    luPt edgeHeight = style.ptToLayoutUnitPt( upperBound.height()+lowerBound.height() );
    //luPt lineHeight = style.ptToLayoutUnitPt( lineBound.height() );

    //setHeight( edgeHeight + ( ( height-edgeHeight-1 ) / lineHeight + 1 ) * lineHeight );
    setHeight( QMAX( edgeHeight, height ) );
}

void Artwork::drawBigRoundBracket( QPainter& p, const ContextStyle& style, const QChar chars[],
                                   luPixel x, luPixel y, luPt charHeight )
{
    uchar uppercorner = style.symbolTable().character( chars[0] );
    uchar lowercorner = style.symbolTable().character( chars[1] );
    uchar line = style.symbolTable().character( chars[2] );

    QFont f = style.symbolTable().font( chars[0] );
    f.setPointSizeFloat( style.layoutUnitToFontSize( charHeight, false ) );
    p.setFont(f);

    QFontMetrics fm(f);
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);
    QRect lineBound = fm.boundingRect(line);

    pixel ptX = style.layoutUnitToPixelX( x );
    pixel ptY = style.layoutUnitToPixelY( y );
    pixel height = style.layoutUnitToPixelY( getHeight() );

//     p.setPen( Qt::red );
//     //p.drawRect( ptX, ptY, upperBound.width(), upperBound.height() + lowerBound.height() );
//     p.drawRect( ptX, ptY, style.layoutUnitToPixelX( getWidth() ),
//                 style.layoutUnitToPixelY( getHeight() ) );

//     p.setPen( Qt::black );
    p.drawText( ptX, ptY-upperBound.top(), QString( QChar( uppercorner ) ) );
    p.drawText( ptX, ptY+height-lowerBound.top()-lowerBound.height(),
                QString( QChar( lowercorner ) ) );

    // for printing
    //pt safety = lineBound.height() / 10.0;
    pixel safety = 0;

    pixel gap = height - upperBound.height() - lowerBound.height();
    pixel lineHeight = lineBound.height() - safety;
    int lineCount = qRound( static_cast<double>( gap ) / lineHeight );
    pixel start = upperBound.height()-lineBound.top() - safety;

    for (int i = 0; i < lineCount; i++) {
        p.drawText( ptX, ptY+start+i*lineHeight, QString(QChar(line)));
    }
    pixel remaining = gap - lineCount*lineHeight;
    pixel dist = ( lineHeight - remaining ) / 2;
    p.drawText( ptX, ptY+height-upperBound.height()+dist-lineBound.height()-lineBound.top(),
                QString( QChar( line ) ) );
}

void Artwork::calcCurlyBracket( const ContextStyle& style, const QChar chars[],
                                luPt height, luPt charHeight )
{
    uchar uppercorner = style.symbolTable().character( chars[0] );
    uchar lowercorner = style.symbolTable().character( chars[1] );
    //uchar line = style.symbolTable().character( chars[2] );
    uchar middle = style.symbolTable().character( chars[3] );

    QFont f = style.symbolTable().font( chars[0] );
    f.setPointSizeFloat( style.layoutUnitPtToPt( charHeight ) );
    QFontMetrics fm( f );
    LuPtRect upperBound = fm.boundingRect( uppercorner );
    LuPtRect lowerBound = fm.boundingRect( lowercorner );
    //LuPtRect lineBound = fm.boundingRect( line );
    LuPtRect middleBound = fm.boundingRect( middle );

    setWidth( style.ptToLayoutUnitPt( fm.width( QChar( uppercorner ) ) ) );
    luPt edgeHeight = style.ptToLayoutUnitPt( upperBound.height()+
                                              lowerBound.height()+
                                              middleBound.height() );
    //luPt lineHeight = style.ptToLayoutUnitPt( lineBound.height() );

    //setHeight( edgeHeight + ( ( height-edgeHeight-1 ) / lineHeight + 1 ) * lineHeight );
    setHeight( QMAX( edgeHeight, height ) );
}

void Artwork::drawBigCurlyBracket( QPainter& p, const ContextStyle& style, const QChar chars[],
                                   luPixel x, luPixel y, luPt charHeight )
{
    //QFont f = style.getSymbolFont();
    QFont f = style.symbolTable().font( chars[0] );
    f.setPointSizeFloat( style.layoutUnitToFontSize( charHeight, false ) );
    p.setFont(f);

    uchar uppercorner = style.symbolTable().character( chars[0] );
    uchar lowercorner = style.symbolTable().character( chars[1] );
    uchar line = style.symbolTable().character( chars[2] );
    uchar middle = style.symbolTable().character( chars[3] );

    QFontMetrics fm(p.fontMetrics());
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);
    QRect middleBound = fm.boundingRect(middle);
    QRect lineBound = fm.boundingRect(line);

    pixel ptX = style.layoutUnitToPixelX( x );
    pixel ptY = style.layoutUnitToPixelY( y );
    pixel height = style.layoutUnitToPixelY( getHeight() );

    //p.setPen(Qt::gray);
    //p.drawRect(x, y, upperBound.width() + offset, height);

    p.drawText( ptX, ptY-upperBound.top(), QString( QChar( uppercorner ) ) );
    p.drawText( ptX, ptY+(height-middleBound.height())/2-middleBound.top(),
                QString( QChar( middle ) ) );
    p.drawText( ptX, ptY+height-lowerBound.top()-lowerBound.height(),
                QString( QChar( lowercorner ) ) );

    // for printing
    // If the world was perfect and the urw-symbol font correct
    // this could be 0.
    //lu safety = lineBound.height() / 10;
    pixel safety = 0;

    pixel lineHeight = lineBound.height() - safety;
    pixel gap = height/2 - upperBound.height() - middleBound.height() / 2;

    if (gap > 0) {
        QString ch = QString(QChar(line));
        int lineCount = qRound( gap / lineHeight ) + 1;

        pixel start = (height - middleBound.height()) / 2 + safety;
        for (int i = 0; i < lineCount; i++) {
            p.drawText( ptX, ptY-lineBound.top()+QMAX( start-(i+1)*lineHeight,
                                                       upperBound.width() ),
                        ch );
        }

        start = (height + middleBound.height()) / 2 - safety;
        for (int i = 0; i < lineCount; i++) {
            p.drawText( ptX, ptY-lineBound.top()+QMIN( start+i*lineHeight,
                                                       height-upperBound.width()-lineBound.height() ),
                        ch );
        }
    }
}

KFORMULA_NAMESPACE_END
