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

#include "GroupCmd.h"

GroupCmd::GroupCmd (GDocument* doc) : Command(i18n("Group Objects")) {
  document = doc;
  group = 0L;
#ifdef NO_LAYERS
  QListIterator<GObject> it (doc->getSelection ());
  for (; it.current (); ++it) 
    objects.append (it.current ());
#else
  for (list<GObject*>::iterator it = doc->getSelection ().begin ();
       it != doc->getSelection ().end (); it++)
    objects.append (*it);
#endif
}

GroupCmd::~GroupCmd () {
  if (group)
    group->unref ();
}

void GroupCmd::execute () {
  if (objects.count () > 0) {
    group = new GGroup ();
    group->ref ();

    QListIterator<GObject> it (objects);
    for (; it.current (); ++it) {
      GObject* obj = it.current ();
      group->addObject (obj);
    }
    // now insert the new group into the document
    document->insertObject (group);

    // and select it (but only it !)
    document->deleteSelectedObjects ();
    document->selectObject (group);
  }
}

void GroupCmd::unexecute () {
}
