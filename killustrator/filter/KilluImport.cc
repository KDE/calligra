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

#include <qfile.h>
#include "KilluImport.h"
#include "GGroup.h"

KilluImport::KilluImport () {
}

KilluImport::~KilluImport () {
}

bool KilluImport::setup (GDocument* , const char* ) {
  return true;
}

bool KilluImport::importFromFile (GDocument *doc) {

    // create a temporary document
    GDocument *tmpDoc = new GDocument ();
    bool flag = false;

    QFile file(inputFileName());
    if(!file.open(IO_ReadOnly))
       return false;

    QDomDocument document;
    document.setContent(&file);
    if (flag = tmpDoc->readFromXml (document)) {
	GGroup* group = new GGroup ();
	group->ref ();
	// now copy all objects to the group
#ifdef NO_LAYERS
	QListIterator<GObject> iter = tmpDoc->getObjects ();
	for (; iter.current (); ++iter) {
	    GObject* obj = iter.current ();
	    obj->ref ();
	    group->addObject (obj);
	}
#else
	for (vector<GLayer*>::const_iterator li = tmpDoc->getLayers ().begin ();
	     li != tmpDoc->getLayers ().end (); li++) {
	    if ((*li)->isVisible ()) {
		list<GObject*>& contents = (*li)->objects ();
		for (list<GObject*>::iterator oi = contents.begin ();
		     oi != contents.end (); oi++) {
		    GObject* obj = *oi;
		    obj->ref ();
		    group->addObject (obj);
		}
	    }
	}
#endif
	doc->insertObject (group);
    }

    // and delete it
    delete tmpDoc;
    return flag;
}
