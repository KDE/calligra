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


KFORMULA_NAMESPACE_BEGIN

const char Artwork::leftRoundBracket[] = {
    static_cast<char>(230), // uppercorner
    static_cast<char>(232), // lowercorner
    static_cast<char>(231)  // line
};
const char Artwork::leftSquareBracket[] = {
    static_cast<char>(233), // uppercorner
    static_cast<char>(235), // lowercorner
    static_cast<char>(234)  // line
};
const char Artwork::leftCurlyBracket[] = {
    static_cast<char>(236), // uppercorner
    static_cast<char>(238), // lowercorner
    static_cast<char>(239), // line
    static_cast<char>(237)  // middle
};

const char Artwork::rightRoundBracket[] = {
    static_cast<char>(246), // uppercorner
    static_cast<char>(248), // lowercorner
    static_cast<char>(247)  // line
};
const char Artwork::rightSquareBracket[] = {
    static_cast<char>(249), // uppercorner
    static_cast<char>(251), // lowercorner
    static_cast<char>(250)  // line
};
const char Artwork::rightCurlyBracket[] = {
    static_cast<char>(252), // uppercorner
    static_cast<char>(254), // lowercorner
    static_cast<char>(239), // line
    static_cast<char>(253)  // middle
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
            calcCharSize( style, mySize, '[' );
        }
        break;
    case RightSquareBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            calcSquareBracket( style, rightSquareBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, ']');
        }
        break;
    case LineBracket:
        calcCharSize(style, mySize, '|');
        break;
    case SlashBracket:
        calcCharSize(style, mySize, '/');
        break;
    case BackSlashBracket:
        calcCharSize(style, mySize, '\\');
        break;
    case LeftCornerBracket:
        calcCharSize(style, mySize, static_cast<char>(225));
        break;
    case RightCornerBracket:
        calcCharSize(style, mySize, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            calcRoundBracket( style, leftRoundBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, '(');
        }
        break;
    case RightRoundBracket:
        if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            calcRoundBracket( style, rightRoundBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, ')');
        }
        break;
    case Empty:
        setHeight(parentSize);
        setWidth(style.getEmptyRectWidth());
        break;
    case LeftCurlyBracket:
        if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            calcCurlyBracket( style, leftCurlyBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, '{');
        }
        break;
    case RightCurlyBracket:
        if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            calcCurlyBracket( style, rightCurlyBracket, parentSize, mySize );
        }
        else {
            calcCharSize(style, mySize, '}');
        }
        break;
    case Integral:
        calcCharSize( style, 1.5*mySize, static_cast<char>(242) );
        break;
    case Sum:
        calcCharSize( style, 1.5*mySize, static_cast<char>(229) );
        break;
    case Product:
        calcCharSize( style, 1.5*mySize, static_cast<char>(213) );
        break;
    }
}


void Artwork::calcSizes( const ContextStyle& style,
                         ContextStyle::TextStyle tstyle )
{
    luPt mySize = style.getAdjustedSize( tstyle );
    switch (type) {
    case LeftSquareBracket:
        calcCharSize(style, mySize, '[');
        break;
    case RightSquareBracket:
        calcCharSize(style, mySize, ']');
        break;
    case LineBracket:
        calcCharSize(style, mySize, '|');
        break;
    case SlashBracket:
        calcCharSize(style, mySize, '/');
        break;
    case BackSlashBracket:
        calcCharSize(style, mySize, '\\');
        break;
    case LeftCornerBracket:
        calcCharSize(style, mySize, static_cast<char>(225));
        break;
    case RightCornerBracket:
        calcCharSize(style, mySize, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        calcCharSize(style, mySize, '(');
        break;
    case RightRoundBracket:
        calcCharSize(style, mySize, ')');
        break;
    case Empty:
        calcCharSize(style, mySize, ' ');
        break;
    case LeftCurlyBracket:
        calcCharSize(style, mySize, '{');
        break;
    case RightCurlyBracket:
        calcCharSize(style, mySize, '}');
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
            drawCharacter(painter, style, myX, myY, mySize, '[');
        }
        break;
    case RightSquareBracket:
        if ( !doSimpleSquareBracket( parentSize, mySize ) ) {
            drawRightSquareBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, ']');
        }
        break;
    case LeftCurlyBracket:
        if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            drawLeftCurlyBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, '{');
        }
        break;
    case RightCurlyBracket:
        if ( !doSimpleCurlyBracket( parentSize, mySize ) ) {
            drawRightCurlyBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, '}');
        }
        break;
    case LineBracket:
        drawCharacter(painter, style, myX, myY, mySize, '|');
        break;
    case SlashBracket:
        drawCharacter(painter, style, myX, myY, mySize, '/');
        break;
    case BackSlashBracket:
        drawCharacter(painter, style, myX, myY, mySize, '\\');
        break;
    case LeftCornerBracket:
        drawCharacter(painter, style, myX, myY, mySize, static_cast<char>(225));
        break;
    case RightCornerBracket:
        drawCharacter(painter, style, myX, myY, mySize, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            drawLeftRoundBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, '(');
        }
        break;
    case RightRoundBracket:
        if ( !doSimpleRoundBracket( parentSize, mySize ) ) {
            drawRightRoundBracket( painter, style, myX, myY, parentSize, mySize );
        }
        else {
            drawCharacter(painter, style, myX, myY, mySize, ')');
        }
        break;
    case Empty:
        //drawEmpty(painter, myX, myY, getHeight());
        break;
    case Integral:
        drawCharacter(painter, style, myX, myY, 1.5*mySize, static_cast<char>(242));
        break;
    case Sum:
        drawCharacter(painter, style, myX, myY, 1.5*mySize, static_cast<char>(229));
        break;
    case Product:
        drawCharacter(painter, style, myX, myY, 1.5*mySize, static_cast<char>(213));
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
        drawCharacter(painter, style, myX, myY, mySize, '[');
        break;
    case RightSquareBracket:
        drawCharacter(painter, style, myX, myY, mySize, ']');
        break;
    case LeftCurlyBracket:
        drawCharacter(painter, style, myX, myY, mySize, '{');
        break;
    case RightCurlyBracket:
        drawCharacter(painter, style, myX, myY, mySize, '}');
        break;
    case LineBracket:
        drawCharacter(painter, style, myX, myY, mySize, '|');
        break;
    case SlashBracket:
        drawCharacter(painter, style, myX, myY, mySize, '/');
        break;
    case BackSlashBracket:
        drawCharacter(painter, style, myX, myY, mySize, '\\');
        break;
    case LeftCornerBracket:
        drawCharacter(painter, style, myX, myY, mySize, static_cast<char>(225));
        break;
    case RightCornerBracket:
        drawCharacter(painter, style, myX, myY, mySize, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        drawCharacter(painter, style, myX, myY, mySize, '(');
        break;
    case RightRoundBracket:
        drawCharacter(painter, style, myX, myY, mySize, ')');
        break;
    case Empty:
        break;
    case Integral:
    case Sum:
    case Product:
        break;
    }
}


void Artwork::calcCharSize( const ContextStyle& style, luPt height, QChar ch )
{
    QFont f = style.getSymbolFont();
    f.setPointSize( height );

    QFontMetrics fm(f);
    setWidth( fm.width( ch ) );
    LuPixelRect bound = fm.boundingRect( ch );
    setHeight( bound.height() );
    setBaseline( -bound.top() );
}


void Artwork::drawCharacter( QPainter& painter, const ContextStyle& style, luPixel x, luPixel y,
                             luPt height, QChar ch )
{
    QFont f = style.getSymbolFont();
    f.setPointSizeFloat( style.layoutUnitToFontSize( height, false ) );

    painter.setFont( f );
    painter.drawText( style.layoutUnitToPixelX( x ),
                      style.layoutUnitToPixelY( y+getBaseline() ),
                      QString( ch ) );

    //painter.drawRect(bound);
    //painter.drawRect(x, y, getWidth(), getHeight());
    //cerr << bound.x() << " " << bound.y() << " " << bound.width() << " " << bound.height() << endl;
}


void Artwork::calcRoundBracket( const ContextStyle& style, const char chars[],
                                luPt height, luPt charHeight )
{
    //char uppercorner = chars[0];
    //char lowercorner = chars[1];
    char line = chars[2];

    QFont f = style.getSymbolFont();
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

void Artwork::drawSmallRoundBracket(QPainter& p, const ContextStyle& style, const char chars[],
                                    luPixel x, luPixel y, luPt charHeight)
{
    //kdDebug() << "Artwork::drawSmallRoundBracket" << endl;
    QFont f = style.getSymbolFont();
    f.setPointSizeFloat( style.layoutUnitToFontSize( charHeight, false ) );
    p.setFont(f);

    char uppercorner = chars[0];
    char lowercorner = chars[1];

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

void Artwork::drawBigRoundBracket( QPainter& p, const ContextStyle& style, const char chars[],
                                   luPixel x, luPixel y, luPt charHeight )
{
    //kdDebug() << "Artwork::drawBigRoundBracket" << endl;
    QFont f = style.getSymbolFont();
    f.setPointSizeFloat( style.layoutUnitToFontSize( charHeight, false ) );
    p.setFont(f);

    char uppercorner = chars[0];
    char lowercorner = chars[1];
    char line = chars[2];

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


void Artwork::calcSquareBracket( const ContextStyle& style, const char chars[],
                                 luPt height, luPt charHeight )
{
    //char uppercorner = chars[0];
    //char lowercorner = chars[1];
    char line = chars[2];

    QFont f = style.getSymbolFont();
    f.setPointSize( charHeight );
    QFontMetrics fm( f );
    //LuRect upperBound = fm.boundingRect(uppercorner);
    //LuRect lowerBound = fm.boundingRect(lowercorner);

    setWidth( fm.width( QChar( line ) ) );
    setHeight( ( ( height-1 ) / charHeight + 1 ) * charHeight );
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


void Artwork::calcCurlyBracket( const ContextStyle& style, const char chars[],
                                luPt height, luPt charHeight )
{
    //char uppercorner = chars[0];
    //char lowercorner = chars[1];
    char line = chars[2];
    //char middle = chars[3];

    QFont f = style.getSymbolFont();
    f.setPointSize( charHeight );
    QFontMetrics fm( f );
    //LuRect upperBound = fm.boundingRect(uppercorner);
    //LuRect lowerBound = fm.boundingRect(lowercorner);
    //LuRect lineBound = fm.boundingRect(line);
    //LuRect middleBound = fm.boundingRect(middle);

    setWidth( fm.width( QChar( line ) ) );
    setHeight( ( ( height-1 ) / charHeight + 1 ) * charHeight );
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

void Artwork::drawSmallCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[],
                                    luPixel x, luPixel y, luPt charHeight)
{
    QFont f = style.getSymbolFont();
    f.setPointSizeFloat( style.layoutUnitToFontSize( charHeight, false ) );
    p.setFont(f);

    char uppercorner = chars[0];
    char lowercorner = chars[1];
    //char line = chars[2];
    char middle = chars[3];

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

void Artwork::drawBigCurlyBracket( QPainter& p, const ContextStyle& style, const char chars[],
                                   luPixel x, luPixel y, luPt charHeight )
{
    QFont f = style.getSymbolFont();
    f.setPointSizeFloat( style.layoutUnitToFontSize( charHeight, false ) );
    p.setFont(f);

    char uppercorner = chars[0];
    char lowercorner = chars[1];
    char line = chars[2];
    char middle = chars[3];

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
