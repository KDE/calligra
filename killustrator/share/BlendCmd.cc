/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <BlendCmd.h>
#include <klocale.h>

#include <GDocument.h>
#include <GObject.h>
#include <GCurve.h>

BlendCmd::BlendCmd (GDocument* doc, int steps)
  : Command(i18n("Blend objects"))
{
  document = doc;
  num_steps = steps;
  QListIterator<GObject> it(doc->getSelection());
  for (int i = 0; it.current(); ++it, ++i) {
    if (i == 2)
      break;
    (*it)->ref ();
    if (i == 0)
      sobj = *it;
    else
      eobj = *it;
  }
  start = end = 0L;
}

BlendCmd::~BlendCmd () {
    if (sobj)
        sobj->unref ();
    if (eobj)
        eobj->unref ();
    for (GCurve *c=curves.first(); c!=0L; c=curves.next())
        c->unref ();
    if (start)
        start->unref ();
    if (end)
        end->unref ();
}

void BlendCmd::execute () {
  if (! start) {
    if (sobj->isA ("GCurve"))
      start = (GCurve *) sobj;
    else
      start = sobj->convertToCurve ();
  }
  if (! end) {
    if (eobj->isA ("GCurve"))
      end = (GCurve *) eobj;
    else
      end = eobj->convertToCurve ();
  }
  if (start == NULL || end == NULL)
    return;
  document->setAutoUpdate (false);
  for (int i = 0; i < num_steps; i++) {
    unsigned int idx = document->findIndexOfObject (sobj);
    GCurve *curve = GCurve::blendCurves (start, end, i, num_steps);
    document->insertObjectAtIndex (curve, idx + i + 1);
    curves.append(curve);
  }
  document->setAutoUpdate (true);
}

void BlendCmd::unexecute () {
  if (start == NULL || end == NULL)
    return;
  document->setAutoUpdate (false);
  for (GCurve *c=curves.first(); c!=0L; c=curves.next())
    document->deleteObject(c);
  document->setAutoUpdate (true);
}

