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
/* Module: Rect Value Command (header)                            */
/******************************************************************/

#ifndef rectvaluecmd_h
#define rectvaluecmd_h

#include <qlist.h>
#include <qcolor.h>

#include "command.h"
#include "kpobject.h"
#include "kppieobject.h"

class KPresenterDoc;

/******************************************************************/
/* Class: RectValueCmd                                            */
/******************************************************************/

class RectValueCmd : public Command
{
  Q_OBJECT

public:
  struct RectValues
  {
    int xRnd,yRnd;
  };

  RectValueCmd(QString _name,QList<RectValues> &_oldValues,RectValues _newValues,
	       QList<KPObject> &_objects,KPresenterDoc *_doc);
  ~RectValueCmd();
  
  virtual void execute();
  virtual void unexecute();

protected:
  RectValueCmd()
    {;}

  KPresenterDoc *doc;
  QList<RectValues> oldValues;
  QList<KPObject> objects;
  RectValues newValues;

};

#endif
