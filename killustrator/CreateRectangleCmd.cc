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

#include <CreateRectangleCmd.h>

#include <klocale.h>

#include <GDocument.h>
#include <GPolygon.h>

CreateRectangleCmd::CreateRectangleCmd (GDocument* doc, GPolygon* obj) :
  Command(i18n("Create Rectangle"))
{
  document = doc;
  object = obj;
  object->ref ();
}

CreateRectangleCmd::CreateRectangleCmd (GDocument* doc, const Coord& p0,
                                        const Coord& p1, bool flag) :
  Command(i18n("Create Rectangle"))
{
  document = doc;
  object = 0L;
  spos = p0;
  epos = p1;
  sflag = flag;
}

CreateRectangleCmd::~CreateRectangleCmd () {
  if (object)
    object->unref ();
}

void CreateRectangleCmd::execute () {
  if (object == 0L) {
    // create polygon
    object = new GPolygon (document, sflag ? GPolygon::PK_Square :
                           GPolygon::PK_Rectangle);
    object->addPoint (0, spos);
    object->addPoint (1, spos);
    object->addPoint (2, spos);
    object->addPoint (3, spos);
    object->setEndPoint (epos);
    //    object->ref ();
  }
  document->insertObject (object);
}

void CreateRectangleCmd::unexecute () {
  document->deleteObject (object);
}

