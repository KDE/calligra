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
#include <strstream.h>
#include <qclipboard.h>
#include "PasteCmd.h"
#include "GText.h"
#include <klocale.h>

PasteCmd::PasteCmd (GDocument* doc) 
  : Command(i18n("Paste")) 
{
  document = doc;
}

PasteCmd::~PasteCmd () {
  for (list<GObject*>::iterator it = objects.begin ();
       it != objects.end (); it++)
      (*it)->unref ();
}

void PasteCmd::execute () {
    for (list<GObject*>::iterator it = objects.begin ();
	 it != objects.end (); it++)
	(*it)->unref ();
    objects.clear ();
    const char* buf = QApplication::clipboard ()->text ();
    if (::strlen (buf)) {
      if (::strncmp (buf, "<?xml", 5) == 0) {
	// KIllustrator objects
	QWMatrix m;
	m.translate (10, 10);
	
	istrstream is (buf);
	document->insertFromXml (is, objects);
	document->unselectAllObjects ();
	for (list<GObject*>::iterator it = objects.begin ();
	     it != objects.end (); it++) {
	  (*it)->ref ();
	  (*it)->transform (m, true);
	  document->selectObject (*it);
	}
      }
      else {
	// plain text
	GText *tobj = new GText ();
	tobj->setText (buf);
	objects.push_back (tobj);
	document->insertObject (tobj);
      }
    }
}

void PasteCmd::unexecute () {
  for (list<GObject*>::iterator it = objects.begin ();
       it != objects.end (); it++)
      document->deleteObject (*it);
}

