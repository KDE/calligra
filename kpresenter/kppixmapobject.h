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
/* Module: pixmap object (header)                                 */
/******************************************************************/

#ifndef kppixmapobject_h
#define kppixmapobject_h

#include <qpixmap.h>

#include "kpobject.h"
#include "kppixmapcollection.h"

/******************************************************************/
/* Class: KPPixmapObject                                          */
/******************************************************************/

class KPPixmapObject : public KPObject
{
  Q_OBJECT

public:
  KPPixmapObject(KPPixmapCollection *_pixmapCollection);
  KPPixmapObject(KPPixmapCollection *_pixmapCollection,QString _filename);

  virtual void setSize(int _width,int _height);
  virtual void setSize(QSize _size)
    { setSize(_size.width(),_size.height()); }
  virtual void resizeBy(int _dx,int _dy);

  virtual void setFileName(QString _filename);
  virtual void setPixmap(QString _filename,QString _data);

  virtual ObjType getType()
    { return OT_PICTURE; }
  virtual QString getFileName()
    { return filename; }

  virtual void save(ostream& out);
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst);

  virtual void draw(QPainter *_painter,int _diffx,int _diffy);

protected:
  KPPixmapObject()
    {;}
  
  QString filename;
  KPPixmapCollection *pixmapCollection;
  QPixmap *pixmap;
  QSize pixSize;
  QString data;

};

#endif
