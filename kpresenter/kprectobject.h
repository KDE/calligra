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

#ifndef kprectobject_h
#define kprectobject_h

#include <kpobject.h>

class KPGradient;

/******************************************************************/
/* Class: KPRectObject                                            */
/******************************************************************/

class KPRectObject : public KPObject
{
public:
    KPRectObject();
    KPRectObject( QPen _pen, QBrush _brush, FillType _fillType,
                  QColor _gColor1, QColor _gColor2, BCType _gType, int _xRnd, int _yRnd,
                  bool _unbalanced, int _xfactor, int _yfactor );
    virtual ~KPRectObject()
    { if ( gradient ) delete gradient; }

    KPRectObject &operator=( const KPRectObject & );

    virtual void setSize( int _width, int _height );
    virtual void resizeBy( int _dx, int _dy );

    virtual void setPen( QPen _pen )
    { pen = _pen; }
    virtual void setBrush( QBrush _brush )
    { brush = _brush; }
    virtual void setRnds( int _xRnd, int _yRnd )
    { xRnd = _xRnd; yRnd = _yRnd; }
    virtual void setFillType( FillType _fillType );
    virtual void setGColor1( QColor _gColor1 )
    { if ( gradient ) gradient->setColor1( _gColor1 ); gColor1 = _gColor1; }
    virtual void setGColor2( QColor _gColor2 )
    { if ( gradient ) gradient->setColor2( _gColor2 ); gColor2 = _gColor2; }
    virtual void setGType( BCType _gType )
    { if ( gradient ) gradient->setBackColorType( _gType ); gType = _gType; }
    virtual void setGUnbalanced( bool b )
    { if ( gradient ) gradient->setUnbalanced( b ); unbalanced = b; }
    virtual void setGXFactor( int f )
    { if ( gradient ) gradient->setXFactor( f ); xfactor = f; }
    virtual void setGYFactor( int f )
    { if ( gradient ) gradient->setYFactor( f ); yfactor = f; }

    virtual ObjType getType() const
    { return OT_RECT; }
    virtual QPen getPen() const
    { return pen; }
    virtual QBrush getBrush() const
    { return brush; }
    virtual void getRnds( int &_xRnd, int &_yRnd ) const
    { _xRnd = xRnd; _yRnd = yRnd; }
    virtual FillType getFillType() const
    { return fillType; }
    virtual QColor getGColor1() const
    { return gColor1; }
    virtual QColor getGColor2() const
    { return gColor2; }
    virtual BCType getGType() const
    { return gType; }
    virtual bool getGUnbalanced() const
    { return unbalanced; }
    virtual int getGXFactor() const
    { return xfactor; }
    virtual int getGYFactor() const
    { return yfactor; }

    virtual QDomDocumentFragment save( QDomDocument& doc );
    virtual void load( KOMLParser& parser, QValueList<KOMLAttrib>& lst );

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

protected:
    void paint( QPainter *_painter );

    QPen pen;
    QBrush brush;
    int xRnd, yRnd;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    bool unbalanced;
    int xfactor, yfactor;

    KPGradient *gradient;
    bool drawShadow;

};

#endif
