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

#include "SplitLineCmd.h"

SplitLineCmd::SplitLineCmd (GDocument* doc, GPolyline* o, int idx) 
  : Command(i18n("Split Line"))
{
  document = doc;
  obj1 = o;
  obj1->ref ();
  obj2 = 0L;
  index = idx;
}

SplitLineCmd::~SplitLineCmd () {
  obj1->unref ();
  if (obj2)
    obj2->unref ();
}

void SplitLineCmd::execute () {
  if (obj2)
    obj2->unref ();

  obj2 = obj1->splitAt (index);
  if (! obj2)
    return;

  if (obj1->isA ("GPolygon")) {
    // we have splitted a polygon -> replace the current object
    unsigned pos = document->findIndexOfObject (obj1);
    document->deleteObject (obj1);
    document->insertObjectAtIndex (obj2, pos);
    document->selectObject (obj2);
  }
  else {
    obj2->ref ();
    unsigned pos = document->findIndexOfObject (obj1);
    document->insertObjectAtIndex (obj2, pos + 1);
    document->selectObject (obj2);
  }
}

void SplitLineCmd::unexecute () {
  if (obj1->isA ("GPolygon")) {
    unsigned pos = document->findIndexOfObject (obj2);
    document->deleteObject (obj2);
    document->insertObjectAtIndex (obj1, pos);
    document->selectObject (obj1);
  }
  else if (obj2) {
    obj1->joinWith (obj2);
    document->deleteObject (obj2);
  }
}
