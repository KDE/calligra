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

#include <iostream.h>
#include "UngroupCmd.h"

UngroupCmd::UngroupCmd (GDocument* doc) : Command(i18n("Ungroup ???")) {
  document = doc;
#ifdef NO_LAYERS
  QListIterator<GObject> it (doc->getSelection ());
  for (; it.current (); ++it) {
    GObject* o = it.current ();
    if (o->isA ("GGroup")) {
      GGroup* gobj = (GGroup *) o;
      gobj->ref ();
      groups.append (gobj);
    }
  }
#else
  for (list<GObject*>::iterator it = doc->getSelection ().begin ();
       it != doc->getSelection ().end (); it++) {
    GObject* o = *it;
    if (o->isA ("GGroup")) {
      GGroup* gobj = (GGroup *) o;
      gobj->ref ();
      groups.append (gobj);
    }
  }
#endif
}

UngroupCmd::~UngroupCmd () {
  QListIterator<GGroup> it (groups);
  for (; it.current (); ++it) 
    it.current ()->unref ();
}

void UngroupCmd::execute () {
  QListIterator<GGroup> it (groups);
  for (; it.current (); ++it) {
    GGroup *group = it.current ();

    int pos = document->findIndexOfObject (group);
    if (pos != -1) {
      document->setAutoUpdate (false);
      // extract the members of the group
      QListIterator<GObject> mi (group->getMembers ());
      for (int offs = 0; mi.current (); ++mi, ++offs) {
	GObject* obj = mi.current ();
	// transform it according to the group transformation matrix
	obj->transform (group->matrix (), true);
	    
	// and insert it into the object list at the former position
	// of the group object
	document->insertObjectAtIndex (obj, pos + offs);
	document->selectObject (obj);
      }
      // remove the group object
      document->deleteObject (group);
      document->setAutoUpdate (true);
    }
  }
}

void UngroupCmd::unexecute () {
}

