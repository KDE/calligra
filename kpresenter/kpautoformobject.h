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
/* Module: autoform object (header)                               */
/******************************************************************/

#ifndef kpautoformobject_h
#define kpautoformobject_h

#include <qpntarry.h>
#include <qlist.h>

#include "kpobject.h"
#include "autoformEdit/atfinterpreter.h"
#include "kpresenter_utils.h"

#define RAD_FACTOR 180.0 / M_PI

/******************************************************************/
/* Class: KPAutoformObject                                        */
/******************************************************************/

class KPAutoformObject : public KPObject
{
  Q_OBJECT

public:
  KPAutoformObject();
  KPAutoformObject(QPen _pen,QBrush _brush,QString _filename,LineEnd _lineBegin,LineEnd _lineEnd);

  virtual void setPen(QPen _pen)
    { pen = _pen; }
  virtual void setBrush(QBrush _brush)
    { brush = _brush; }
  virtual void setFileName(QString _filename);
  virtual void setLineBegin(LineEnd _lineBegin)
    { lineBegin = _lineBegin; }
  virtual void setLineEnd(LineEnd _lineEnd)
    { lineEnd = _lineEnd; }

  virtual ObjType getType()
    { return OT_AUTOFORM; }
  virtual QPen getPen()
    { return pen; } 
  virtual QBrush getBrush()
    { return brush; } 
  virtual QString getFileName()
    { return filename; }
  virtual LineEnd getLineBegin()
    { return lineBegin; }
  virtual LineEnd getLineEnd()
    { return lineEnd; }

  virtual void save(ostream& out);
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst);

  virtual void draw(QPainter *_painter,int _diffx,int _diffy);

protected:
  float getAngle(QPoint p1,QPoint p2);
  void paint(QPainter *_painter);

  QPen pen;
  QBrush brush;
  QString filename;
  LineEnd lineBegin,lineEnd;

  ATFInterpreter atfInterp;

};

#endif
