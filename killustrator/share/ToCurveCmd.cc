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

#include "ToCurveCmd.h"
#include "ToCurveCmd.moc"
#include <klocale.h>

#include "GDocument.h"
#include "GObject.h"
#include "GCurve.h"

ToCurveCmd::ToCurveCmd (GDocument* doc) 
  : Command(i18n("Convert to curve"))
{
  document = doc;
  list<GObject*>::iterator it = doc->getSelection ().begin ();
  for (; it != doc->getSelection ().end (); it++) {
    (*it)->ref ();
    objects.push_back (*it);
  }
}

ToCurveCmd::~ToCurveCmd () {
  list<GObject*>::iterator i1;
  for (i1 = objects.begin (); i1 != objects.end (); i1++)
    (*i1)->unref ();
  list<GCurve*>::iterator i2;
  for (i2 = curves.begin (); i2 != curves.end (); i2++)
    (*i2)->unref ();
}

void ToCurveCmd::execute () {
  document->setAutoUpdate (false);
  list<GObject*>::iterator i;
  for (i = objects.begin (); i != objects.end (); i++) {
    unsigned int idx = document->findIndexOfObject (*i);
    GCurve *curve = (*i)->convertToCurve ();
    if (curve) {
      curves.push_back (curve);
      document->deleteObject (*i);
      document->insertObjectAtIndex (curve, idx);
      document->selectObject (curve);
    }
  }
  document->setAutoUpdate (true);
}

void ToCurveCmd::unexecute () {
  list<GCurve*>::iterator i1;
  list<GObject*>::iterator i2;
  i1 = curves.begin ();
  i2 = objects.begin ();
  for (; i1 != curves.end (); i1++, i2++) {
    unsigned int idx = document->findIndexOfObject (*i1);
    document->deleteObject (*i1);
    document->insertObjectAtIndex (*i2, idx);
  }
}
