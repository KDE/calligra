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

#include <unistd.h>
#include <stdio.h>
#include <sys/param.h>
#include <string.h>
#include <fstream.h>
#include <pfstream.h>
#include "PSImport.h"
#include "GGroup.h"

static const char *check_for_pstoedit (const char* dir) {
  static char buf[MAXPATHLEN];
  sprintf (buf, "%s/pstoedit", dir);
  if (access (buf, X_OK) == 0)
    return buf;
  else
    return NULL;
}

PSImport::PSImport () {
}

PSImport::~PSImport () {
}

bool PSImport::setup (GDocument* , const char* ) {
  return true;
}

bool PSImport::installed () {
  char* path = getenv ("PATH");
  const char* dir = strtok (path, ":");
  const char* fpath = NULL;
  do {
    if ((fpath = check_for_pstoedit (dir))) {
      fullPath = fpath;
      return true;
    }
    dir = strtok (NULL, ":");
  } while (dir);
  return false;
}

bool PSImport::importFromFile (GDocument *doc) {
  // create a temporary document
  GDocument *tmpDoc = new GDocument ();
  bool flag = false;
  char cmd[1000];

  sprintf (cmd, "|%s -f kil %s 2> /dev/null", (const char *) fullPath,
	   inputFileName ());

  // load it
  ipfstream in (cmd);
  if (in.fail ())
    return false;
  if (flag = tmpDoc->readFromXml (in)) {
    GGroup* group = new GGroup ();
    group->ref ();
    // now copy all objects to the group
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
    doc->insertObject (group);
  }

  // and delete it
  delete tmpDoc;
  return flag;
}
