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

  virtual ObjType getType()
    { return OT_CLIPART; }
  virtual QString getFileName()
    { return filename; }

  virtual void save(ostream& out);
  virtual void load(KOMLParser& parser,vector<KOMLAttrib>& lst);

  virtual void draw(QPainter *_painter,int _diffx,int _diffy);

protected:
  QString filename;
  KPClipart clipart;

};

#endif
