/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <CommandHistory.h>
#include <TranslateCmd.h>
#include <DuplicateCmd.h>

#include <kdebug.h>

#include <typeinfo>

using namespace std;

#define MAX_HISTSIZE 1000

CommandHistory::CommandHistory ()
{
   index = 0;
   history.setAutoDelete(true);
}

void CommandHistory::addCommand (Command *cmd, bool exec)
{
   if (exec)
      cmd->execute ();
   // remove all command objects "behind" the current command
   unsigned int num = history.count ();
   for (unsigned int i = index; i < num; i++)
      history.remove(index);

   // special treatment of translation of duplicated objects
   const type_info& type=typeid(*cmd);
   //if (cmd->isA ("TranslateCmd"))
   if (type==typeid(TranslateCmd))
   {
      //if (history.count () > 0 && history.getLast ()->isA ("DuplicateCmd"))
      if (history.count () > 0 && typeid(history.getLast ())==typeid(DuplicateCmd))
      {
         TranslateCmd* tcmd = (TranslateCmd *) cmd;
         DuplicateCmd::setRepetitionOffset (tcmd->xOffset (), tcmd->yOffset ());
      }
      else
         DuplicateCmd::resetRepetition ();
   }
   history.append (cmd);
   if (history.count () > MAX_HISTSIZE)
      history.removeFirst ();
   else
      index++;

   emit changed(isUndoPossible(), isRedoPossible());
}

void CommandHistory::undo ()
{
   if (index > 0)
   {
      // get the last executed command
      Command* cmd = history.at (index - 1);
      // undo it
      cmd->unexecute ();
      // and update the history list
      index--;
      emit changed(isUndoPossible(), isRedoPossible());
   }
}

void CommandHistory::redo ()
{
   if (index < history.count ())
   {
      Command* cmd = history.at (index);
      cmd->execute ();
      index++;
      emit changed(isUndoPossible(), isRedoPossible());
   }
}

void CommandHistory::reset ()
{
   history.clear ();
   index = 0;
   emit changed(false, false);
}

QString CommandHistory::getUndoName()
{
   if (index > 0)
   {
      Command* cmd = history.at (index - 1);
      return cmd->getName();
   }
   else
      return QString::null;
}

QString CommandHistory::getRedoName()
{
   if (index < history.count ())
      return history.at (index)->getName();
   else
      return QString::null;
}

void CommandHistory::dump ()
{
   QListIterator<Command> it (history);
   for (it += (index - 1); it.current (); --it)
      kdDebug(38000) << it.current ()->getName () << endl;
   kdDebug(38000) << "index = " << index << endl;
}

#include <CommandHistory.moc>
