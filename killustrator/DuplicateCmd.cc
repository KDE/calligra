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

#include <DuplicateCmd.h>

#include <klocale.h>

#include <GDocument.h>
#include "GPage.h"
#include <GObject.h>
#include <PStateManager.h>

bool DuplicateCmd::repeatCmd = false;
float DuplicateCmd::repOffX = 0.0;
float DuplicateCmd::repOffY = 0.0;

DuplicateCmd::DuplicateCmd (GDocument* doc)
  : Command(i18n("Duplicate"))
{
  document = doc;
  for (QListIterator<GObject> it(doc->activePage()->getSelection()); it.current(); ++it) {
    GObject* o = *it;
    o->ref ();
    objects.append(o);
  }
}

DuplicateCmd::~DuplicateCmd () {
    GObject *o;
    for (o=objects.first(); o!=0L; o=objects.next())
        o->unref ();
    for (o=new_objects.first(); o!=0L; o=new_objects.next())
        o->unref ();
}

void DuplicateCmd::execute () {
  float xoff;
  float yoff;

  if (repeatCmd) {
    xoff = repOffX;
    yoff = repOffY;
  }
  else {
    xoff = PStateManager::instance ()->duplicateXOffset ();
    yoff = PStateManager::instance ()->duplicateYOffset ();
  }
  QWMatrix m;
  m.translate (xoff, yoff);

  document->activePage()->unselectAllObjects ();
  for (GObject *i=objects.first(); i!=0L;
       i=objects.next()) {
    GObject *o = i->copy ();
    o->ref ();
    o->transform (m, true);
    document->activePage()->insertObject (o);
    document->activePage()->selectObject (o);
    new_objects.append(o);
  }
}

void DuplicateCmd::unexecute () {
    document->activePage()->unselectAllObjects ();
    GObject *o;
    for (o=new_objects.first(); o!=0L; o=new_objects.next())
        document->activePage()->deleteObject(o);
    for (o = objects.first(); o!=0L; o=objects.next())
        document->activePage()->selectObject(o);
}

void DuplicateCmd::resetRepetition () {
    repeatCmd = false;
}

void DuplicateCmd::setRepetitionOffset (float dx, float dy) {
  repOffX = dx + PStateManager::instance ()->duplicateXOffset ();
  repOffY = dy + PStateManager::instance ()->duplicateYOffset ();
  repeatCmd = true;
}

