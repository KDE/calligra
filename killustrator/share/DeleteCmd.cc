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
#include "DeleteCmd.h"
#include <klocale.h>

DeleteCmd::DeleteCmd (GDocument* doc) : Command(i18n("Delete")) {
  document = doc;
  for (list<GObject*>::iterator it = doc->getSelection ().begin ();
       it != doc->getSelection ().end (); it++) {
    GObject *o = *it;
    o->ref ();
    // store the old position of the object
    int pos = doc->findIndexOfObject (o);
    objects.push_back (pair<int, GObject*> (pos, o));
  }
}

DeleteCmd::~DeleteCmd () {
  list<pair<int, GObject*> >::iterator i;

  for (i = objects.begin (); i != objects.end (); i++)
    i->second->unref ();
}

void DeleteCmd::execute () {
  list<pair<int, GObject*> >::iterator i;

  document->setAutoUpdate (false);
  for (i = objects.begin (); i != objects.end (); i++)
    document->deleteObject (i->second);
  document->setAutoUpdate (true);
}

void DeleteCmd::unexecute () {
  list<pair<int, GObject*> >::iterator i;
  document->setAutoUpdate (false);
  document->unselectAllObjects ();
  for (i = objects.begin (); i != objects.end (); i++) {
    // insert the object at the old position
    int pos = i->first;
    GObject* obj = i->second;
    obj->ref ();
    document->insertObjectAtIndex (obj, pos);
    document->selectObject (obj);
  }
  document->setAutoUpdate (true);
}

