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

#include <klocale.h>
#include <GLayer.h>
#include <GDocument.h>
#include <GObject.h>

int GLayer::lastID = 0;

GLayer::GLayer (GDocument* doc, const QString &text) :
  visibleFlag (true), printableFlag (true),
  editableFlag (true), wasEditable (true), internalFlag (false),
  document (doc) {
  if (text.isEmpty())
    ident=i18n("Layer #") + QString::number(lastID++);
}

GLayer::~GLayer () {
  for (GObject *o=contents.first(); o!=0L; o=contents.next()) {
    if (o->isSelected ())
      document->unselectObject (o);
    o->setLayer (0L);
    o->unref ();
  }
  contents.clear ();
}

QString GLayer::name () const {
  return ident;
}

void GLayer::setName (const QString &text) {
  ident = text;
}

void GLayer::setVisible (bool flag) {
  if (visibleFlag != flag) {
    visibleFlag = flag;
    if (!visibleFlag)
      editableFlag = false;
    else {
      if (wasEditable)
        editableFlag = true;
    }
    emit propertyChanged ();
  }
}

void GLayer::setPrintable (bool flag) {
  if (isInternal ())
    return;

  if (printableFlag != flag) {
    printableFlag = flag;
    emit propertyChanged ();
  }
}

void GLayer::setEditable (bool flag) {
  if (editableFlag != flag) {
    editableFlag = flag;
    if (editableFlag)
      visibleFlag = true;
    wasEditable = editableFlag;
    emit propertyChanged ();
  }
}

void GLayer::setInternal () {
  internalFlag = true;
  printableFlag = false;
}

void GLayer::insertObject (GObject* obj) {
  obj->setLayer (this);
  contents.append(obj);
}

void GLayer::deleteObject (GObject* obj) {
  GObject *o = contents.at(contents.findRef(obj));
  if (o!=0L) {
    //    o->setLayer (0L);
    o->unref ();
    contents.removeRef(o);
  }
}

GObject* GLayer::findContainingObject (int x, int y) {
  // We are looking for the most relevant object, that means the object
  // in front of all others. So, we have to start at the end of the
  // list ...
  GObject *o=contents.last();
  for (; o!=0L; o=contents.prev())
    if (o->contains (Coord (x, y)))
      return o;
  // nothing found
  return 0L;
}

int GLayer::findIndexOfObject (GObject *obj) {
    return contents.findRef(obj);
}

void GLayer::insertObjectAtIndex (GObject* obj, unsigned int idx) {
  contents.insert (idx, obj);
  obj->setLayer (this);
}

GObject *GLayer::objectAtIndex (unsigned int idx) {
    return contents.at(idx);
}


void GLayer::moveObjectToIndex (GObject* obj, unsigned int idx) {
    if(contents.removeRef(obj))
        contents.insert(idx, obj);
}

#include <GLayer.moc>
