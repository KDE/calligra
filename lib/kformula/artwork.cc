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


void Artwork::calcCharSize( const ContextStyle& style, lu height, QChar ch )
{
    QFont f = style.getSymbolFont();
    f.setPointSize( height );
    fontSize = height;

    QFontMetrics fm(f);
    LuRect bound = fm.boundingRect( ch );
    setWidth( fm.width( ch ) );
    setHeight( bound.height() );
    setBaseline( -bound.top() );
}


bool Artwork::doSimpleRoundBracket( const ContextStyle& style, lu height )
{
    return height < style.getBaseSize();
}

bool Artwork::doSimpleSquareBracket( const ContextStyle& style, lu height )
{
    return height < style.getBaseSize();
}

bool Artwork::doSimpleCurlyBracket( const ContextStyle& style, lu height )
{
    return height < 2*( style.getBaseSize() );
}


void Artwork::calcSizes( const ContextStyle& style, lu parentSize )
{
    switch (type) {
    case LeftSquareBracket:
        if (!doSimpleSquareBracket(style, parentSize)) {
            calcSquareBracket(style, leftSquareBracket, parentSize);
        }
        else {
            calcCharSize(style, parentSize, '[');
        }
        break;
    case RightSquareBracket:
        if (!doSimpleSquareBracket(style, parentSize)) {
            calcSquareBracket(style, rightSquareBracket, parentSize);
        }
        else {
            calcCharSize(style, parentSize, ']');
        }
        break;
    case LineBracket:
        calcCharSize(style, parentSize, '|');
        break;
    case SlashBracket:
        calcCharSize(style, parentSize, '/');
        break;
    case BackSlashBracket:
        calcCharSize(style, parentSize, '\\');
        break;
    case LeftCornerBracket:
        calcCharSize(style, parentSize, static_cast<char>(225));
        break;
    case RightCornerBracket:
        calcCharSize(style, parentSize, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        if (!doSimpleRoundBracket(style, parentSize)) {
            calcRoundBracket(style, leftRoundBracket, parentSize);
        }
        else {
            calcCharSize(style, parentSize, '(');
        }
        break;
    case RightRoundBracket:
        if (!doSimpleRoundBracket(style, parentSize)) {
            calcRoundBracket(style, rightRoundBracket, parentSize);
        }
        else {
            calcCharSize(style, parentSize, ')');
        }
        break;
    case Empty:
        setHeight(parentSize);
        setWidth(style.getEmptyRectWidth());
        break;
    case LeftCurlyBracket:
        if (!doSimpleCurlyBracket(style, parentSize)) {
            calcCurlyBracket(style, leftCurlyBracket, parentSize);
        }
        else {
            calcCharSize(style, parentSize, '{');
        }
        break;
    case RightCurlyBracket:
        if (!doSimpleCurlyBracket(style, parentSize)) {
            calcCurlyBracket(style, rightCurlyBracket, parentSize);
        }
        else {
            calcCharSize(style, parentSize, '}');
        }
        break;
    case Integral:
        calcCharSize(style, parentSize, static_cast<char>(242));
        break;
    case Sum:
        calcCharSize(style, parentSize, static_cast<char>(229));
        break;
    case Product:
        calcCharSize(style, parentSize, static_cast<char>(213));
        break;
    }
}


void Artwork::calcSizes( const ContextStyle& style,
                         ContextStyle::TextStyle tstyle )
{
    double mySize = style.getAdjustedSize( tstyle );
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


void Artwork::draw(QPainter& painter, const LuRect& r, const ContextStyle& style,
                   lu parentSize, const LuPoint& origin)
{
    lu myX = origin.x() + getX();
    lu myY = origin.y() + getY();
    if ( !LuRect( myX, myY, getWidth(), getHeight() ).intersects( r ) )
        return;

    painter.setPen(style.getDefaultColor());

    switch (type) {
    case LeftSquareBracket:
        if (!doSimpleSquareBracket(style, parentSize)) {
            drawLeftSquareBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '[');
        }
        break;
    case RightSquareBracket:
        if (!doSimpleSquareBracket(style, parentSize)) {
            drawRightSquareBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, ']');
        }
        break;
    case LeftCurlyBracket:
        if (!doSimpleCurlyBracket(style, parentSize)) {
            drawLeftCurlyBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '{');
        }
        break;
    case RightCurlyBracket:
        if (!doSimpleCurlyBracket(style, parentSize)) {
            drawRightCurlyBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '}');
        }
        break;
    case LineBracket:
        drawCharacter(painter, style, myX, myY, '|');
        break;
    case SlashBracket:
        drawCharacter(painter, style, myX, myY, '/');
        break;
    case BackSlashBracket:
        drawCharacter(painter, style, myX, myY, '\\');
        break;
    case LeftCornerBracket:
        drawCharacter(painter, style, myX, myY, static_cast<char>(225));
        break;
    case RightCornerBracket:
        drawCharacter(painter, style, myX, myY, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        if (!doSimpleRoundBracket(style, parentSize)) {
            drawLeftRoundBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, '(');
        }
        break;
    case RightRoundBracket:
        if (!doSimpleRoundBracket(style, parentSize)) {
            drawRightRoundBracket(painter, style, myX, myY, parentSize);
        }
        else {
            drawCharacter(painter, style, myX, myY, ')');
        }
        break;
    case Empty:
        //drawEmpty(painter, myX, myY, getHeight());
        break;
    case Integral:
        //drawIntegral(painter, style, myX, myY+getHeight()/2, getHeight()/2);
        drawCharacter(painter, style, myX, myY, static_cast<char>(242));
        break;
    case Sum:
        drawCharacter(painter, style, myX, myY, static_cast<char>(229));
        break;
    case Product:
        drawCharacter(painter, style, myX, myY, static_cast<char>(213));
        break;
    }

    // debug
    //painter.setBrush(Qt::NoBrush);
    //painter.setPen(Qt::green);
    //painter.drawRect(myX, myY, getWidth(), getHeight());
}


void Artwork::draw(QPainter& painter, const LuRect& r, const ContextStyle& style,
                   ContextStyle::TextStyle, const LuPoint& parentOrigin)
{
    lu myX = parentOrigin.x() + getX();
    lu myY = parentOrigin.y() + getY();
    if ( !LuRect( myX, myY, getWidth(), getHeight() ).intersects( r ) )
        return;

    painter.setPen(style.getDefaultColor());

    switch (type) {
    case LeftSquareBracket:
        drawCharacter(painter, style, myX, myY, '[');
        break;
    case RightSquareBracket:
        drawCharacter(painter, style, myX, myY, ']');
        break;
    case LeftCurlyBracket:
        drawCharacter(painter, style, myX, myY, '{');
        break;
    case RightCurlyBracket:
        drawCharacter(painter, style, myX, myY, '}');
        break;
    case LineBracket:
        drawCharacter(painter, style, myX, myY, '|');
        break;
    case SlashBracket:
        drawCharacter(painter, style, myX, myY, '/');
        break;
    case BackSlashBracket:
        drawCharacter(painter, style, myX, myY, '\\');
        break;
    case LeftCornerBracket:
        drawCharacter(painter, style, myX, myY, static_cast<char>(225));
        break;
    case RightCornerBracket:
        drawCharacter(painter, style, myX, myY, static_cast<char>(241));
        break;
    case LeftRoundBracket:
        drawCharacter(painter, style, myX, myY, '(');
        break;
    case RightRoundBracket:
        drawCharacter(painter, style, myX, myY, ')');
        break;
    case Empty:
        break;
    case Integral:
    case Sum:
    case Product:
        break;
    }
}


void Artwork::drawCharacter( QPainter& painter, const ContextStyle& style, lu x, lu y, QChar ch )
{
    QFont f = style.getSymbolFont();
    f.setPointSizeFloat( style.layoutUnitToFontSize( fontSize, false ) );

    painter.setFont( f );
    painter.drawText( style.layoutUnitToPixelX( x ),
                      style.layoutUnitToPixelY( y+getBaseline() ),
                      QString( ch ) );

    //painter.drawRect(bound);
    //painter.drawRect(x, y, getWidth(), getHeight());
    //cerr << bound.x() << " " << bound.y() << " " << bound.width() << " " << bound.height() << endl;
}


void Artwork::calcRoundBracket(const ContextStyle& style, const char chars[], lu height)
{
    lu charHeight;
    char uppercorner = chars[0];
    char lowercorner = chars[1];
    char line = chars[2];

    if ( height <= 80*20 ) {
        charHeight = height/2;
    }
    else {
        charHeight = 40*20;
    }

    QFont f = style.getSymbolFont();
    f.setPointSize(charHeight);
    QFontMetrics fm(f);
    LuRect upperBound = fm.boundingRect(uppercorner);
    LuRect lowerBound = fm.boundingRect(lowercorner);

    setWidth( fm.width( QChar( line ) ) );
    if ( height <= 80*20 ) {
        setHeight(upperBound.height() + lowerBound.height());
    }
    else {
        setHeight(height);
    }
}

void Artwork::drawLeftRoundBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height)
{
    if ( height <= 80*20 ) {
        drawSmallRoundBracket(p, style, leftRoundBracket, x, y, height/2);
	//cout << "drawLeftSmallRoundBracket" << endl;
    }
    else {
        drawBigRoundBracket(p, style, leftRoundBracket, x, y, 40*20, height);
	//cout << "drawLeftBigRoundBracket" << endl;
    }
}

void Artwork::drawRightRoundBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height)
{
    if ( height <= 80*20 ) {
        drawSmallRoundBracket(p, style, rightRoundBracket, x, y, height/2);
    }
    else {
        drawBigRoundBracket(p, style, rightRoundBracket, x, y, 40*20, height);
        //p.drawRect( x, y, getWidth(), getHeight() );
    }
}

void Artwork::drawSmallRoundBracket(QPainter& p, const ContextStyle& style, const char chars[],
                                    lu x, lu y, lu charHeight)
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

void Artwork::drawBigRoundBracket(QPainter& p, const ContextStyle& style, const char chars[],
                                  lu x, lu y, lu charHeight, lu height)
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


void Artwork::calcSquareBracket(const ContextStyle& style, const char chars[], lu height)
{
    lu charHeight;
    char uppercorner = chars[0];
    char lowercorner = chars[1];
    char line = chars[2];

    if ( height <= 80*20 ) {
        charHeight = height/2;
    }
    else {
        charHeight = 40*20;
        //line = leftBigRoundBracket[2];
    }

    QFont f = style.getSymbolFont();
    f.setPointSize(charHeight);
    QFontMetrics fm(f);
    LuRect upperBound = fm.boundingRect(uppercorner);
    LuRect lowerBound = fm.boundingRect(lowercorner);

    setWidth( fm.width( QChar( line ) ) );
    if (height <= 80*20) {
        setHeight(upperBound.height() + lowerBound.height());
    }
    else {
        setHeight(height);
    }
}

void Artwork::drawLeftSquareBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height)
{
    if ( height <= 80*20 ) {
        drawSmallRoundBracket(p, style, leftSquareBracket, x, y, height/2);
    }
    else {
        drawBigRoundBracket(p, style, leftSquareBracket, x, y, 40*20, height);
    }
}

void Artwork::drawRightSquareBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height)
{
    if (height <= 80*20) {
        drawSmallRoundBracket(p, style, rightSquareBracket, x, y, height/2);
    }
    else {
        drawBigRoundBracket(p, style, rightSquareBracket, x, y, 40*20, height);
    }
}


void Artwork::calcCurlyBracket(const ContextStyle& style, const char chars[], lu height)
{
    lu charHeight;
    char uppercorner = chars[0];
    char lowercorner = chars[1];
    char line = chars[2];
    char middle = chars[3];

    if (height <= 120*20) {
        charHeight = height/3;
    }
    else {
        charHeight = 40*20;
    }

    QFont f = style.getSymbolFont();
    f.setPointSize(charHeight);
    QFontMetrics fm(f);
    LuRect upperBound = fm.boundingRect(uppercorner);
    LuRect lowerBound = fm.boundingRect(lowercorner);
    //LuRect lineBound = fm.boundingRect(line);
    LuRect middleBound = fm.boundingRect(middle);

    setWidth( fm.width( QChar( line ) ) );
    if ( height <= 120*20 ) {
        setHeight(upperBound.height() + lowerBound.height() + middleBound.height());
    }
    else {
        setHeight(height);
    }
}

void Artwork::drawLeftCurlyBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height)
{
    if (height <= 120*20) {
        drawSmallCurlyBracket(p, style, leftCurlyBracket, x, y, height/3);
    }
    else {
        drawBigCurlyBracket(p, style, leftCurlyBracket, x, y, 40*20, height);
    }
}

void Artwork::drawRightCurlyBracket(QPainter& p, const ContextStyle& style, lu x, lu y, lu height)
{
    if (height <= 120*20) {
        drawSmallCurlyBracket(p, style, rightCurlyBracket, x, y, height/3);
    }
    else {
        drawBigCurlyBracket(p, style, rightCurlyBracket, x, y, 40*20, height);
    }
}

void Artwork::drawSmallCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[],
                                    lu x, lu y, lu charHeight)
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

void Artwork::drawBigCurlyBracket(QPainter& p, const ContextStyle& style, const char chars[],
                                  lu x, lu y, lu charHeight, lu height)
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
