/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: PenBrush Command (header)                              */
/******************************************************************/

#ifndef penbrushcmd_h
#define penbrushcmd_h

#include <qlist.h>
#include <qpen.h>
#include <qbrush.h>

#include "command.h"
#include "kpobject.h"

class KPresenterDoc;

/******************************************************************/
/* Class: PenBrushCmd                                             */
/******************************************************************/

class PenBrushCmd : public Command
{
  Q_OBJECT

public:
  struct Pen
  {
    QPen pen;
    LineEnd lineBegin,lineEnd;
    
    Pen &operator=(Pen &_pen) {
      pen  = _pen.pen;
      lineBegin = _pen.lineBegin;
      lineEnd = _pen.lineEnd;
      return *this;
    }
  };

  struct Brush
  {
    QBrush brush;
    QColor gColor1;
    QColor gColor2;
    BCType gType;
    FillType fillType;

    Brush &operator=(Brush &_brush) {
      brush = _brush.brush;
      gColor1 = _brush.gColor1;
      gColor2 = _brush.gColor2;
      gType = _brush.gType;
      fillType = _brush.fillType;
      return *this;
    }
  };

  static const int LB_ONLY = 1;
  static const int LE_ONLY = 2;
  static const int PEN_ONLY = 4;
  static const int BRUSH_ONLY = 8;

  PenBrushCmd(QString _name,QList<Pen> &_oldPen,QList<Brush> &_oldBrush,
	      Pen _newPen,Brush _newBrush,QList<KPObject> &_objects,KPresenterDoc *_doc,int _flags = 0);
  ~PenBrushCmd();
  
  virtual void execute();
  virtual void unexecute();

protected:
  PenBrushCmd()
    {;}

  KPresenterDoc *doc;
  QList<Pen> oldPen;
  QList<Brush> oldBrush;
  QList<KPObject> objects;
  Pen newPen;
  Brush newBrush;
  int flags;

};

#endif
