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
/* Module: Clipart (header)                                       */
/******************************************************************/
 
#ifndef kpclipart_h
#define kpclipart_h

#include <qobject.h>
#include <qstring.h>

#include "qwmf.h"
#include "global.h"

/******************************************************************/
/* class KPClipart                                                */
/******************************************************************/

class KPClipart : public QObject
{
  Q_OBJECT

public:
  KPClipart();
  KPClipart(QString _filename);
  ~KPClipart();

  void setClipartName(QString);
  QString getClipartName() {return filename;}
  QPicture* getPic();

protected:
  QPicture *pic;
  QString filename;
  QWinMetaFile wmf;

};

#endif
