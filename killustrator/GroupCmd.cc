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
#include <qdict.h>

#include <GDocument.h>
#include <GGroup.h>
#include "GPage.h"

GroupCmd::GroupCmd (GDocument* doc) : Command(i18n("Group Objects")) {
  document = doc;
  group = 0L;

  QMap<int, GObject*> idx_map;

  for (QListIterator<GObject> it(doc->activePage()->getSelection()); it.current(); ++it) {
    // remember position of object in order to keep the order of
    // objects in the group
    GObject* o = *it;
    int idx = (int) document->activePage()->findIndexOfObject (o);
    idx_map.insert(idx, o);
  }
  for (QMap<int, GObject*>::ConstIterator it=idx_map.begin();
       it != idx_map.end(); ++it) {
      // now insert the objects according their position in the list
      objects.append(it.data());
  }
}

GroupCmd::~GroupCmd () {
  if (group)
    group->unref ();
}

void GroupCmd::execute () {
  if (!objects.isEmpty ()) {
    group = new GGroup (document);
    group->ref();

    document->setAutoUpdate (false);

    for (GObject *o=objects.first(); o!=0L;
         o=objects.next())
      group->addObject (o);

    // now insert the new group into the document
    document->activePage()->insertObject (group);

    // and select it (but only it !)
    document->activePage()->deleteSelectedObjects ();
    document->activePage()->selectObject (group);
    document->setAutoUpdate (true);
  }
}

void GroupCmd::unexecute () {
  int pos = document->activePage()->findIndexOfObject (group);
  if (pos != -1) {
    document->setAutoUpdate (false);
    // extract the members of the group
    const QList<GObject> members = group->getMembers ();
    QListIterator<GObject> mi(members);
    for (int offs = 0; mi.current(); ++mi, ++offs) {
      GObject* obj = *mi;
      // transform it according to the group transformation matrix
      obj->transform (group->matrix (), true);

      // and insert it into the object list at the former position
      // of the group object
      document->activePage()->insertObjectAtIndex (obj, pos + offs);
      document->activePage()->selectObject (obj);
    }
    // remove the group object
    document->activePage()->deleteObject (group);
    document->setAutoUpdate (true);
  }
}

