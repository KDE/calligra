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

#include "CommandHistory.h"

#define MAX_HISTSIZE 100

CommandHistory::CommandHistory () {
  index = 0;
  history.setAutoDelete (true);
}

void CommandHistory::addCommand (Command *cmd, bool exec = false) {
  if (exec)
    cmd->execute ();
  // remove all command objects "behind" the current command
  for (unsigned int i = index; i <= history.count () - index; i++)
    history.remove ();

  history.append (cmd);
  if (history.count () > MAX_HISTSIZE)
    history.removeFirst ();
  else
    index++;
}

void CommandHistory::undo () {
  if (index > 0) {
    // get the last executed command
    Command* cmd = history.at (index - 1);
    // undo it
    cmd->unexecute ();
    // and update the history list
    index--;
  }
}

void CommandHistory::redo () {
  if (index < history.count ()) {
    Command* cmd = history.at (index);
    cmd->execute ();
    index++;
  }
}

void CommandHistory::reset () {
  history.clear ();
  index = 0;
}
