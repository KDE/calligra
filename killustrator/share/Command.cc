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

#include "Command.h"

ObjectManipCmd::ObjectManipCmd (GDocument* doc, const QString& name) : 
  Command(name) 
{
  objects.resize (doc->selectionCount ());
  states.resize (doc->selectionCount ());

  list<GObject*>::iterator it = doc->getSelection ().begin ();
  for (unsigned int i = 0; it != doc->getSelection ().end (); it++, i++) {
    (*it)->ref ();
    objects.insert (i, (*it));
    states.insert (i, 0L);
  }
  document = doc;
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
  for (unsigned int i = 0; i < objects.count (); i++)
    objects[i]->restoreState (states[i]);
}
