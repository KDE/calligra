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

#ifndef kptextobject_h
#define kptextobject_h

#include "kpobject.h"
#include "ktextedit.h"
#include "kpgradient.h"

class KPresenterView;
class KPresenterDoc;
class QDomElement;
class TextCmd;

/******************************************************************/
/* Class: KPTextObject                                            */
/******************************************************************/

class KPTextObject : public KPObject
{
    friend class TextCmd;
public:
    KPTextObject( KPresenterDoc *doc );
    virtual ~KPTextObject()
    {}

    virtual void setSize( int _width, int _height );
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
    virtual void setDrawEditRect( bool b )
    { drawEditRect = b; }
    virtual void setDrawEmpty( bool b )
    { drawEmpty = b; }
    virtual void setGUnbalanced( bool b )
    { if ( gradient ) gradient->setUnbalanced( b ); unbalanced = b; }
    virtual void setGXFactor( int f )
    { if ( gradient ) gradient->setXFactor( f ); xfactor = f; }
    virtual void setGYFactor( int f )
    { if ( gradient ) gradient->setYFactor( f ); yfactor = f; }

    virtual ObjType getType()
    { return OT_TEXT; }
    virtual int getSubPresSteps()
    { return ktextobject.paragraphs() - 1; }
    virtual KTextEdit* getKTextObject()
    { return &ktextobject; }
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
    virtual bool getDrawEditRect()
    { return drawEditRect; }
    virtual bool getDrawEmpty()
    { return drawEmpty; }
    virtual bool getGUnbalanced()
    { return unbalanced; }
    virtual int getGXFactor( )
    { return xfactor; }
    virtual int getGYFactor()
    { return yfactor; }

    virtual void save( QTextStream& out );
    virtual void load( KOMLParser& parser, QValueList<KOMLAttrib>& lst );

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

    virtual void activate( QWidget *_widget, int diffx, int diffy );
    virtual void deactivate( KPresenterDoc *doc );

    virtual void zoom( float _fakt );
    virtual void zoomOrig();

    virtual void extendObject2Contents( KPresenterView *view = 0 );

    void recalcPageNum( KPresenterDoc *doc );

protected:
    virtual void saveKTextObject( QTextStream& out );
    virtual void loadKTextObject( const QDomElement &e, int type );
    void drawParags( QPainter *p, int from, int to );

    KTextEdit ktextobject;

    QPen pen;
    QBrush brush;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    bool unbalanced;
    int xfactor, yfactor;

    KPGradient *gradient;
    bool drawEditRect, drawEmpty;

private:
    KPTextObject &operator=( const KPTextObject & );
};

#endif
