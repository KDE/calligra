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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_LAYOUT_IFACE_H
#define KSPREAD_LAYOUT_IFACE_H

#include <dcopobject.h>

namespace KSpread
{
class Format;

class LayoutIface : virtual public DCOPObject
{
    K_DCOP
public:
    LayoutIface(Format *_layout);
    virtual void update(){;}
k_dcop:

    virtual void setBgColor(const QString& _c);
    virtual void setBgColor(int r,int g,int b);
    virtual void setTextColor(const QString& _c);
    virtual void setTextColor(int r,int g,int b);
    virtual void setAngle(int angle);
    virtual void setVerticalText(bool _vertical);
    virtual void setMultiRow(bool _multi);
    virtual void setAlign(const QString &_align);
    virtual void setAlignY(const QString &_alignY);
    virtual void setPrefix(const QString &_prefix);
    virtual void setPostfix(const QString &_postfix);
    /** @deprecated use setFormatType*/
    virtual void setFormatNumber(const QString &ft) { setFormatType(ft); }
    virtual void setFormatType(const QString &formatType);
    /** @deprecated use getFormatType */
    virtual void setPrecision(int  _p);
    //font
    virtual void setTextFontBold( bool _b );
    virtual void setTextFontItalic( bool _b );
    virtual void setTextFontUnderline( bool _b );
    virtual void setTextFontStrike( bool _b );
    virtual void setTextFontSize( int _size );
    virtual void setTextFontFamily( const QString& _font );
    //border left
    virtual void setLeftBorderStyle( const QString& _style );
    virtual void setLeftBorderColor(const QString& _c);
    virtual void setLeftBorderColor(int r,int g,int b);
    virtual void setLeftBorderWidth( int _size );
    //border right
    virtual void setRightBorderStyle( const QString& _style );
    virtual void setRightBorderColor(const QString& _c);
    virtual void setRightBorderColor(int r,int g,int b);
    virtual void setRightBorderWidth( int _size );
    //border top
    virtual void setTopBorderStyle( const QString& _style );
    virtual void setTopBorderColor(const QString& _c);
    virtual void setTopBorderColor(int r,int g,int b);
    virtual void setTopBorderWidth( int _size );
    //border bottom
    virtual void setBottomBorderStyle( const QString& _style );
    virtual void setBottomBorderColor(const QString& _c);
    virtual void setBottomBorderColor(int r,int g,int b);
    virtual void setBottomBorderWidth( int _size );
    // fall back diagonal
    virtual void setFallDiagonalStyle( const QString& _style );
    virtual void setFallDiagonalColor(const QString& _c);
    virtual void setFallDiagonalColor(int r,int g,int b);
    virtual void setFallDiagonalWidth( int _size );
    //GoUpDiagonal
    virtual void setGoUpDiagonalStyle( const QString& _style );
    virtual void setGoUpDiagonalColor(const QString& _c);
    virtual void setGoUpDiagonalColor(int r,int g,int b);
    virtual void setGoUpDiagonalWidth( int _size );
    //indent
    virtual void setIndent( double indent );
    //don't print text
    virtual void setDontPrintText ( bool _b);
private:
    Format *layout;
};

} //namespace KSpread

#endif
