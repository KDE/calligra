/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kpgradient_h
#define kpgradient_h

#include <qcolor.h>
#include <kpixmap.h>
#include <global.h>

class QPainter;

/******************************************************************/
/* Class: KPGradient						  */
/******************************************************************/

class KPGradient
{
public:
    KPGradient( const QColor &_color1, const QColor &_color2, BCType _bcType,
                const QSize &_size, bool _unbalanced, int _xfactor, int _yfactor );
    ~KPGradient() {}

    QColor getColor1() const { return color1; }
    QColor getColor2() const { return color2; }
    BCType getBackColorType() const { return bcType; }
    bool getUnbalanced() const { return unbalanced; }
    int getXFactor() const { return xFactor; }
    int getYFactor() const { return yFactor; }

    void setColor1( const QColor &_color ) { color1 = _color; paint(); }
    void setColor2( const QColor &_color ) { color2 = _color; paint(); }
    void setBackColorType( BCType _type ) { bcType = _type; paint(); }
    void setUnbalanced( bool b ) { unbalanced = b; paint(); }
    void setXFactor( int i ) { xFactor = i; paint(); }
    void setYFactor( int i ) { yFactor = i; paint(); }

    void init(const QColor &c1, const QColor &c2, BCType _type,
              bool _unbalanced, int xf, int yf);

    QPixmap* getGradient() const { return (QPixmap*)&pixmap; }
    QSize getSize() const { return pixmap.size(); }

    void setSize( const QSize &_size ) {
	if ( _size != pixmap.size() ) {
	    pixmap.resize( _size );
	    paint();
	}
    }

    void addRef();
    bool removeRef();

protected:
    KPGradient() {}
    void paint();

    QColor color1, color2;
    BCType bcType;

    KPixmap pixmap;
    int refCount;

    bool unbalanced;
    int xFactor, yFactor;
};

#endif
