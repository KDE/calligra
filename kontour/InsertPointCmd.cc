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

#include <InsertPointCmd.h>

#include <klocale.h>

#include <GDocument.h>
#include <GPolyline.h>

InsertPointCmd::InsertPointCmd (GDocument* doc, GPolyline* o,
                                int idx, float x, float y)
  : Command(i18n("Insert Point"))
{
  document = doc;
  obj = o;
  obj->ref ();
  index = idx;
  point = Coord (x, y);
  if (obj->isA ("GPolygon")) {
    GPolygon *poly = (GPolygon *) obj;
    kind = poly->getKind ();
  }
}

InsertPointCmd::~InsertPointCmd () {
  obj->unref ();
}

void InsertPointCmd::execute () {
  obj->insertPoint (index, point, true);
}

void InsertPointCmd::unexecute () {
  obj->removePoint (index);
  if (obj->isA ("GPolygon")) {
    GPolygon *poly = (GPolygon *) obj;
    poly->setKind (kind);
  }
}

