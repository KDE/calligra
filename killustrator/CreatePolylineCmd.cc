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

#include <CreatePolylineCmd.h>

#include <klocale.h>

#include <GDocument.h>
#include <GPolyline.h>

CreatePolylineCmd::CreatePolylineCmd (GDocument* doc, GPolyline* line)
  : Command(i18n("Create Polyline"))
{
  document = doc;
  object = line;
  object->ref ();
}

CreatePolylineCmd::CreatePolylineCmd (GDocument* doc,
                                      const QList<Coord>& pts)
  : Command(i18n("Create Polyline"))
{
  document = doc;
  object = 0;
  points = pts;
}

CreatePolylineCmd::~CreatePolylineCmd () {
  if (object)
    object->unref ();
}

void CreatePolylineCmd::execute () {
  if (object == 0L) {
    // create new object
    object = new GPolyline (document);
    QListIterator<Coord> it (points);
    for (int i = 0; it.current (); ++it, ++i)
      object->addPoint (i, *(it.current ()));
    //    object->ref ();
  }
  document->insertObject (object);
}

void CreatePolylineCmd::unexecute () {
  document->deleteObject (object);
}

