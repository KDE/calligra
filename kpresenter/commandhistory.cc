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
/* Module: Command History                                        */
/******************************************************************/

#include "commandhistory.h"
#include "commandhistory.moc"

/******************************************************************/
/* Class: CommandHistory                                          */
/******************************************************************/

/*====================== constructor =============================*/
CommandHistory::CommandHistory()
{
  commands.setAutoDelete(true);
  present = 0;
}

/*======================= destructor =============================*/
CommandHistory::~CommandHistory()
{
  commands.clear();
}

/*====================== add command =============================*/
void CommandHistory::addCommand(Command *_command)
{
  commands.insert(present++,_command);
  emit undoRedoChanged(getUndoName(),getRedoName());
  if (commands.count() > MAX_UNDO_REDO)
    {
      commands.remove(static_cast<unsigned int>(0));
      present--;
    }
}

/*======================= undo ===================================*/
void CommandHistory::undo()
{
  if (--present > -1)
    {
      commands.at(present)->unexecute();
      emit undoRedoChanged(getUndoName(),getRedoName());
    }
  else present++;
}

/*======================= redo ===================================*/
void CommandHistory::redo()
{
  if (present < static_cast<int>(commands.count()))
    {
      commands.at(present++)->execute();
      emit undoRedoChanged(getUndoName(),getRedoName());
    }
}

/*======================== get undo name ========================*/
QString CommandHistory::getUndoName()
{
  if (present - 1 > -1)
    return commands.at(present - 1)->getName();
  else
    return QString();
}

/*======================== get redo name ========================*/
QString CommandHistory::getRedoName()
{
  if (present < static_cast<int>(commands.count()))
    return commands.at(present)->getName();
  else
    return QString();
}
