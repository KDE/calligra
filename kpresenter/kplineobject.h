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
/* Module: line object (header)                                   */
/******************************************************************/

#ifndef kplineobject_h
#define kplineobject_h

#include "kpobject.h"
#include "kpresenter_utils.h"

#define RAD_FACTOR 180.0 / M_PI

/******************************************************************/
/* Class: KPLineObject                                            */
/******************************************************************/

class KPLineObject : public KPObject
{
  Q_OBJECT

public:
  KPLineObject();
  KPLineObject(QPen _pen,LineEnd _lineBegin,LineEnd _lineEnd,LineType _lineType);

  virtual void setPen(QPen _pen)
    { pen = _pen; }
  virtual void setLineBegin(LineEnd _lineBegin)
    { lineBegin = _lineBegin; }
  virtual void setLineEnd(LineEnd _lineEnd)
    { lineEnd = _lineEnd; }
  virtual void setLineType(LineType _lineType)
    { lineType = _lineType; }

  virtual ObjType getType()
    { return OT_LINE; }
  virtual QPen getPen()
    { return pen; } 
  virtual LineEnd getLineBegin()
    { return lineBegin; }
  virtual LineEnd getLineEnd()
    { return lineEnd; }
  virtual LineType getLineType()
    { return lineType; }

  virtual void save(ostream& out);
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst);

  virtual void draw(QPainter *_painter,int _diffx,int _diffy);

protected:
  float getAngle(KPoint p1,QPoint p2);
  void paint(QPainter *_painter);

  QPen pen;
  LineEnd lineBegin,lineEnd;
  LineType lineType;

};

#endif
