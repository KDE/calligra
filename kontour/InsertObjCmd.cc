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

#include <InsertObjCmd.h>

#include <klocale.h>

#include <GDocument.h>
#include <GObject.h>
#include "GPage.h"

InsertObjCmd::InsertObjCmd (GDocument* doc, QList<GObject>& objs)
  : Command(i18n("Insert Object"))
{
  document = doc;
  QListIterator<GObject> it (objs);
  for (; it.current (); ++it) {
    GObject* obj = it.current ();
    obj->ref ();
    objects.append (obj);
  }
}

InsertObjCmd::~InsertObjCmd () {
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    GObject* obj = it.current ();
    obj->unref ();
  }
}

void InsertObjCmd::execute () {
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    GObject* obj = it.current ();
    obj->ref ();
    document->activePage()->insertObject (obj);
  }
}

void InsertObjCmd::unexecute () {
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    GObject* obj = it.current ();
    document->activePage()->deleteObject (obj);
  }
}

