/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Move Command (header)                                  */
/******************************************************************/

#ifndef movecmd_h
#define movecmd_h

#include <qpoint.h>
#include <qlist.h>
#include <qrect.h>

#include "command.h"
#include "kpobject.h"

class KPresenterDocument_impl;

/******************************************************************/
/* Class: MoveByCmd                                               */
/******************************************************************/

class MoveByCmd : public Command
{
  Q_OBJECT

public:
  MoveByCmd(QString _name,QPoint _diff,QList<KPObject> _objects,KPresenterDocument_impl *_doc);
  ~MoveByCmd();
  
  virtual void execute();
  virtual void unexecute();

protected:
  MoveByCmd()
    {;}

  QPoint diff;
  QList<KPObject> objects;
  KPresenterDocument_impl *doc;

};

#endif
