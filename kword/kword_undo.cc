/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Undo                                                   */
/******************************************************************/

#include "kword_undo.h"
#include "kword_undo.moc"

/******************************************************************/
/* Class: KWTextChangeCommand                                     */
/******************************************************************/

/*================================================================*/
void KWTextChangeCommand::execute()
{
}

/*================================================================*/
void KWTextChangeCommand::unexecute()
{
}

/******************************************************************/
/* Class: KWCommandHistory                                        */
/******************************************************************/

/*================================================================*/
KWCommandHistory::KWCommandHistory()
  : current(-1)
{
  history.setAutoDelete(true);
}

/*================================================================*/
void KWCommandHistory::addCommand(KWCommand *_command)
{
  if (current < static_cast<int>(history.count()))
    {
      QList<KWCommand> _commands;
      _commands.setAutoDelete(false);
  
      for (int i = 0;i < current;i++)
	{    
	  _commands.insert(i,history.at(0));
	  history.take(0);
	}
      
      _commands.append(_command);
      history.clear();
      history = _commands;
      history.setAutoDelete(true);
    }
  else
    history.append(_command);

  if (history.count() > MAX_UNDO_REDO)
    history.removeFirst();
  else
    current++;

  emit undoRedoChanged(getUndoName(),getRedoName());
}

/*================================================================*/
void KWCommandHistory::undo()
{
  if (current > 0)
    {
      history.at(current - 1)->unexecute();
      current--;
      emit undoRedoChanged(getUndoName(),getRedoName());
    }
  
}

/*================================================================*/
void KWCommandHistory::redo()
{
  if (current < static_cast<int>(history.count()))
    {
      history.at(current)->execute();
      current++;
      emit undoRedoChanged(getUndoName(),getRedoName());
    }
}

/*================================================================*/
QString KWCommandHistory::getUndoName()
{
  if (current > 0)
    return history.at(current - 1)->getName();
  else
    return QString();
}

/*================================================================*/
QString KWCommandHistory::getRedoName()
{
  if (current < static_cast<int>(history.count()))
    return history.at(current)->getName();
  else
    return QString();
}
