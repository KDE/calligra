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


Artwork::Artwork(SymbolType t)
    : baseline( -1 ), type(t)
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
    luPt mySize = style.getAdjustedSize( tstyle );
    switch (type) {
    case LeftSquareBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            calcRoundBracket( style, leftSquareBracket, parentSize, mySize );
        }
        else {
            calcCharSize( style, mySize, leftSquareBracketChar );
        }
        break;
    case RightSquareBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            calcRoundBracket( style, rightSquareBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, rightSquareBracketChar);
        }
        break;
    case LeftLineBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            calcRoundBracket( style, leftLineBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, verticalLineChar);
        }
        break;
    case RightLineBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            calcRoundBracket( style, rightLineBracket, parentSize, mySize );
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

    switch (type) {
    case LeftSquareBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawBigRoundBracket( painter, style, leftSquareBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, leftSquareBracketChar);
        }
        break;
    case RightSquareBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawBigRoundBracket( painter, style, rightSquareBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, rightSquareBracketChar);
        }
        break;
    case LeftCurlyBracket:
        if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            drawBigCurlyBracket( painter, style, leftCurlyBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, leftCurlyBracketChar);
        }
        break;
    case RightCurlyBracket:
        if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            drawBigCurlyBracket( painter, style, rightCurlyBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, rightCurlyBracketChar);
        }
        break;
    case LeftLineBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawBigRoundBracket( painter, style, leftLineBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, verticalLineChar);
        }
        break;
    case RightLineBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
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
        drawCharacter(painter, style, myX, myY, mySize, leftAngleBracketChar);
        break;
    case RightCornerBracket:
        drawCharacter(painter, style, myX, myY, mySize, rightAngleBracketChar);
        break;
    case LeftRoundBracket:
        if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            drawBigRoundBracket( painter, style, leftRoundBracket, myX, myY, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, leftParenthesisChar);
        }
        break;
    case RightRoundBracket:
        if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
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
    f.setPointSizeFloat( style.layoutUnitPtToPt( height ) );
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

    //painter.drawRect(bound);
    //painter.drawRect(x, y, getWidth(), getHeight());
    //cerr << bound.x() << " " << bound.y() << " " << bound.width() << " " << bound.height() << endl;
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
