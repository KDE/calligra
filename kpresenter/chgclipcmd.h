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
/* Module: Change Clipart (header)                                */
/******************************************************************/

#ifndef chgclipcmd_h
#define chgclipcmd_h

#include <qstring.h>

#include "command.h"
#include "kpclipartobject.h"

class KPresenterDocument_impl;

/******************************************************************/
/* Class: ChgClipCmd                                              */
/******************************************************************/

class ChgClipCmd : public Command
{
  Q_OBJECT

public:
  ChgClipCmd(QString _name,KPClipartObject *_object,QString _oldName,QString _newName,KPresenterDocument_impl *_doc);
  ~ChgClipCmd();
  
  virtual void execute();
  virtual void unexecute();

protected:
  ChgClipCmd()
    {;}

  KPClipartObject *object;
  KPresenterDocument_impl *doc;
  QString oldName,newName;

};

#endif
