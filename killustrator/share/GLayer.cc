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

#include <algorithm>
#include <stdio.h>

#include <kapp.h>
#include <klocale.h>
#include "GLayer.h"
#include "GLayer.moc"
#include "GDocument.h"

int GLayer::lastID = 1;

GLayer::GLayer (GDocument* doc, const char* text) : 
  visibleFlag (true), printableFlag (true), 
  editableFlag (true), wasEditable (true), document (doc) {
  if (text == 0L) {
    char buf[20];

    sprintf (buf, "%s #%d", I18N("Layer"), lastID++);
    ident = buf;
  }
}

GLayer::~GLayer () {
  list<GObject*>::iterator iter;
  for (iter = contents.begin (); iter != contents.end (); iter++) {
    GObject *obj = *iter;
    if (obj->isSelected ())
      document->unselectObject (obj);
    obj->setLayer (0L);
    obj->unref ();
  }
  contents.clear ();
}

const char* GLayer::name () const {
  return ident.data ();
}

void GLayer::setName (const char* text) {
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

void GLayer::insertObject (GObject* obj) {
  obj->setLayer (this);
  contents.push_back (obj);
}

void GLayer::deleteObject (GObject* obj) {
  list<GObject*>::iterator i = find (contents.begin (), contents.end (),
				     obj);
  if (i != contents.end ()) {
    (*i)->setLayer (0L);
    (*i)->unref ();
    contents.erase (i);
  }
}

GObject* GLayer::findContainingObject (int x, int y) {
  // We are looking for the most relevant object, that means the object 
  // in front of all others. So, we have to start at the end of the
  // list ... 
  list<GObject*>::reverse_iterator i = contents.rbegin ();
  for (; i != contents.rend (); i++)
    if ((*i)->contains (Coord (x, y)))
      return *i;
  // nothing found
  return 0L;
}

int GLayer::findIndexOfObject (GObject *obj) {
  list<GObject*>::iterator i = find (contents.begin (), contents.end (), obj);
  if (i == contents.end ())
    return -1;
  else
    return distance (contents.begin (), i);
}

void GLayer::insertObjectAtIndex (GObject* obj, unsigned int idx) {
  list<GObject*>::iterator i = contents.begin ();
  advance (i, idx);
  contents.insert (i, obj);
  obj->setLayer (this);
}

void GLayer::moveObjectToIndex (GObject* obj, unsigned int idx) {
  list<GObject*>::iterator i = find (contents.begin (), contents.end (), obj);
  if (i == contents.end ())
    return;
  contents.erase (i);

  i = contents.begin ();
  advance (i, idx);
  contents.insert (i, obj);
}
