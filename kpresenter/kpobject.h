/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Base class for objects (header)                        */
/******************************************************************/

#ifndef kpobject_h
#define kpobject_h

#include <krect.h>
#include <ksize.h>
#include <kpoint.h>
#include <qpen.h>
#include <qbrush.h>
#include <qcursor.h>
#include <qcolor.h>

#include "global.h"

#include <koStream.h>
#include <iostream.h>

#include "kpgradient.h"

class QPainter;

/******************************************************************/
/* Class: KPObject                                                */
/******************************************************************/

class KPObject
{
public:
	KPObject();

	virtual void setSelected( bool _selected )
    { selected = _selected; }
	virtual void rotate( float _angle )
    { angle = _angle; }
	virtual void setSize( KSize _size )
    { setSize( _size.width(), _size.height() ); }
	virtual void setSize( int _width, int _height )
    { ext = KSize( _width > 20 ? _width : 20, _height > 20 ? _height : 20 ); }
	virtual void setOrig( KPoint _point )
    { orig = _point; }
	virtual void setOrig( int _x, int _y )
    { orig = KPoint( _x, _y ); }
	virtual void moveBy( KPoint _point )
    { orig = orig + _point; }
	virtual void moveBy( int _dx, int _dy )
    { orig = orig + KPoint( _dx, _dy ); }
	virtual void resizeBy( KSize _size )
    { resizeBy( _size.width(), _size.height() ); }
	virtual void resizeBy( int _dx, int _dy )
    { ext = ext + KSize( _dx + ext.width() > 20 ? _dx : 0, _dy + ext.height() > 20 ? _dy : 0 ); }
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

	virtual void save( ostream& out )
    {; }
	virtual void load( KOMLParser& parser, vector<KOMLAttrib>& lst )
    {; }

	virtual ObjType getType()
    { return OT_UNDEFINED; }
	virtual KRect getBoundingRect( int _diffx, int _diffy );
	virtual bool isSelected()
    { return selected; }
	virtual float getAngle()
    { return angle; }
	virtual int getShadowDistance()
    { return shadowDistance; }
	virtual ShadowDirection getShadowDirection()
    { return shadowDirection; }
	virtual QColor getShadowColor()
    { return shadowColor; }
	virtual KSize getSize()
    { return ext; }
	virtual KPoint getOrig()
    { return orig; }
	virtual Effect getEffect()
    { return effect; }
	virtual Effect2 getEffect2()
    { return effect2; }
	virtual int getPresNum()
    { return presNum; }
	virtual int getSubPresSteps()
    { return 0; }

	virtual void drawSelection( bool _dSelection )
    { dSelection = _dSelection; }

	virtual void zoom( float _fakt );
	virtual void zoomOrig();
	virtual void setOwnClipping( bool _ownClipping )
    { ownClipping = _ownClipping; }
	virtual void setSubPresStep( int _subPresStep )
    { subPresStep = _subPresStep; }
	virtual void doSpecificEffects( bool _specEffects, bool _onlyCurrStep = true )
    { specEffects = _specEffects; onlyCurrStep = _onlyCurrStep; }

	virtual void draw( QPainter *_painter, int _diffx, int _diffy );

	virtual bool contains( KPoint _point, int _diffx, int _diffy );
	virtual QCursor getCursor( KPoint _point, int _diffx, int _diffy, ModifyType &_modType );

	virtual void activate( QWidget *_widget, int diffx, int diffy )
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

protected:
	virtual void getShadowCoords( int& _x, int& _y, ShadowDirection _direction, int _distance );
	virtual void paintSelection( QPainter *_painter );
	virtual void doDelete();

	float angle;
	KPoint orig;
	KSize ext;
	int shadowDistance;
	ShadowDirection shadowDirection;
	QColor shadowColor;
	Effect effect;
	Effect2 effect2;
	int presNum;

	bool selected, dSelection;
	bool zoomed;
	float presFakt;
	KPoint oldOrig;
	KSize oldExt;
	int subPresStep;
	bool specEffects;
	bool onlyCurrStep;
	bool ownClipping;

	bool inObjList;
	int cmds;
	bool move;

};

#endif
