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
/* Module: Pie Value Command (header)                             */
/******************************************************************/

#ifndef pievaluecmd_h
#define pievaluecmd_h

#include <qlist.h>
#include <qcolor.h>

#include "command.h"
#include "kpobject.h"
#include "kppieobject.h"

class KPresenterDoc;

/******************************************************************/
/* Class: PieValueCmd                                             */
/******************************************************************/

class PieValueCmd : public Command
{
  Q_OBJECT

public:
  struct PieValues
  {
    PieType pieType;
    int pieAngle,pieLength;
  };

  PieValueCmd(QString _name,QList<PieValues> &_oldValues,PieValues _newValues,
	    QList<KPObject> &_objects,KPresenterDoc *_doc);
  ~PieValueCmd();
  
  virtual void execute();
  virtual void unexecute();

protected:
  PieValueCmd()
    {;}

  KPresenterDoc *doc;
  QList<PieValues> oldValues;
  QList<KPObject> objects;
  PieValues newValues;

};

#endif
