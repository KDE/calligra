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

#include <UngroupCmd.h>

#include <klocale.h>
#include <GDocument.h>
#include <GGroup.h>

UngroupCmd::UngroupCmd (GDocument* doc) : Command(i18n("Ungroup")) {

    groups.setAutoDelete(true);
    document = doc;
    for(QListIterator<GObject> it(doc->getSelection()); it.current(); ++it) {
        GObject* o = *it;
        if (o->isA ("GGroup")) {
            GGroup* gobj = (GGroup *) o;
            gobj->ref ();
            GPair *p=new GPair;
            p->group=gobj;
            groups.append(p);
        }
    }
}

UngroupCmd::~UngroupCmd () {
    for (GPair *p=groups.first(); p!=0L; p=groups.next()) {
        p->group->unref ();
        for (GObject *o=p->members.first(); o!=0L; o=p->members.next())
            o->unref ();
    }
}

void UngroupCmd::execute () {
    for (GPair *p=groups.first(); p!=0L; p=groups.next()) {
        GGroup *group = p->group;
        int pos = document->findIndexOfObject (group);
        if (pos != -1) {
            document->setAutoUpdate (false);
            // extract the members of the group
            QList <GObject> members = group->getMembers ();
            GObject *mo=members.first();
            for (int offs = 0; mo!=0L; mo=members.next(), offs++) {
                // transform it according to the group transformation matrix
                mo->transform (group->matrix (), true);

                // and insert it into the object list at the former position
                // of the group object
                document->insertObjectAtIndex(mo, pos + offs);
                document->selectObject (mo);
                p->members.append(mo);
                mo->ref ();
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
  for (GPair *p=groups.first(); p!=0L; p=groups.next()) {
    QWMatrix m = p->group->matrix ().invert ();

    for (GObject *o=p->members.first(); o!=0L; o=p->members.next()) {
      o->transform (m, true);
      p->group->addObject (o);
      document->deleteObject (o);
    }
    document->insertObject (p->group);
    document->selectObject (p->group);
  }
  document->setAutoUpdate (true);
}

