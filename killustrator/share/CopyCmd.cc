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
#include <qclipboard.h>
#include <strstream.h>
#include "CopyCmd.h"
#include "CopyCmd.moc"
#include <klocale.h>

#include "GDocument.h"
#include "GObject.h"

CopyCmd::CopyCmd (GDocument* doc) 
  : Command(i18n("Copy"))
{
  document = doc;
  for (list<GObject*>::iterator it = doc->getSelection ().begin ();
       it != doc->getSelection ().end (); it++) {
    GObject* o = *it;
    o->ref ();
    objects.push_back (o);
  }
}

CopyCmd::~CopyCmd () {
  for (list<GObject*>::iterator it = objects.begin ();
       it != objects.end (); it++)
      (*it)->unref ();
}

void CopyCmd::execute () {
  ostrstream os;
  XmlWriter xs (os);

  xs.startTag ("doc", false);
  xs.addAttribute ("mime", KILLUSTRATOR_MIMETYPE);
  xs.closeTag ();

  for (list<GObject*>::iterator it = objects.begin ();
       it != objects.end (); it++)
      (*it)->writeToXml (xs);

  xs.endTag (); // </doc>

  os << ends;
  QApplication::clipboard ()->setText (os.str ());
}

void CopyCmd::unexecute () {
  QApplication::clipboard ()->clear ();
}

