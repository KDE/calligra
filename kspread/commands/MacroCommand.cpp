/* This file is part of the KDE project
   Copyright 2005,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <klocale.h>

#include "Doc.h"
#include "Sheet.h"

#include "MacroCommand.h"

using namespace KSpread;

void MacroCommand::redo ()
{
  QList<AbstractRegionCommand *>::iterator it;
  for (it = manipulators.begin(); it != manipulators.end(); ++it)
    (*it)->redo ();
  // add me to undo if needed - same as in AbstractRegionCommand::redo
  if (m_firstrun && m_register)
  {
    m_register = false;
    m_sheet->doc()->addCommand (this);
  }
  m_firstrun = false;
}

void MacroCommand::undo ()
{
  QList<AbstractRegionCommand *>::iterator it;
  for (it = manipulators.begin(); it != manipulators.end(); ++it)
    (*it)->undo ();
}

void MacroCommand::add (AbstractRegionCommand *manipulator)
{
  manipulator->setRegisterUndo (false);
  manipulators.push_back (manipulator);
}
