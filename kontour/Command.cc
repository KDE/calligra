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

#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"

Command::Command(GDocument *aGDoc, const QString &name):
KCommand(name)
{
  mGDoc = aGDoc;
}

ObjectManipCmd::ObjectManipCmd(GDocument *aGDoc, const QString& name):
Command(aGDoc, name)
{
  objects.resize(document()->activePage()->selectionCount());
  states.resize (document()->activePage()->selectionCount());

  QPtrListIterator<GObject> it(document()->activePage()->getSelection());
  for(unsigned int i = 0; it.current(); ++it, ++i)
  {
    (*it)->ref();
    objects.insert(i, (*it));
    states.insert(i, 0L);
  }
}

ObjectManipCmd::ObjectManipCmd(GObject *obj, const QString &name):
Command(0L, name)
{
  objects.resize(1);
  states.resize(1);

  obj->ref();
  objects.insert(0, obj);
  states.insert(0, 0L);
}

ObjectManipCmd::~ObjectManipCmd()
{
/*  for(unsigned int i = 0; i < objects.count(); i++)
  {
    objects[i]->unref();
    if(states[i])
      states[i]->unref();
  }*/
}

void ObjectManipCmd::execute()
{
  for(unsigned int i = 0; i < objects.count(); i++)
  {
/*    if(states[i])
      states[i]->unref();*/
    states.insert(i, objects[i]->saveState());
  }
}

void ObjectManipCmd::unexecute()
{
  if(document())
    document()->activePage()->unselectAllObjects();
  for(unsigned int i = 0; i < objects.count(); i++)
  {
    objects[i]->restoreState(states[i]);
    document()->activePage()->selectObject(objects[i]);
  }
}
