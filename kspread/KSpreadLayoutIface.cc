/* This file is part of the KDE project
   
   Copyright 2002 Laurent Montel <montel@kde.org>
   Copyright 2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002 John Dailey <dailey@vt.edu>

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

#include "KSpreadLayoutIface.h"
#include <qbrush.h>
#include "kspread_format.h"

KSpreadLayoutIface::KSpreadLayoutIface(KSpreadFormat *_layout)
{
    layout=_layout;
}

void KSpreadLayoutIface::setBgColor(const QString& _c)
{
    QColor c(_c);
    layout->setBgColor(c);
}

void KSpreadLayoutIface::setBgColor(int r,int g,int b)
{
    QColor c(r,g,b);
    layout->setBgColor(c);
}


void KSpreadLayoutIface::setTextColor(int r,int g,int b)
{
    QColor c(r,g,b);
    layout->setTextColor(c);
}

void KSpreadLayoutIface::setTextColor(const QString& _c)
{
    QColor c(_c);
    layout->setTextColor(c);
}

void KSpreadLayoutIface::setAngle(int angle)
{
    layout->setAngle(angle);
}

void KSpreadLayoutIface::setVerticalText(bool _vertical)
{
    layout->setVerticalText(_vertical);
}

void KSpreadLayoutIface::setMultiRow(bool _multi)
{
    layout->setMultiRow( _multi );
}

void KSpreadLayoutIface::setAlign( const QString& _Align )
{
    KSpreadFormat::Align Align;
    if(_Align=="Left")
        Align=KSpreadFormat::Left;
    else if(_Align=="Right")
        Align=KSpreadFormat::Right;
    else if(_Align=="Center")
        Align=KSpreadFormat::Center;
    else
        Align=KSpreadFormat::Undefined;
    layout->setAlign( Align);
}

void KSpreadLayoutIface::setAlignY( const QString& _AlignY )
{
    KSpreadFormat::AlignY AlignY;
    if(_AlignY=="Top")
        AlignY=KSpreadFormat::Top;
    else if(_AlignY=="Middle")
        AlignY=KSpreadFormat::Middle;
    else if(_AlignY=="Bottom")
        AlignY=KSpreadFormat::Bottom;
    else
        AlignY=KSpreadFormat::Middle;
    layout->setAlignY( AlignY);
}

void KSpreadLayoutIface::setPostfix(const QString &_postfix)
{
    layout->setPostfix( _postfix );
}

void KSpreadLayoutIface::setPrefix(const QString &_prefix)
{
    layout->setPrefix( _prefix );
}

void KSpreadLayoutIface::setFormatType(const QString &_formatType)
{
    FormatType format;
    layout->setFactor( 1.0);
    layout->setPrecision(2);
    if(_formatType=="Number")
        format=Number_format;
    else if(_formatType=="Money")
        format=Money_format;
    else if(_formatType=="Percentage")
        {
        format=Percentage_format;
        layout->setFactor( 100.0);
        }
    else if(_formatType=="Scientific")
        format=Scientific_format;
    else if(_formatType=="ShortDate")
        format=ShortDate_format;
    else if(_formatType=="TextDate")
        format=TextDate_format;
    else if(_formatType=="Time")
        format=Time_format;
    else if(_formatType=="SecondeTime")
        format=SecondeTime_format;
    else if(_formatType=="fraction_half")
        format=fraction_half;
    else if(_formatType=="fraction_quarter")
        format=fraction_quarter;
    else if(_formatType=="fraction_eighth")
        format=fraction_eighth;
    else if(_formatType=="fraction_sixteenth")
        format=fraction_sixteenth;
    else if(_formatType=="fraction_tenth")
        format=fraction_tenth;
    else if(_formatType=="fraction_hundredth")
        format=fraction_hundredth;
    else if(_formatType=="fraction_one_digit")
        format=fraction_one_digit;
    else if(_formatType=="fraction_two_digits")
        format=fraction_two_digits;
    else if(_formatType=="fraction_three_digits")
        format=fraction_three_digits;
    else
        format=Number_format;
    layout->setFormatType( format);
}


void KSpreadLayoutIface::setPrecision(int _p)
{
    layout->setPrecision( _p );
}

void KSpreadLayoutIface::setTextFontBold(bool _b)
{
    layout->setTextFontBold( _b );
}

void KSpreadLayoutIface::setTextFontItalic(bool _b)
{
    layout->setTextFontItalic( _b );
}

void KSpreadLayoutIface::setTextFontUnderline(bool _b)
{
    layout->setTextFontUnderline( _b );
}

void KSpreadLayoutIface::setTextFontStrike(bool _b)
{
    layout->setTextFontStrike( _b );
}

void KSpreadLayoutIface::setTextFontSize( int _size )
{
    layout->setTextFontSize( _size );
}

void KSpreadLayoutIface::setTextFontFamily( const QString& _font )
{
    layout->setTextFontFamily( _font );
}


void KSpreadLayoutIface::setFaktor( double _factor )
{
    layout->setFactor( _factor );
}

//border left
void KSpreadLayoutIface::setLeftBorderStyle( const QString& _style )
{
    if(_style=="DotLine")
        layout->setLeftBorderStyle(Qt::DotLine);
    else if(_style=="DashLine")
        layout->setLeftBorderStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        layout->setLeftBorderStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        layout->setLeftBorderStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        layout->setLeftBorderStyle(Qt::SolidLine);
    else
        layout->setLeftBorderStyle(Qt::SolidLine);
}

void KSpreadLayoutIface::setLeftBorderColor(const QString& _c)
{
    QColor c(_c);
    layout->setLeftBorderColor(c );
}

void KSpreadLayoutIface::setLeftBorderColor(int r,int g,int b)
{
    QColor c(r,g,b);
    layout->setLeftBorderColor(c );
}

void KSpreadLayoutIface::setLeftBorderWidth( int _size )
{
    layout->setLeftBorderWidth( _size );
}


//border right
void KSpreadLayoutIface::setRightBorderStyle( const QString& _style )
{
    if(_style=="DotLine")
        layout->setRightBorderStyle(Qt::DotLine);
    else if(_style=="DashLine")
        layout->setRightBorderStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        layout->setRightBorderStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        layout->setRightBorderStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        layout->setRightBorderStyle(Qt::SolidLine);
    else
        layout->setRightBorderStyle(Qt::SolidLine);
}

void KSpreadLayoutIface::setRightBorderColor(const QString& _c)
{
    QColor c(_c);
    layout->setRightBorderColor(c );
}

void KSpreadLayoutIface::setRightBorderColor(int r,int g,int b)
{
    QColor c(r,g,b);
    layout->setRightBorderColor(c );
}

void KSpreadLayoutIface::setRightBorderWidth( int _size )
{
    layout->setRightBorderWidth( _size );
}


//border top
void KSpreadLayoutIface::setTopBorderStyle( const QString& _style )
{
    if(_style=="DotLine")
        layout->setTopBorderStyle(Qt::DotLine);
    else if(_style=="DashLine")
        layout->setTopBorderStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        layout->setTopBorderStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        layout->setTopBorderStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        layout->setTopBorderStyle(Qt::SolidLine);
    else
        layout->setTopBorderStyle(Qt::SolidLine);
}

void KSpreadLayoutIface::setTopBorderColor(const QString& _c)
{
    QColor c(_c);
    layout->setTopBorderColor(c );
}

void KSpreadLayoutIface::setTopBorderColor(int r,int g,int b)
{
    QColor c(r,g,b);
    layout->setTopBorderColor(c );
}

void KSpreadLayoutIface::setTopBorderWidth( int _size )
{
    layout->setTopBorderWidth( _size );
}


//border bottom
void KSpreadLayoutIface::setBottomBorderStyle( const QString& _style )
{
    if(_style=="DotLine")
        layout->setBottomBorderStyle(Qt::DotLine);
    else if(_style=="DashLine")
        layout->setBottomBorderStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        layout->setBottomBorderStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        layout->setBottomBorderStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        layout->setBottomBorderStyle(Qt::SolidLine);
    else
        layout->setBottomBorderStyle(Qt::SolidLine);
}

void KSpreadLayoutIface::setBottomBorderColor(const QString& _c)
{
    QColor c(_c);
    layout->setBottomBorderColor(c );
}
void KSpreadLayoutIface::setBottomBorderColor(int r,int g,int b)
{
    QColor c(r,g,b);
    layout->setBottomBorderColor(c );
}

void KSpreadLayoutIface::setBottomBorderWidth( int _size )
{
    layout->setBottomBorderWidth( _size );
}


//fall back diagonal
void KSpreadLayoutIface::setFallDiagonalStyle( const QString& _style )
{
    if(_style=="DotLine")
        layout->setFallDiagonalStyle(Qt::DotLine);
    else if(_style=="DashLine")
        layout->setFallDiagonalStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        layout->setFallDiagonalStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        layout->setFallDiagonalStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        layout->setFallDiagonalStyle(Qt::SolidLine);
    else
        layout->setFallDiagonalStyle(Qt::SolidLine);
}

void KSpreadLayoutIface::setFallDiagonalColor(const QString& _c)
{
    QColor c(_c);
    layout->setFallDiagonalColor(c );
}
void KSpreadLayoutIface::setFallDiagonalColor(int r,int g,int b)
{
    QColor c(r,g,b);
    layout->setFallDiagonalColor(c );
}

void KSpreadLayoutIface::setFallDiagonalWidth( int _size )
{
    layout->setFallDiagonalWidth( _size );
}



//GoUpDiagonal
void KSpreadLayoutIface::setGoUpDiagonalStyle( const QString& _style )
{
    if(_style=="DotLine")
        layout->setGoUpDiagonalStyle(Qt::DotLine);
    else if(_style=="DashLine")
        layout->setGoUpDiagonalStyle(Qt::DashLine);
    else if(_style=="DashDotLine")
        layout->setGoUpDiagonalStyle(Qt::DashDotLine);
    else if(_style=="DashDotDotLine")
        layout->setGoUpDiagonalStyle(Qt::DashDotDotLine);
    else if(_style=="SolidLine")
        layout->setGoUpDiagonalStyle(Qt::SolidLine);
    else
        layout->setGoUpDiagonalStyle(Qt::SolidLine);
}

void KSpreadLayoutIface::setGoUpDiagonalColor(const QString& _c)
{
    QColor c(_c);
    layout->setGoUpDiagonalColor(c );
}
void KSpreadLayoutIface::setGoUpDiagonalColor(int r,int g,int b)
{
    QColor c(r,g,b);
    layout->setGoUpDiagonalColor(c );
}

void KSpreadLayoutIface::setGoUpDiagonalWidth( int _size )
{
    layout->setGoUpDiagonalWidth( _size );
}


void KSpreadLayoutIface::setIndent( double indent )
{
    if( indent >= 0.0 )
        layout->setIndent( indent );
    else
        layout->setIndent( 0.0 );
}


void KSpreadLayoutIface::setDontPrintText ( bool _print)
{
    layout->setDontPrintText ( _print);
}

