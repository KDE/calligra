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
#include <klocale.h>

UngroupCmd::UngroupCmd (GDocument* doc) : Command(i18n("Ungroup")) {
  document = doc;
  for (list<GObject*>::iterator it = doc->getSelection ().begin ();
       it != doc->getSelection ().end (); it++) {
    GObject* o = *it;
    if (o->isA ("GGroup")) {
      GGroup* gobj = (GGroup *) o;
      gobj->ref ();
      list<GObject*> dummy;
      groups.push_back (GPair (gobj, dummy));
    }
  }
}

UngroupCmd::~UngroupCmd () {
  for (list<GPair>::iterator it = groups.begin ();
       it != groups.end (); it++) {
    it->first->unref ();
    list<GObject*>& olist = it->second;
    for (list<GObject*>::iterator it2 = olist.begin ();
	 it2 != olist.end (); it2++)
      (*it2)->unref ();
  }
}

void UngroupCmd::execute () {
  for (list<GPair>::iterator it = groups.begin (); it != groups.end (); it++) {
    GGroup *group = it->first;
    list<GObject*>& olist = it->second;

    int pos = document->findIndexOfObject (group);
    if (pos != -1) {
      document->setAutoUpdate (false);
      // extract the members of the group
      const list<GObject*> members = group->getMembers ();
      list<GObject*>::const_iterator mi = members.begin ();
      for (int offs = 0; mi != members.end (); mi++, offs++) {
	GObject* obj = *mi;
	// transform it according to the group transformation matrix
	obj->transform (group->matrix (), true);
	    
	// and insert it into the object list at the former position
	// of the group object
	document->insertObjectAtIndex (obj, pos + offs);
	document->selectObject (obj);
	olist.push_back (obj);
	obj->ref ();
      }
      // remove the group object
      document->deleteObject (group);
      document->setAutoUpdate (true);
    }
  }
}

void UngroupCmd::unexecute () {
  document->setAutoUpdate (false);
  document->unselectAllObjects ();
  for (list<GPair>::iterator it = groups.begin (); it != groups.end (); it++) {
    GGroup *group = it->first;
    QWMatrix m = group->matrix ().invert ();
    list<GObject*>& olist = it->second;

    for (list<GObject*>::iterator it2 = olist.begin ();
	 it2 != olist.end (); it2++) {
      GObject* obj = *it2;
      obj->transform (m, true);
      group->addObject (obj);
      document->deleteObject (obj);
    }
    document->insertObject (group);
    document->selectObject (group);
  }
  document->setAutoUpdate (true);
}

