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
/* Module: ellipse object (header)                                */
/******************************************************************/

#ifndef kpellipseobject_h
#define kpellipseobject_h

#include "kpobject.h"

/******************************************************************/
/* Class: KPEllipseObject                                         */
/******************************************************************/

class KPEllipseObject : public KPObject
{
  Q_OBJECT

public:
  KPEllipseObject();
  KPEllipseObject(QPen _pen,QBrush _brush);

  virtual void setPen(QPen _pen)
    { pen = _pen; }
  virtual void setBrush(QBrush _brush)
    { brush = _brush; }

  virtual ObjType getType()
    { return OT_ELLIPSE; }
  virtual QPen getPen()
    { return pen; } 
  virtual QBrush getBrush()
    { return brush; } 

  virtual void save(ostream& out);
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst);

  virtual void draw(QPainter *_painter,int _diffx,int _diffy);

protected:
  void paint(QPainter *_painter);

  QPen pen;
  QBrush brush;

};

#endif
