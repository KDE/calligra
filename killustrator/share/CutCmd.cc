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
#include "CutCmd.h"

CutCmd::CutCmd (GDocument* doc, QList<GObject>& cboard) 
  : Command(i18n("Cut"))
{
  document = doc;
  clipboard = &cboard;
#ifdef NO_LAYERS
  QListIterator<GObject> it (doc->getSelection ());
  for (; it.current (); ++it) {
    it.current ()->ref ();
    objects.append (it.current ());
  }
#else
  for (list<GObject*>::iterator it = doc->getSelection ().begin ();
       it != doc->getSelection ().end (); it++) {
    GObject* o = *it;
    o->ref ();
    objects.append (o);
  }
#endif
}

CutCmd::~CutCmd () {
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) 
    it.current ()->unref ();
}

void CutCmd::execute () {
  clipboard->clear ();

  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    clipboard->append (it.current ()->copy ());
    document->deleteObject (it.current ());
  }
}

void CutCmd::unexecute () {
}

