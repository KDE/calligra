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

#include <algorithm>

#include <klocale.h>
#include <kapp.h>

struct release_obj {
  void operator () (GObject* obj) {
    obj->unref ();
  }
};

GGroup::GGroup () {
  connect (this, SIGNAL(propertiesChanged (GObject::Property, int)), this, 
           SLOT(propagateProperties (GObject::Property, int)));
}

GGroup::GGroup (const list<XmlAttribute>& attribs) : GObject (attribs) {
  connect (this, SIGNAL(propertiesChanged (GObject::Property, int)), this, 
           SLOT(propagateProperties (GObject::Property, int)));
}

GGroup::GGroup (const GGroup& obj) : GObject (obj) {
    list<GObject*>::const_iterator i = obj.members.begin ();
    for (; i != obj.members.end (); i++)
	members.push_back ((*i)->copy ());
    calcBoundingBox ();
}

GGroup::~GGroup () {
  for_each (members.begin (), members.end (), release_obj ());
  members.clear ();
}

QString GGroup::typeName () const {
  return SI18N ("Group");
}

bool GGroup::contains (const Coord& p) {
    if (box.contains (p)) {
	Coord np = p.transform (iMatrix);
	list<GObject*>::iterator i = members.begin ();
	for (; i != members.end (); i++)
	    if ((*i)->contains (np))
		return true;
    }
    return false;
}

void GGroup::addObject (GObject* obj) {
  obj->ref ();
  members.push_back (obj);
  updateRegion ();
}

void GGroup::draw (QPainter& p, bool /*withBasePoints*/, bool outline) {
  p.save ();
  p.setWorldMatrix (tmpMatrix, true);

    list<GObject*>::iterator i = members.begin ();
    for (; i != members.end (); i++)
	(*i)->draw (p, false, outline);

  p.restore ();
}

GObject* GGroup::copy () {
  return new GGroup (*this);
}

GObject* GGroup::clone (const list<XmlAttribute>& attribs) {
  return new GGroup (attribs);
}

void GGroup::calcBoundingBox () {
  if (members.empty ())
    return;

  list<GObject*>::iterator it = members.begin ();
  for (; it != members.end (); it++)
      (*it)->calcBoundingBox ();

  it = members.begin ();
  Rect r = members.front ()->boundingBox ();
  for (it++; it != members.end (); it++) {
    r = r.unite ((*it)->boundingBox ());
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
    
void GGroup::propagateProperties (GObject::Property prop, int mask) {
    list<GObject*>::iterator i = members.begin ();
    for (; i != members.end (); i++) {
      if (prop == GObject::Prop_Outline) {
	  // don't update the custom info (shape etc.)
	  outlineInfo.mask = mask & (GObject::OutlineInfo::Color | 
				     GObject::OutlineInfo::Style |
				     GObject::OutlineInfo::Width);
	  (*i)->setOutlineInfo (outlineInfo);
      }
      if (prop == GObject::Prop_Fill) {
	  fillInfo.mask = mask;
	  (*i)->setFillInfo (fillInfo);
      }
    }
}

void GGroup::writeToXml (XmlWriter& xml) {
  xml.startTag ("group", false);

  writePropertiesToXml (xml);
  xml.closeTag (false);

    list<GObject*>::iterator i = members.begin ();
    for (; i != members.end (); i++)
	(*i)->writeToXml (xml);

  xml.endTag ();
}

void GGroup::printInfo () {
    cout << ">>>>>>>>>>>>>>>>>>>>>\n";
    cout << className () << " bbox = [" << boundingBox () << "]" << endl;
    list<GObject*>::iterator i = members.begin ();
    for (; i != members.end (); i++) {
	(*i)->printInfo ();
    }
    cout << "<<<<<<<<<<<<<<<<<<<<<\n";
}
