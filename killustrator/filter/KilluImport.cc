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

#include "KilluImport.h"
#include "GGroup.h"

KilluImport::KilluImport () {
}

KilluImport::~KilluImport () {
}

bool KilluImport::setup (GDocument* doc, const char* format) {
  return true;
}

bool KilluImport::importFromFile (GDocument *doc) {
  // create a temporary document
  GDocument *tmpDoc = new GDocument ();
  bool flag = false;

  // load it
  if (flag = tmpDoc->readFromXml (inputFileName ())) {
    GGroup* group = new GGroup ();
    group->ref ();
    // now copy all objects to the group
    QListIterator<GObject> iter = tmpDoc->getObjects ();
    for (; iter.current (); ++iter) {
      GObject* obj = iter.current ();
      obj->ref ();
      group->addObject (obj);
    }
    doc->insertObject (group);
  }
  
  // and delete it
  delete tmpDoc;
  return flag;
}
