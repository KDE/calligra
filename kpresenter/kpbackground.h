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
/* Module: Background (header)                                    */
/******************************************************************/

#ifndef kpbackground_h
#define kpbackground_h

#include <qobject.h>
#include <qpicture.h>
#include <qcolor.h>
#include <qstring.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qrect.h>
#include <qsize.h>
#include <qpixmap.h>

#include "qwmf.h"
#include "global.h"
#include "kppixmapcollection.h"
#include "kpclipart.h"
#include "kpgradientcollection.h"

#include <komlParser.h>
#include <komlStreamFeed.h>
#include <komlWriter.h>
#include <kapp.h>

#include <iostream.h>
#include <fstream.h>

#include <string.h>

/******************************************************************/
/* Class: KPBackGround                                            */
/******************************************************************/

class KPBackGround : public QObject
{
  Q_OBJECT

public:
  KPBackGround(KPPixmapCollection *_pixmapCollection,KPGradientCollection *_gradientCollection);
  virtual ~KPBackGround()
    {;}

  virtual void setBackType(BackType _backType)
    { backType = _backType; }
  virtual void setBackView(BackView _backView)
    { backView = _backView; }
  virtual void setBackColor1(QColor _color)
    { removeGradient(); backColor1 = _color; }
  virtual void setBackColor2(QColor _color)
    { removeGradient(); backColor2 = _color; }
  virtual void setBackColorType(BCType _bcType)
    { removeGradient(); bcType = _bcType; }
  virtual void setBackPixFilename(QString _filename);
  virtual void setBackPix(QString _filename,QString _data);
  virtual void setBackClipFilename(QString _filename);
  virtual void setPageEffect(PageEffect _pageEffect)
    { pageEffect = _pageEffect; }

  virtual void setSize(QSize _size)
    { removeGradient(); ext = _size; }
  virtual void setSize(int _width,int _height)
    { removeGradient(); ext = QSize(_width,_height); }

  virtual BackType getBackType()
    { return backType; }
  virtual BackView getBackView()
    { return backView; }
  virtual QColor getBackColor1()
    { return backColor1; }
  virtual QColor getBackColor2()
    { return backColor2; }
  virtual BCType getBackColorType()
    { return bcType; }
  virtual QString getBackPixFilename()
    { return backPixFilename; }
  virtual QString getBackClipFilename()
    { return backClipFilename; }
  virtual PageEffect getPageEffect()
    { return pageEffect; }

  virtual QSize getSize()
    { return ext; }

  virtual void draw(QPainter *_painter,QPoint _offset,bool _drawBorders);

  virtual void restore();

  virtual void save(ostream& out); 
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst); 

protected:
  virtual void drawBackColor(QPainter *_painter);
  virtual void drawBackPix(QPainter *_painter);
  virtual void drawBackClip(QPainter *_painter);
  virtual void drawBorders(QPainter *_painter);
  virtual void removeGradient();

  BackType backType; 
  BackView backView; 
  QColor backColor1;
  QColor backColor2;
  BCType bcType;
  QString backPixFilename;
  QString backClipFilename; 
  PageEffect pageEffect;
  
  QPixmap *backPix;
  KPPixmapCollection *pixmapCollection;
  KPGradientCollection *gradientCollection;
  QPixmap *gradient;
  KPClipart backClip;

  QSize ext;
  QSize pixSize;
  QString data;

};

#endif
