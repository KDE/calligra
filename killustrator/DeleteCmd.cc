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

#include <DeleteCmd.h>

#include <klocale.h>

#include <GDocument.h>
#include <GObject.h>
#include "GPage.h"

DeleteCmd::DeleteCmd (GDocument* doc) : Command(i18n("Delete")) {
  document = doc;
  objects.setAutoDelete(true);
  for(QListIterator<GObject> it(doc->activePage()->getSelection()); it.current(); ++it) {
      MyPair *p=new MyPair;
      p->o=*it;
      p->o->ref ();
      // store the old position of the object
      p->pos = doc->activePage()->findIndexOfObject (p->o);
      objects.append(p);
  }
}

DeleteCmd::~DeleteCmd () {
    for (MyPair *p=objects.first(); p!=0; p=objects.next())
        p->o->unref ();
}

void DeleteCmd::execute () {
    document->setAutoUpdate (false);
    for (MyPair *p=objects.first(); p!=0L; p=objects.next())
        document->activePage()->deleteObject (p->o);
    document->setAutoUpdate (true);
}

void DeleteCmd::unexecute () {
  document->setAutoUpdate (false);
  document->activePage()->unselectAllObjects ();
  for (MyPair *p=objects.first(); p!=0L; p=objects.next()) {
    // insert the object at the old position
    p->o->ref ();
    document->activePage()->insertObjectAtIndex (p->o, p->pos);
    document->activePage()->selectObject (p->o);
  }
  document->setAutoUpdate (true);
}

