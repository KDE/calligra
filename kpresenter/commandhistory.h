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
/* Module: Command History (header)                               */
/******************************************************************/

#ifndef commandhistory_h
#define commandhistory_h

#include <qobject.h>
#include <qstring.h>
#include <qlist.h>

#include "command.h"
#include "global.h"

/******************************************************************/
/* Class: CommandHistory                                          */
/******************************************************************/

class CommandHistory : public QObject
{
  Q_OBJECT

public:
  CommandHistory();
  virtual ~CommandHistory();

  virtual void addCommand(Command *_command);

  virtual void undo();
  virtual void redo();

  virtual QString getUndoName();
  virtual QString getRedoName();

signals:
  void undoRedoChanged(QString,QString);

protected:
  QList<Command> commands;
  int present;

};

#endif
