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

UngroupCmd::UngroupCmd (GDocument* doc) {
  document = doc;
  QListIterator<GObject> it (doc->getSelection ());
  for (; it.current (); ++it) {
    GObject* o = it.current ();
    if (o->isA ("GGroup")) {
      GGroup* gobj = (GGroup *) o;
      gobj->ref ();
      groups.append (gobj);
    }
  }
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
      // remove the group object
      document->deleteObject (group);

      // extract the members of the group
      QListIterator<GObject> mi (group->getMembers ());
      for (int offs = 0; mi.current (); ++mi, ++offs) {
	GObject* obj = mi.current ();
	// transform it according to the group transformation matrix
	obj->transform (group->matrix (), true);
	    
	// and insert it into the object list at the former position
	// of the group object
	document->insertObjectAtIndex (obj, pos + offs);
      }
    }
  }
}

void UngroupCmd::unexecute () {
}

