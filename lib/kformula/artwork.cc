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

#include <iostream.h>

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


Artwork::Artwork(SymbolType t)
    : type(t)
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
    luPt mySize = style.getAdjustedSize( tstyle );
    switch (type) {
    case LeftSquareBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            calcSquareBracket( style, leftSquareBracket, parentSize, mySize );
        }
        else {
            calcCharSize( style, mySize, leftSquareBracketChar );
        }
        break;
    case RightSquareBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            calcSquareBracket( style, rightSquareBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, rightSquareBracketChar);
        }
        break;
    case LeftLineBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            calcSquareBracket( style, leftLineBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, verticalLineChar);
        }
        break;
    case RightLineBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            calcSquareBracket( style, rightLineBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, verticalLineChar);
        }
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
        if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            calcRoundBracket( style, leftRoundBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, leftParenthesisChar);
        }
        break;
    case RightRoundBracket:
        if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            calcRoundBracket( style, rightRoundBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, rightParenthesisChar);
        }
        break;
    case EmptyBracket:
        setHeight(parentSize);
        //setWidth(style.getEmptyRectWidth());
        setWidth(0);
        break;
    case LeftCurlyBracket:
        if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            calcCurlyBracket( style, leftCurlyBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, leftCurlyBracketChar);
        }
        break;
    case RightCurlyBracket:
        if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            calcCurlyBracket( style, rightCurlyBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, rightCurlyBracketChar);
        }
        break;
    case Integral:
        calcCharSize( style, 1.5*mySize, integralChar );
        break;
    case Sum:
        calcCharSize( style, 1.5*mySize, summationChar );
        break;
    case Product:
        calcCharSize( style, 1.5*mySize, productChar );
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

    switch (type) {
    case LeftSquareBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawLeftSquareBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, leftSquareBracketChar);
        }
        break;
    case RightSquareBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawRightSquareBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, rightSquareBracketChar);
        }
        break;
    case LeftCurlyBracket:
        if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            drawLeftCurlyBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, leftCurlyBracketChar);
        }
        break;
    case RightCurlyBracket:
        if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            drawRightCurlyBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, rightCurlyBracketChar);
        }
        break;
    case LeftLineBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawLeftLineBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, verticalLineChar);
        }
        break;
    case RightLineBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawRightLineBracket( painter, style, myX, myY, parentSize, mySize );
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
        drawCharacter(painter, style, myX, myY, mySize, leftAngleBracketChar);
        break;
    case RightCornerBracket:
        drawCharacter(painter, style, myX, myY, mySize, rightAngleBracketChar);
        break;
    case LeftRoundBracket:
        if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            drawLeftRoundBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, leftParenthesisChar);
        }
        break;
    case RightRoundBracket:
        if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            drawRightRoundBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, rightParenthesisChar);
        }
        break;
    case EmptyBracket:
        break;
    case Integral:
        drawCharacter(painter, style, myX, myY, 1.5*mySize, integralChar);
        break;
    case Sum:
        drawCharacter(painter, style, myX, myY, 1.5*mySize, summationChar);
        break;
    case Product:
        drawCharacter(painter, style, myX, myY, 1.5*mySize, productChar);
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
    f.setPointSize( height );

    QFontMetrics fm(f);
    setWidth( fm.width( c ) );
    LuPixelRect bound = fm.boundingRect( c );
    setHeight( bound.height() );
    setBaseline( -bound.top() );
}


void Artwork::drawCharacter( QPainter& painter, const ContextStyle& style, luPixel x, luPixel y,
                             luPt height, QChar ch )
{
    //QFont f = style.getSymbolFont();
    uchar c = style.symbolTable().character( ch );
    QFont f = style.symbolTable().font( ch );
    f.setPointSizeFloat( style.layoutUnitToFontSize( height, false ) );

    painter.setFont( f );
    painter.drawText( style.layoutUnitToPixelX( x ),
                      style.layoutUnitToPixelY( y+getBaseline() ),
                      QString( QChar( c ) ) );

    //painter.drawRect(bound);
    //painter.drawRect(x, y, getWidth(), getHeight());
    //cerr << bound.x() << " " << bound.y() << " " << bound.width() << " " << bound.height() << endl;
}


void Artwork::calcRoundBracket( const ContextStyle& style, const QChar chars[],
                                luPt height, luPt charHeight )
{
    //char uppercorner = chars[0];
    //char lowercorner = chars[1];
    uchar line = style.symbolTable().character( chars[2] );

    QFont f = style.symbolTable().font( chars[0] );
    //QFont f = style.getSymbolFont();
    f.setPointSize( charHeight );
    QFontMetrics fm( f );
    //LuRect upperBound = fm.boundingRect(uppercorner);
    //LuRect lowerBound = fm.boundingRect(lowercorner);

    setWidth( fm.width( QChar( line ) ) );
    setHeight( ( ( height-1 ) / charHeight + 1 ) * charHeight );
}

void Artwork::drawLeftRoundBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y,
                                    luPt height, luPt charHeight )
{
    if ( height <= 2*charHeight ) {
        drawSmallRoundBracket( p, style, leftRoundBracket, x, y, charHeight );
	//cout << "drawLeftSmallRoundBracket" << endl;
    }
    else {
        drawBigRoundBracket( p, style, leftRoundBracket, x, y, charHeight );
	//cout << "drawLeftBigRoundBracket" << endl;
    }
}

void Artwork::drawRightRoundBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y,
                                     luPt height, luPt charHeight )
{
    if ( height <= 2*charHeight ) {
        drawSmallRoundBracket( p, style, rightRoundBracket, x, y, charHeight );
    }
    else {
        drawBigRoundBracket( p, style, rightRoundBracket, x, y, charHeight );
        //p.drawRect( x, y, getWidth(), getHeight() );
    }
}

void Artwork::drawSmallRoundBracket(QPainter& p, const ContextStyle& style, const QChar chars[],
                                    luPixel x, luPixel y, luPt charHeight)
{
    //kdDebug() << "Artwork::drawSmallRoundBracket" << endl;
    uchar uppercorner = style.symbolTable().character( chars[0] );
    uchar lowercorner = style.symbolTable().character( chars[1] );

    //QFont f = style.getSymbolFont();
    QFont f = style.symbolTable().font( chars[0] );
    f.setPointSizeFloat( style.layoutUnitToFontSize( charHeight, false ) );
    p.setFont(f);

    QFontMetrics fm(p.fontMetrics());
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);

    pt ptX = style.layoutUnitToPixelX( x );
    pt ptY = style.layoutUnitToPixelY( y );

    //p.setPen( Qt::gray );
    //p.drawRect( ptX, ptY, upperBound.width(), upperBound.height() + lowerBound.height() );

    //p.setPen( Qt::black );
    p.drawText( ptX, ptY-upperBound.top(), QString( QChar( uppercorner ) ) );
    p.drawText( ptX, ptY+upperBound.height()-lowerBound.top(), QString( QChar( lowercorner ) ) );
}

void Artwork::drawBigRoundBracket( QPainter& p, const ContextStyle& style, const QChar chars[],
                                   luPixel x, luPixel y, luPt charHeight )
{
    //kdDebug() << "Artwork::drawBigRoundBracket" << endl;
    uchar uppercorner = style.symbolTable().character( chars[0] );
    uchar lowercorner = style.symbolTable().character( chars[1] );
    uchar line = style.symbolTable().character( chars[2] );

    //QFont f = style.getSymbolFont();
    QFont f = style.symbolTable().font( chars[0] );
    f.setPointSizeFloat( style.layoutUnitToFontSize( charHeight, false ) );
    p.setFont(f);

    QFontMetrics fm(f);
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);
    QRect lineBound = fm.boundingRect(line);

    pt ptX = style.layoutUnitToPixelX( x );
    pt ptY = style.layoutUnitToPixelY( y );
    pt height = style.layoutUnitToPixelY( getHeight() );

    p.drawText( ptX, ptY-upperBound.top(), QString( QChar( uppercorner ) ) );
    p.drawText( ptX, ptY+height-lowerBound.top()-lowerBound.height(),
                QString( QChar( lowercorner ) ) );

    // for printing
    // If the world was perfect and the urw-symbol font correct
    // this could be 0.
    //pt safety = lineBound.height() / 10.0;
    pt safety = 0;

    pt gap = height - upperBound.height() - lowerBound.height();
    pt lineHeight = lineBound.height() - safety;
    int lineCount = static_cast<int>( gap / lineHeight );
    pt start = upperBound.height()-lineBound.top() - safety;

    for (int i = 0; i < lineCount; i++) {
        p.drawText( ptX, ptY+start+i*lineHeight, QString(QChar(line)));
    }
    pt remaining = gap - lineCount*lineHeight;
    pt dist = ( lineHeight - remaining ) / 2;
    p.drawText( ptX, ptY+height-upperBound.height()+dist-lineBound.height()-lineBound.top(),
                QString( QChar( line ) ) );
}


void Artwork::calcSquareBracket( const ContextStyle& style, const QChar chars[],
                                 luPt height, luPt charHeight )
{
    //char uppercorner = chars[0];
    //char lowercorner = chars[1];
    uchar line = style.symbolTable().character( chars[2] );

    //QFont f = style.getSymbolFont();
    QFont f = style.symbolTable().font( chars[0] );
    f.setPointSize( charHeight );
    QFontMetrics fm( f );
    //LuRect upperBound = fm.boundingRect(uppercorner);
    //LuRect lowerBound = fm.boundingRect(lowercorner);

    setWidth( fm.width( QChar( line ) ) );
    setHeight( ( ( height-1 ) / charHeight + 1 ) * charHeight );
}

void Artwork::drawLeftLineBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y,
                                   luPt height, luPt charHeight )
{
    if ( height <= 2*charHeight ) {
        drawSmallRoundBracket( p, style, leftLineBracket, x, y, charHeight );
    }
    else {
        drawBigRoundBracket( p, style, leftLineBracket, x, y, charHeight );
    }
}

void Artwork::drawRightLineBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y,
                                    luPt height, luPt charHeight )
{
    if ( height <= 2*charHeight ) {
        drawSmallRoundBracket( p, style, rightLineBracket, x, y, charHeight );
    }
    else {
        drawBigRoundBracket( p, style, rightLineBracket, x, y, charHeight );
    }
}

void Artwork::drawLeftSquareBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y,
                                     luPt height, luPt charHeight )
{
    if ( height <= 2*charHeight ) {
        drawSmallRoundBracket( p, style, leftSquareBracket, x, y, charHeight );
    }
    else {
        drawBigRoundBracket( p, style, leftSquareBracket, x, y, charHeight );
    }
}

void Artwork::drawRightSquareBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y,
                                      luPt height, luPt charHeight )
{
    if ( height <= 2*charHeight ) {
        drawSmallRoundBracket( p, style, rightSquareBracket, x, y, charHeight );
    }
    else {
        drawBigRoundBracket( p, style, rightSquareBracket, x, y, charHeight );
    }
}


void Artwork::calcCurlyBracket( const ContextStyle& style, const QChar chars[],
                                luPt height, luPt charHeight )
{
    //char uppercorner = chars[0];
    //char lowercorner = chars[1];
    uchar line = style.symbolTable().character( chars[2] );
    //char middle = chars[3];

    //QFont f = style.getSymbolFont();
    QFont f = style.symbolTable().font( chars[0] );
    f.setPointSize( charHeight );
    QFontMetrics fm( f );
    //LuRect upperBound = fm.boundingRect(uppercorner);
    //LuRect lowerBound = fm.boundingRect(lowercorner);
    //LuRect lineBound = fm.boundingRect(line);
    //LuRect middleBound = fm.boundingRect(middle);

    setWidth( fm.width( QChar( line ) ) );
    setHeight( QMAX( ( height-1 ) / charHeight + 1, 3 ) * charHeight );
}

void Artwork::drawLeftCurlyBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y,
                                    luPt height, luPt charHeight )
{
    if ( height <= 3*charHeight ) {
        drawSmallCurlyBracket( p, style, leftCurlyBracket, x, y, charHeight );
    }
    else {
        drawBigCurlyBracket( p, style, leftCurlyBracket, x, y, charHeight );
    }
}

void Artwork::drawRightCurlyBracket( QPainter& p, const ContextStyle& style, luPixel x, luPixel y,
                                     luPt height, luPt charHeight )
{
    if ( height <= 3*charHeight ) {
        drawSmallCurlyBracket( p, style, rightCurlyBracket, x, y, charHeight );
    }
    else {
        drawBigCurlyBracket( p, style, rightCurlyBracket, x, y, charHeight );
    }
}

void Artwork::drawSmallCurlyBracket(QPainter& p, const ContextStyle& style, const QChar chars[],
                                    luPixel x, luPixel y, luPt charHeight)
{
    //QFont f = style.getSymbolFont();
    QFont f = style.symbolTable().font( chars[0] );
    f.setPointSizeFloat( style.layoutUnitToFontSize( charHeight, false ) );
    p.setFont(f);

    uchar uppercorner = style.symbolTable().character( chars[0] );
    uchar lowercorner = style.symbolTable().character( chars[1] );
    //char line = chars[2];
    uchar middle = style.symbolTable().character( chars[3] );

    QFontMetrics fm(p.fontMetrics());
    QRect upperBound = fm.boundingRect(uppercorner);
    QRect lowerBound = fm.boundingRect(lowercorner);
    //LuRect lineBound = fm.boundingRect(line);
    QRect middleBound = fm.boundingRect(middle);

    pt ptX = style.layoutUnitToPixelX( x );
    pt ptY = style.layoutUnitToPixelY( y );

    //p.setPen(Qt::gray);
    //p.drawRect(x, y, upperBound.width() + offset, upperBound.height() + lowerBound.height() + middleBound.height());

    //p.setPen(Qt::black);
    p.drawText( ptX,ptY-upperBound.top(), QString( QChar( uppercorner ) ) );
    p.drawText( ptX, ptY+upperBound.height()-middleBound.top(), QString( QChar( middle ) ) );
    p.drawText( ptX, ptY+upperBound.height()+middleBound.height()-lowerBound.top(),
                QString( QChar( lowercorner ) ) );
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

    pt ptX = style.layoutUnitToPixelX( x );
    pt ptY = style.layoutUnitToPixelY( y );
    pt height = style.layoutUnitToPixelY( getHeight() );

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
    pt safety = 0;

    pt lineHeight = lineBound.height() - safety;
    pt gap = height/2 - upperBound.height() - middleBound.height() / 2;

    if (gap > 0) {
        QString ch = QString(QChar(line));
        int lineCount = static_cast<int>( gap / lineHeight ) + 1;

        pt start = (height - middleBound.height()) / 2 + safety;
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
