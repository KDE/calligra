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
#include "CreateBezierCmd.h"

CreateBezierCmd::CreateBezierCmd (GDocument* doc, GBezier* curve) 
  : Command(i18n("Create Bezier"))
{
  document = doc;
  object = curve;
  object->ref ();
}

/*
CreateBezierCmd::CreateBezierCmd (GDocument* doc, const QList<Coord>& pts) {
  document = doc;
  object = 0L;
  points = pts;
}
*/

CreateBezierCmd::~CreateBezierCmd () {
  if (object)
    object->unref ();
}

void CreateBezierCmd::execute () {
  /*
  if (object == 0L) {
    object = new GBezier ();
    QListIterator<Coord> it (points);
    for (int i = 0; it.current (); ++it, ++i)
      object->addPoint (i, *(it.current ()));
    object->ref ();
  }
  */
  document->insertObject (object);
}

void CreateBezierCmd::unexecute () {
  document->deleteObject (object);
}
