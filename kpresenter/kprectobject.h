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
/* Module: rect object (header)                                   */
/******************************************************************/

#ifndef kprectobject_h
#define kprectobject_h

#include "kpobject.h"

/******************************************************************/
/* Class: KPRectObject                                            */
/******************************************************************/

class KPRectObject : public KPObject
{
  Q_OBJECT

public:
  KPRectObject();
  KPRectObject(QPen _pen,QBrush _brush,RectType _rectType,int _xRnd,int _yRnd);

  virtual void setPen(QPen _pen)
    { pen = _pen; }
  virtual void setBrush(QBrush _brush)
    { brush = _brush; }
  virtual void setRectType(RectType _rectType)
    { rectType = _rectType; }
  virtual void setRnds(int _xRnd,int _yRnd)
    { xRnd = _xRnd; yRnd = _yRnd; }

  virtual ObjType getType()
    { return OT_RECT; }
  virtual QPen getPen()
    { return pen; } 
  virtual QBrush getBrush()
    { return brush; } 
  virtual RectType getRectType()
    { return rectType; }
  virtual void getRnds(int &_xRnd,int &_yRnd)
    { _xRnd = xRnd; _yRnd = yRnd; }

  virtual void save(ostream& out);
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst);

  virtual void draw(QPainter *_painter,int _diffx,int _diffy);

protected:
  void paint(QPainter *_painter);

  QPen pen;
  QBrush brush;
  RectType rectType;
  int xRnd,yRnd;

};

#endif
