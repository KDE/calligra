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

#include <GroupCmd.h>

#include <klocale.h>
#include <map>

#include <GDocument.h>
#include <GGroup.h>

using namespace std;

GroupCmd::GroupCmd (GDocument* doc) : Command(i18n("Group Objects")) {
  document = doc;
  group = 0L;

  map<int, GObject*, less<int> > idx_map;

  for (list<GObject*>::iterator it = doc->getSelection ().begin ();
       it != doc->getSelection ().end (); it++) {
    // remember position of object in order to keep the order of
    // objects in the group
    GObject* o = *it;
    int idx = (int) document->findIndexOfObject (o);
    idx_map[idx] = o;
  }
  for (map<int, GObject*, less<int> >::iterator mi = idx_map.begin ();
       mi != idx_map.end (); mi++) {
    // now insert the objects according their position in the list
    objects.push_back (mi->second);
  }
}

GroupCmd::~GroupCmd () {
  if (group)
    group->unref ();
}

void GroupCmd::execute () {
  if (! objects.empty ()) {
    group = new GGroup ();
    group->ref ();

    document->setAutoUpdate (false);

    for (list<GObject*>::iterator it = objects.begin ();
         it != objects.end (); it++) {
      GObject* obj = *it;
      group->addObject (obj);
    }
    // now insert the new group into the document
    document->insertObject (group);

    // and select it (but only it !)
    document->deleteSelectedObjects ();
    document->selectObject (group);
    document->setAutoUpdate (true);
  }
}

void GroupCmd::unexecute () {
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
    }
    // remove the group object
    document->deleteObject (group);
    document->setAutoUpdate (true);
  }
}

#include <GroupCmd.moc>
