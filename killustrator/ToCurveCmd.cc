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

#include <ToCurveCmd.h>

#include <klocale.h>

#include <GDocument.h>
#include <GCurve.h>
#include "GPage.h"

ToCurveCmd::ToCurveCmd (GDocument* doc)
  : Command(i18n("Convert to curve"))
{
  document = doc;
  QListIterator<GObject> it(doc->activePage()->getSelection());
  for (; it.current(); ++it) {
    (*it)->ref ();
    objects.append(*it);
  }
}

ToCurveCmd::~ToCurveCmd () {
  GObject *o;
  for (o=objects.first(); o!=0L; o=objects.next())
    o->unref ();
  for (o=curves.first(); o!=0L; o=curves.next())
    o->unref ();
}

void ToCurveCmd::execute () {
  document->setAutoUpdate (false);
  for (GObject *i = objects.first(); i !=0L; i=objects.next()) {
    unsigned int idx = document->activePage()->findIndexOfObject (i);
    GCurve *curve = i->convertToCurve ();
    if (curve) {
      curves.append(curve);
      document->activePage()->deleteObject (i);
      document->activePage()->insertObjectAtIndex (curve, idx);
      document->activePage()->selectObject (curve);
    }
  }
  document->setAutoUpdate (true);
}

void ToCurveCmd::unexecute () {
  GCurve *c=curves.first();
  GObject *o=objects.first();
  for ( ; c != 0L; c=curves.next(), o=objects.next()) {
    unsigned int idx = document->activePage()->findIndexOfObject (c);
    document->activePage()->deleteObject (c);
    document->activePage()->insertObjectAtIndex (o, idx);
  }
}

