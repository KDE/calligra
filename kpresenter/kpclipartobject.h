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
/* Module: clipart object (header)                                */
/******************************************************************/

#ifndef kpclipartobject_h
#define kpclipartobject_h

#include "kpobject.h"
#include "kpclipart.h"
#include "kpgradient.h"

/******************************************************************/
/* Class: KPClipartObject                                         */
/******************************************************************/

class KPClipartObject : public KPObject
{
  Q_OBJECT

public:
  KPClipartObject();
  KPClipartObject(QString _filename);

  virtual void setFileName(QString _filename)
    { filename = _filename; clipart.setClipartName(filename); }
  virtual void setPen(QPen _pen)
    { pen = _pen; }
  virtual void setBrush(QBrush _brush)
    { brush = _brush; }
  virtual void setFillType(FillType _fillType);
  virtual void setGColor1(QColor _gColor1)
    { if (gradient) gradient->setColor1(_gColor1); gColor1 = _gColor1; redrawPix = true; }
  virtual void setGColor2(QColor _gColor2)
    { if (gradient) gradient->setColor2(_gColor2); gColor2 = _gColor2; redrawPix = true; }
  virtual void setGType(BCType _gType)
    { if (gradient) gradient->setBackColorType(_gType); gType = _gType; redrawPix = true; }

  virtual ObjType getType()
    { return OT_CLIPART; }
  virtual QString getFileName()
    { return filename; }
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

  virtual void save(ostream& out);
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst);

  virtual void draw(QPainter *_painter,int _diffx,int _diffy);

  virtual void setSize(int _width,int _height);
  virtual void resizeBy(int _dx,int _dy);

protected:
  QString filename;
  KPClipart clipart;

  QPen pen;
  QBrush brush;
  QColor gColor1,gColor2;
  BCType gType;
  FillType fillType;

  KPGradient *gradient;
  QPixmap pix;
  bool redrawPix;

};

#endif
