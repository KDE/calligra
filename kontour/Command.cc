/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#include "Command.h"

#include <kdebug.h>

#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"

Command::Command(GDocument *aGDoc, const QString &name):
KCommand(name)
{
  mGDoc = aGDoc;
}

TransformationCmd::TransformationCmd(GDocument *aGDoc, const QString& name):
Command(aGDoc, name)
{
  objects.resize(document()->activePage()->selectionCount());
  states.resize(document()->activePage()->selectionCount());
  QPtrListIterator<GObject> it(document()->activePage()->getSelection());
  for(unsigned int i = 0; it.current(); ++it, ++i)
  {
    (*it)->ref();
    objects.insert(i, (*it));
    states[i] = (*it)->matrix();
  }
}

TransformationCmd::~TransformationCmd()
{
  for(unsigned int i = 0; i < objects.count(); i++)
    objects[i]->unref();
}

void TransformationCmd::execute()
{
}

void TransformationCmd::unexecute()
{
  document()->activePage()->unselectAllObjects();
  for(unsigned int i = 0; i < objects.count(); i++)
  {
    objects[i]->matrix(states[i]);
    document()->activePage()->selectObject(objects[i]);
  }
  document()->activePage()->updateSelection();
}
