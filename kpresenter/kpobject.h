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

#ifndef kpobject_h
#define kpobject_h

#include <qrect.h>
#include <qsize.h>
#include <qpoint.h>
#include <qpen.h>
#include <qbrush.h>
#include <qcursor.h>
#include <qcolor.h>

#include "global.h"

#include <koStream.h>
#include <qtextstream.h>

#include "kpgradient.h"

class QPainter;
class DCOPObject;

/******************************************************************/
/* Class: KPObject                                                */
/******************************************************************/

class KPObject
{
public:
    KPObject();
    virtual ~KPObject();

    KPObject &operator=( const KPObject & );

    virtual DCOPObject* dcopObject();

    virtual void setSelected( bool _selected )
    { selected = _selected; }
    virtual void rotate( float _angle )
    { angle = _angle; }
    virtual void setSize( QSize _size )
    { setSize( _size.width(), _size.height() ); }
    virtual void setSize( int _width, int _height )
    { ext = QSize( _width > 20 ? _width : 20, _height > 20 ? _height : 20 ); }
    virtual void setOrig( QPoint _point )
    { orig = _point; }
    virtual void setOrig( int _x, int _y )
    { orig = QPoint( _x, _y ); }
    virtual void moveBy( QPoint _point )
    { orig = orig + _point; }
    virtual void moveBy( int _dx, int _dy )
    { orig = orig + QPoint( _dx, _dy ); }
    virtual void resizeBy( QSize _size )
    { resizeBy( _size.width(), _size.height() ); }
    virtual void resizeBy( int _dx, int _dy )
    { ext = ext + QSize( _dx + ext.width() > 20 ? _dx : 0, _dy + ext.height() > 20 ? _dy : 0 ); }
    virtual void setShadowDistance( int _distance )
    { shadowDistance = _distance; }
    virtual void setShadowDirection( ShadowDirection _direction )
    { shadowDirection = _direction; }
    virtual void setShadowColor( QColor _color )
    { shadowColor = _color; }
    virtual void setEffect( Effect _effect )
    { effect = _effect; }
    virtual void setEffect2( Effect2 _effect2 )
    { effect2 = _effect2; }
    virtual void setPresNum( int _presNum )
    { presNum = _presNum; }
    virtual void setDisappear( bool b )
    { disappear = b; }
    virtual void setDisappearNum( int num )
    { disappearNum = num; }
    virtual void setEffect3( Effect3 _effect3)
    { effect3 = _effect3; }

    virtual void save( QTextStream& /*out*/ )
    {; }
    virtual void load( KOMLParser& /*parser*/, QValueList<KOMLAttrib>& /*lst*/ )
    {; }

    virtual ObjType getType() const
    { return OT_UNDEFINED; }
    virtual QRect getBoundingRect( int _diffx, int _diffy ) const;
    virtual bool isSelected() const
    { return selected; }
    virtual float getAngle() const
    { return angle; }
    virtual int getShadowDistance() const
    { return shadowDistance; }
    virtual ShadowDirection getShadowDirection() const
    { return shadowDirection; }
    virtual QColor getShadowColor() const
    { return shadowColor; }
    virtual QSize getSize() const
    { return ext; }
    virtual QPoint getOrig() const
    { return orig; }
    virtual Effect getEffect() const
    { return effect; }
    virtual Effect2 getEffect2() const
    { return effect2; }
    virtual Effect3 getEffect3() const
    { return effect3; }
    virtual int getPresNum() const
    { return presNum; }
    virtual int getSubPresSteps() const
    { return 0; }
    virtual bool getDisappear() const
    { return disappear; }
    virtual int getDisappearNum() const
    { return disappearNum; }

    virtual void drawSelection( bool _dSelection )
    { dSelection = _dSelection; }

    virtual void zoom( float _fakt );
    virtual void zoomOrig();
    virtual bool isZoomed() const { return zoomed; }
    virtual void setOwnClipping( bool _ownClipping )
    { ownClipping = _ownClipping; }
    virtual void setSubPresStep( int _subPresStep )
    { subPresStep = _subPresStep; }
    virtual void doSpecificEffects( bool _specEffects, bool _onlyCurrStep = true )
    { specEffects = _specEffects; onlyCurrStep = _onlyCurrStep; }

    virtual void draw( QPainter *_painter, int _diffx, int _diffy );

    virtual bool contains( QPoint _point, int _diffx, int _diffy ) const;
    virtual bool intersects( QRect _rect, int _diffx, int _diffy ) const;
    virtual QCursor getCursor( QPoint _point, int _diffx, int _diffy, ModifyType &_modType ) const;

    virtual void activate( QWidget * /*_widget*/, int /*diffx*/, int /*diffy*/ )
    {; }
    virtual void deactivate()
    {; }

    virtual void removeFromObjList()
    { inObjList = false; doDelete(); }
    virtual void addToObjList()
    { inObjList = true; }
    virtual void incCmdRef()
    { cmds++; }
    virtual void decCmdRef()
    { cmds--; doDelete(); }

    virtual void setMove( bool _move )
    { move = _move; }

    virtual void setSticky( bool b ) { sticky = b; }
    bool isSticky() const { return sticky; }

    static void setupClipRegion( QPainter *painter, const QRegion &clipRegion );

protected:
    void getShadowCoords( int& _x, int& _y, ShadowDirection _direction, int _distance ) const;
    virtual void paintSelection( QPainter *_painter );
    virtual void doDelete();

    float angle;
    QPoint orig;
    QSize ext;
    int shadowDistance;
    ShadowDirection shadowDirection;
    QColor shadowColor;
    Effect effect;
    Effect2 effect2;
    Effect3 effect3;
    int presNum, disappearNum;
    bool disappear:1;

    bool selected:1;
    bool dSelection:1;
    bool zoomed:1;

    float presFakt;
    QPoint oldOrig;
    QSize oldExt;
    int subPresStep;

    bool specEffects:1;
    bool onlyCurrStep:1;
    bool ownClipping:1;

    bool inObjList:1;
    bool move:1;
    bool sticky:1;
    int cmds;

    DCOPObject *dcop;

};

#endif
