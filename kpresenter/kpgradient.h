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
/* Module: Gradients (header)                                     */
/******************************************************************/

#ifndef kpgradient_h
#define kpgradient_h

#include <qobject.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qpainter.h>
#include <qimage.h>

#include <dither.h>

#include "global.h"

/******************************************************************/
/* Class: KPGradient                                              */
/******************************************************************/

class KPGradient : public QObject
{
  Q_OBJECT

public:
  KPGradient(QColor _color1,QColor _color2,BCType _bcType,QSize _size);
  ~KPGradient()
    {;}

  virtual QColor getColor1()
    { return color1; }
  virtual QColor getColor2()
    { return color2; }
  virtual BCType getBackColorType()
    { return bcType; }
 
  virtual void setColor1(QColor _color)
    { color1 = _color; paint(); }
  virtual void setColor2(QColor _color)
    { color2 = _color; paint(); }
  virtual void setBackColorType(BCType _type)
    { bcType = _type; paint(); }
 
  virtual QPixmap* getGradient()
    { return &pixmap; }
  virtual QSize getSize()
    { return pixmap.size(); }

  virtual void setSize(QSize _size)
    { pixmap.resize(_size); paint(); }

  virtual void addRef();
  virtual bool removeRef();

protected:
  KPGradient()
    {;}
  virtual void paint();

  QColor color1,color2;
  BCType bcType;
  
  QPixmap pixmap;
  int refCount;
};

#endif
