/* -*- C++ -*-

  $Id$
  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

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

#include "GGroup.h"

#include <qdom.h>
#include <qpainter.h>

#include <klocale.h>
#include <kdebug.h>

#include "GDocument.h"

GGroup::GGroup():
GObject()
{
}

GGroup::GGroup(const QDomElement &element):
GObject(element.namedItem("go").toElement())
{
/*  QDomElement child = element.firstChild().toElement();
    for( ; !child.isNull(); child = child.nextSibling().toElement() ) {
        if(child.tagName()=="gobject")
            continue;
        GObject *obj=KIllustrator::objectFactory(child,doc->document());
        if(!obj)
//        {
//           GObject *proto = GObject::lookupPrototype (child.tagName());
//           if (proto != 0L)
//           {
//              obj = proto->create (m_gdoc, child);
//           }
//           else
              kdDebug(38000) << "invalid object type: " << child.tagName() << endl;
//        }
        if(obj)  // safer
            addObject(obj);
    }*/
}

GGroup::GGroup(const GGroup &obj):
GObject(obj)
{
/*  QPtrList<GObject> tmp = obj.getMembers();
  for(GObject *o=tmp.first(); o!=0L; o=tmp.next())
        members.append(o->copy());
  calcBoundingBox();*/
}

GGroup::~GGroup()
{
  for(GObject *o = members.first(); o != 0L; o = members.next())
    o->unref();
}

void GGroup::addObject(GObject *obj)
{
  obj->ref();
  members.append(obj);
  calcBoundingBox();
}

GObject *GGroup::copy() const
{
  return new GGroup(*this);
}

QString GGroup::typeName() const
{
  return i18n("Group");
}

QDomElement GGroup::writeToXml(QDomDocument &document)
{
  QDomElement group = document.createElement("group");
/*  group.appendChild(GObject::writeToXml(document));
  for(GObject *o = members.first(); o != 0L; o = members.next())
    group.appendChild(o->writeToXml(document));*/
  return group;
}

void GGroup::draw(KoPainter *p, const QWMatrix &m, bool withBasePoints, bool outline, bool withEditMarks)
{
  for(GObject *o = members.first(); o != 0L; o = members.next())
    o->draw(p, tmpMatrix * m, false, outline);
}

int GGroup::getNeighbourPoint(const KoPoint &p)
{
  return -1;
}

void GGroup::movePoint(int idx, double dx, double dy, bool /*ctrlPressed*/)
{
}

void GGroup::removePoint(int idx)
{
}

bool GGroup::contains(const KoPoint &p)
{
/*    if (box.contains (p)) {
        Coord np = p.transform (iMatrix);
        for (GObject *o=members.first(); o!=0L; o=members.next())
            if (o->contains (np))
                return true;
    }*/
  return false;
}

void GGroup::calcBoundingBox()
{
  if(members.isEmpty())
    return;

  GObject *o = members.first();
/*  for (; o!=0; o=members.next())
      o->calcBoundingBox ();

  o = members.first();*/
  mBBox = o->boundingBox();
  for(o = members.next(); o != 0L; o=members.next())
    mBBox = mBBox.unite(o->boundingBox());

/*  Coord p[4];
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

  updateBoundingBox (mr);*/
}

GPath *GGroup::convertToPath() const
{
  return 0L;
}

bool GGroup::isConvertible() const
{
  return false;
}

#include "GGroup.moc"
