/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: pixmap collection (header)                             */
/******************************************************************/

#ifndef kppixmapcollection_h
#define kppixmapcollection_h

#include <qobject.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qlist.h>

#include "kppixmap.h"

/******************************************************************/
/* Class: KPPixmapCollection                                      */
/******************************************************************/

class KPPixmapCollection : public QObject
{
  Q_OBJECT

public:
  KPPixmapCollection()
    { pixmapList.setAutoDelete(true); }

  ~KPPixmapCollection()
    { pixmapList.clear(); }

  virtual QPixmap* getPixmap(QString _filename,QSize _size,QString &_data,bool orig = false,bool addref = true);
  virtual QPixmap* getPixmap(QString _filename,QString _data,QSize _size,bool orig = false,bool addref = true);
  virtual QPixmap* getPixmap(QString _filename,QString _data,QPixmap *_pixmap,QSize _size,bool orig = false,bool addref = true);

  virtual void removeRef(QString _filename,QSize _size);
  virtual void removeRef(QString _filename,QString _data,QSize _size);

protected:
  virtual int inPixmapList(QString _filename,QSize _size);
  virtual int inPixmapList(QString _filename,QString _data,QSize _size);

  QList<KPPixmap> pixmapList;

};

#endif
