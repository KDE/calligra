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

#include <Command.h>
#include <GDocument.h>
#include "GPage.h"
#include <kdebug.h>

ObjectManipCmd::ObjectManipCmd (GDocument* doc, const QString& name) :
  Command(name)
{
  objects.resize (doc->activePage()->selectionCount ());
  states.resize (doc->activePage()->selectionCount ());

  QListIterator<GObject> it(doc->activePage()->getSelection());
  for (unsigned int i = 0; it.current(); ++it, ++i) {
    (*it)->ref ();
    kdDebug() << "1)MASK=" << (*it)->getFillInfo().mask << endl;
    kdDebug() << "1)COLOR=" << (*it)->getFillInfo().color.name() << endl;
    objects.insert (i, (*it));
    kdDebug() << "2)MASK=" << objects[i]->getFillInfo().mask << endl;
    kdDebug() << "2)COLOR=" << objects[i]->getFillInfo().color.name() << endl;
    states.insert (i, 0L);
  }
  document = doc;
}

ObjectManipCmd::ObjectManipCmd (GObject* obj, const QString& name) :
  Command(name)
{
  objects.resize (1);
  states.resize (1);

  obj->ref ();
  objects.insert (0, obj);
  states.insert (0, 0L);
  document = 0L;
}

ObjectManipCmd::~ObjectManipCmd () {
  for (unsigned int i = 0; i < objects.count (); i++) {
    objects[i]->unref ();
    if (states[i])
      states[i]->unref ();
  }
}

void ObjectManipCmd::execute () {
  for (unsigned int i = 0; i < objects.count (); i++) {
    if (states[i])
      states[i]->unref ();
    states.insert (i, objects[i]->saveState ());
  }
}

void ObjectManipCmd::unexecute () {
  if (document)
    document->activePage()->unselectAllObjects ();
  for (unsigned int i = 0; i < objects.count (); i++) {
    objects[i]->restoreState (states[i]);
    document->activePage()->selectObject (objects[i]);
  }
}

