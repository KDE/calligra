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

#ifndef kppartobject_h
#define kppartobject_h

#include <koMainWindow.h>

#include "kpobject.h"

#include <qpixmap.h>

class KPresenterChild;
class KPresenterFrame;
class KPGradient;

/******************************************************************/
/* Class: KPPartObject                                            */
/******************************************************************/

class KPPartObject : public KPObject
{
public:
    KPPartObject( KPresenterChild *_child );
    virtual ~KPPartObject();

    KPPartObject &operator=( const KPPartObject & );

    virtual void save( QTextStream& out );
    virtual void load( KOMLParser& parser, QValueList<KOMLAttrib>& lst );

    virtual void rotate( float _angle );
    virtual void setSize( int _width, int _height );
    virtual void setOrig( QPoint _point );
    virtual void setOrig( int _x, int _y );
    virtual void moveBy( QPoint _point );
    virtual void moveBy( int _dx, int _dy );
    virtual void resizeBy( int _dx, int _dy );

    virtual void setPen( QPen _pen )
    { pen = _pen; }
    virtual void setBrush( QBrush _brush )
    { brush = _brush; }
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

    virtual ObjType getType()
    { return OT_PART; }
    virtual QPen getPen()
    { return pen; }
    virtual QBrush getBrush()
    { return brush; }
    virtual FillType getFillType()
    { return fillType; }
    virtual QColor getGColor1()
    { return gColor1; }
    virtual QColor getGColor2()
    { return gColor2; }
    virtual BCType getGType()
    { return gType; }
    virtual bool getGUnbalanced()
    { return unbalanced; }
    virtual int getGXFactor( )
    { return xfactor; }
    virtual int getGYFactor()
    { return yfactor; }

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

    virtual void activate( QWidget *_widget, int diffx, int diffy );
    virtual void deactivate();

    KPresenterChild *getChild() { return child; }

    void enableDrawing( bool f ) { _enableDrawing = f; }

protected:
    void paint( QPainter *_painter );

    QPen pen;
    QBrush brush;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    bool unbalanced;
    int xfactor, yfactor;

    KPGradient *gradient;
    bool _enableDrawing;

    KPresenterChild *child;

};

#endif
