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

#include <stdlib.h>
#include <iostream.h>
#include <math.h>
#include "GGroup.h"
#include "GGroup.moc"

#include <klocale.h>
#include <kapp.h>

GGroup::GGroup () {
  connect (this, SIGNAL(propertiesChanged ()), this, 
           SLOT(propagateProperties ()));
}

GGroup::GGroup (const list<XmlAttribute>& attribs) : GObject (attribs) {
  connect (this, SIGNAL(propertiesChanged ()), this, 
           SLOT(propagateProperties ()));
}

GGroup::GGroup (const GGroup& obj) : GObject (obj) {
  QListIterator<GObject> it (obj.members);
  for (; it.current (); ++it)
    members.append (it.current ()->copy ());
  calcBoundingBox ();
}

GGroup::~GGroup () {
  QListIterator<GObject> it (members);
  for (; it.current (); ++it)
    it.current ()->unref ();
}

const char* GGroup::typeName () {
  return i18n ("Group");
}

bool GGroup::contains (const Coord& p) {
  QListIterator<GObject> it (members);

  QWMatrix im = tMatrix.invert ();
  Coord np = p.transform (im);
  for (; it.current (); ++it)
    if (it.current ()->contains (np))
      return true;
  return false;
}

void GGroup::addObject (GObject* obj) {
  obj->ref ();
  members.append (obj);
  calcBoundingBox ();
  emit changed ();
}

void GGroup::draw (Painter& p, bool withBasePoints) {
  p.save ();
  p.setWorldMatrix (tmpMatrix, true);

  QListIterator<GObject> it (members);
  for (; it.current (); ++it)
    it.current ()->draw (p);

  p.restore ();
}

GObject* GGroup::copy () {
  return new GGroup (*this);
}

void GGroup::calcBoundingBox () {
  QListIterator<GObject> it (members);
  Rect r = it.current ()->boundingBox ();
  ++it;
  for (; it.current (); ++it) {
    r = r.unite (it.current ()->boundingBox ());
  }

  Coord p[4];
  p[0] = r.topLeft ().transform (tmpMatrix);
  p[1] = Coord (r.right (), r.top ()).transform (tmpMatrix);
  p[2] = r.bottomRight ().transform (tmpMatrix);
  p[3] = Coord (r.left (), r.bottom ()).transform (tmpMatrix);

  Rect mr (p[0].x (), p[0].y (), 0, 0);
  for (int i = 1; i < 4; i++) {
    mr.left (QMIN(p[i].x (), mr.left ()));
    mr.top (QMIN(p[i].y (), mr.top ()));
    mr.right (QMAX(p[i].x (), mr.right ()));
    mr.bottom (QMAX(p[i].y (), mr.bottom ()));
  }

  updateBoundingBox (mr);
}
    
void GGroup::propagateProperties () {
  QListIterator<GObject> it (members);
  for (; it.current (); ++it) {
    it.current ()->setOutlineInfo (outlineInfo);
    it.current ()->setFillInfo (fillInfo);
  }
}

void GGroup::writeToXml (XmlWriter& xml) {
  xml.startTag ("group", false);

  writePropertiesToXml (xml);
  xml.closeTag (false);

  QListIterator<GObject> it (members);
  for (; it.current (); ++it)
    it.current ()->writeToXml (xml);

  xml.endTag ();
}
