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

#include <AddLineSegmentCmd.h>
#include <GPolyline.h>

#include <klocale.h>

AddLineSegmentCmd::AddLineSegmentCmd (GDocument* doc, GPolyline* obj,
                                      int idx, QList<Coord>& pnts)
  : Command(i18n("Add Line Segment"))
{
  document = doc;
  line = obj;
  line->ref ();
  index = idx;
  Coord* c = pnts.first ();
  while (c != 0L) {
    points.append (new Coord (*c));
    c = pnts.next ();
  }
  points.setAutoDelete (true);
}

AddLineSegmentCmd::~AddLineSegmentCmd () {
  line->unref ();
}

void AddLineSegmentCmd::execute () {
  for (unsigned int i = 0; i < points.count (); i++) {
    bool update = (i + 1 == points.count ());
    line->addPoint (index != 0 ? index + i : 0, *(points.at (i)), update);
  }
}

void AddLineSegmentCmd::unexecute () {
  for (unsigned int i = 0; i < points.count (); i++) {
    bool update = (i + 1 == points.count ());
    line->removePoint (index, update);
  }
}

