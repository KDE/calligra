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
    KPGradient( QColor _color1, QColor _color2, BCType _bcType, QSize _size,
		bool _unbalanced, int _xfactor, int _yfactor );
    virtual ~KPGradient()
    {}

    virtual QColor getColor1() const
    { return color1; }
    virtual QColor getColor2() const
    { return color2; }
    virtual BCType getBackColorType() const
    { return bcType; }
    virtual bool getUnbalanced() const
    { return unbalanced; }
    virtual int getXFactor() const
    { return xFactor; }
    virtual int getYFactor() const
    { return yFactor; }

    virtual void setColor1( QColor _color )
    { color1 = _color; paint(); }
    virtual void setColor2( QColor _color )
    { color2 = _color; paint(); }
    virtual void setBackColorType( BCType _type )
    { bcType = _type; paint(); }
    virtual void setUnbalanced( bool b )
    { unbalanced = b; paint(); }
    virtual void setXFactor( int i )
    { xFactor = i; paint(); }
    virtual void setYFactor( int i )
    { yFactor = i; paint(); }

    virtual QPixmap* getGradient() const
    { return (QPixmap*)&pixmap; }
    virtual QSize getSize() const
    { return pixmap.size(); }

    virtual void setSize( QSize _size ) {
	if ( _size != pixmap.size() ) {
	    pixmap.resize( _size );
	    paint();
	}
    }

    virtual void addRef();
    virtual bool removeRef();

protected:
    KPGradient()
    {; }
    virtual void paint();

    QColor color1, color2;
    BCType bcType;

    KPixmap pixmap;
    int refCount;

    bool unbalanced;
    int xFactor, yFactor;

};

#endif
