/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
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

#include <qobject.h>
#include <qrect.h>
#include <qsize.h>
#include <qpoint.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qwmatrix.h>
#include <qcolor.h>

#include "global.h"

#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>
#include <kapp.h>

#include <iostream.h>
#include <fstream.h>
#include <math.h>

#include <string.h>

/******************************************************************/
/* Class: KPObject                                                */
/******************************************************************/

class KPObject : public QObject
{
  Q_OBJECT

public:
  KPObject();
  virtual ~KPObject()
    {;}

  virtual void setSelected(bool _selected)
    { selected = _selected; }
  virtual void rotate(float _angle)
    { angle = _angle; }
  virtual void setSize(QSize _size)
    { setSize(_size.width(),_size.height()); }
  virtual void setSize(int _width,int _height)
    { ext = QSize(_width > 20 ? _width : 20,_height > 20 ? _height : 20); }
  virtual void setOrig(QPoint _point)
    { orig = _point; }
  virtual void setOrig(int _x,int _y)
    { orig = QPoint(_x,_y); }
  virtual void moveBy(QPoint _point)
    { orig = orig + _point; }
  virtual void moveBy(int _dx,int _dy)
    { orig = orig + QPoint(_dx,_dy); }
  virtual void resizeBy(QSize _size)
    { resizeBy(_size.width(),_size.height()); }
  virtual void resizeBy(int _dx,int _dy)
    { ext = ext + QSize(_dx + ext.width() > 20 ? _dx : 0,_dy + ext.height() > 20 ? _dy : 0); }
  virtual void setShadowDistance(int _distance)
    { shadowDistance = _distance; }
  virtual void setShadowDirection(ShadowDirection _direction)
    { shadowDirection = _direction; }
  virtual void setShadowColor(QColor _color)
    { shadowColor = _color; }
  virtual void setEffect(Effect _effect)
    { effect = _effect; }
  virtual void setEffect2(Effect2 _effect2)
    { effect2 = _effect2; }
  virtual void setPresNum(int _presNum)
    { presNum = _presNum; }

  virtual void save(ostream& out) 
    {;}
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst) 
    {;}

  virtual ObjType getType()
    { return OT_UNDEFINED; }
  virtual QRect getBoundingRect(int _diffx,int _diffy);
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
  virtual QSize getSize()
    { return ext; }
  virtual QPoint getOrig()
    { return orig; }
  virtual Effect getEffect()
    { return effect; }
  virtual Effect2 getEffect2()
    { return effect2; } 
  virtual int getPresNum()
    { return presNum; }
  virtual int getSubPresSteps()
    { return 0; }

  virtual void drawSelection(bool _dSelection)
    { dSelection = _dSelection; }

  virtual void zoom(float _fakt);
  virtual void zoomOrig();
  virtual void setOwnClipping(bool _ownClipping)
    { ownClipping = _ownClipping; }
  virtual void setSubPresStep(int _subPresStep)
    { subPresStep = _subPresStep; }
  virtual void doSpecificEffects(int _specEffects)
    { specEffects = _specEffects; }

  virtual void draw(QPainter *_painter,int _diffx,int _diffy);

  virtual bool contains(QPoint _point,int _diffx,int _diffy);
  virtual QCursor getCursor(QPoint _point,int _diffx,int _diffy,ModifyType &_modType);

  virtual void activate(QWidget *_widget,int diffx,int diffy)
    {;}
  virtual void deactivate()
    {;}

protected:
  virtual void getShadowCoords(int& _x,int& _y,ShadowDirection _direction,int _distance);
  virtual void paintSelection(QPainter *_painter);

  float angle;
  QPoint orig;
  QSize ext;
  int shadowDistance;
  ShadowDirection shadowDirection;
  QColor shadowColor;
  Effect effect;
  Effect2 effect2;
  int presNum;

  bool selected,dSelection;
  bool zoomed;
  float presFakt;
  QPoint oldOrig;
  QSize oldExt;
  int subPresStep;
  bool specEffects;
  bool ownClipping;

};

#endif
