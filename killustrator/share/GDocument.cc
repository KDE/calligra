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

#include "GDocument.h"
#include "GDocument.moc"
#include "GObject.h"
#include "GPolygon.h"
#include "GText.h"
#include "GPolyline.h"
#include "GOval.h"
#include "GBezier.h"
#include "GClipart.h"
#include "GGroup.h"

#include <string>
#include <iostream.h>
#include <fstream.h>

#include <values.h>
#include <stack>
#include <vector>
#include <algorithm>

#include "xmlutils/XmlWriter.h"
#include "xmlutils/XmlReader.h"

// default papersize in mm
#define PAPER_WIDTH 210.0
#define PAPER_HEIGHT 298.0

template<class T>
struct del_obj {
  void operator () (T* obj) {
    delete obj;
  }
};

struct unselect_obj {
  void operator () (GObject* obj) {
    obj->select (false);
  }
};

QString GDocument::psPrologPath = 
     kapp->kde_datadir () + "/killustrator/prolog.ps";
QDict<QString> GDocument::fontMap;

GDocument::GDocument () {
  initialize ();
}

GDocument::~GDocument () {
#ifdef NO_LAYERS
  if (! objects.isEmpty ()) {
    QListIterator<GObject> it (objects);
    for (; it.current (); ++it)
      it.current ()->unref ();
    objects.clear ();
  }
#else
  for_each (layers.begin (), layers.end (), del_obj<GLayer> ()); 
  layers.clear ();
  selection.clear ();
#endif
}

void GDocument::setAutoUpdate (bool flag) { 
  autoUpdate = flag; 
  if (autoUpdate)
    emit changed ();
}

void GDocument::initialize () {
  pLayout.format = PG_DIN_A4;
  pLayout.orientation = PG_PORTRAIT;
  pLayout.width = PG_A4_WIDTH; pLayout.height = PG_A4_HEIGHT;
  pLayout.left = 0; pLayout.right = 0;
  pLayout.top = 0; pLayout.bottom = 0;
  pLayout.unit = PG_MM;

  // in pt !!
  paperWidth = (int) (pLayout.width / 25.4 * 72.0);
  paperHeight = (int) (pLayout.height / 25.4 * 72.0);
  last = NULL;
  modifyFlag = false;
  filename = UNNAMED_FILE;

#ifdef NO_LAYERS
  if (! selection.isEmpty ())
    selection.clear ();
  if (! objects.isEmpty ()) {
    QListIterator<GObject> it (objects);
    for (; it.current (); ++it)
      it.current ()->unref ();
    objects.clear ();
  }
#else
  selection.clear ();
  if (! layers.empty ()) {
    vector<GLayer*>::iterator i = layers.begin ();
    for (; i != layers.end (); i++)
      delete *i;
    layers.clear ();
  }
  active_layer = addLayer ();

#endif
  selBoxIsValid = false;
  autoUpdate = true;
  emit changed ();
}

void GDocument::setModified (bool flag) {
  modifyFlag = flag;
}

void GDocument::setPaperSize (int width, int height) {
  paperWidth = width;
  paperHeight = height;
}

int GDocument::getPaperWidth () const {
  return paperWidth;
}

int GDocument::getPaperHeight () const {
  return paperHeight;
}

void GDocument::drawContents (Painter& p, bool withBasePoints) {
#ifdef NO_LAYERS
  QListIterator<GObject> it = getObjects ();
  for (; it.current (); ++it)
    it.current ()->draw (p, withBasePoints);
#else
  vector<GLayer*>::iterator i = layers.begin ();
  for (; i != layers.end (); i++) {
    GLayer* layer = *i;
    if (layer->isVisible ()) {
      const list<GObject*>& contents = layer->objects ();
      for (list<GObject*>::const_iterator oi = contents.begin ();
	   oi != contents.end (); oi++)
	(*oi)->draw (p, withBasePoints);
    }
  }
#endif
}

unsigned int GDocument::objectCount () const { 
#ifdef NO_LAYERS
  return objects.count (); 
#else
  unsigned int num = 0;
  vector<GLayer*>::const_iterator i = layers.begin ();
  for (; i != layers.end (); i++) 
    num += (*i)->objectCount ();
  return num;
#endif
}

void GDocument::insertObject (GObject* obj) {
  obj->ref ();
#ifdef NO_LAYERS
  objects.append (obj);
#else
  active_layer->insertObject (obj);
#endif
  connect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
  setModified ();
  if (autoUpdate)
    emit changed ();
}

void GDocument::selectObject (GObject* obj) {
#ifdef NO_LAYERS
  if (selection.findRef (obj) == -1) {
    obj->select (true);
    selection.append (obj);
    selBoxIsValid = false;
    updateHandle ();
    emit changed ();
    emit selectionChanged ();
  }
#else
  list<GObject*>::iterator i = find (selection.begin (), selection.end (),
				     obj);
  if (i == selection.end ()) {
    // object isn't yet in selection list
    obj->select (true);
    selection.push_back (obj);
    selBoxIsValid = false;
    updateHandle ();
    emit changed ();
    emit selectionChanged ();
  }
#endif
}

void GDocument::unselectObject (GObject* obj) {
#ifdef NO_LAYERS
  if (selection.removeRef (obj)) {
    obj->select (FALSE);
    selBoxIsValid = false;
    updateHandle ();
    emit changed ();
    emit selectionChanged ();
  }
#else
  list<GObject*>::iterator i = find (selection.begin (), selection.end (),
				     obj);
  if (i != selection.end ()) {
    // remove object from the selection list
    obj->select (false);
    selection.erase (i);
    selBoxIsValid = false;
    updateHandle ();
    emit changed ();
    emit selectionChanged ();
  }
#endif
}

void GDocument::unselectAllObjects () {
#ifdef NO_LAYERS
  if (selection.isEmpty ())
    return;

  QListIterator<GObject> it (selection);
  for (; it.current (); ++it) {
    it.current ()->select (false);
  }
  selection.clear ();
#else
  if (selection.empty ())
    return;

  for_each (selection.begin (), selection.end (), unselect_obj ());
  selection.clear ();
#endif
  selBoxIsValid = false;
  emit changed ();
  emit selectionChanged ();
}

void GDocument::selectAllObjects () {
#ifdef NO_LAYERS
  selection.clear ();
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    it.current ()->select (true);
    selection.append (it.current ());
  }
#else
  selection.clear ();
  vector<GLayer*>::const_iterator i = layers.begin ();
  for (; i != layers.end (); i++) {
    GLayer* layer = *i;
    if (layer->isEditable ()) {
      list<GObject*>& contents = layer->objects ();
      for (list<GObject*>::iterator oi = contents.begin ();
	   oi != contents.end (); oi++) {
	GObject* obj = *oi;
	obj->select (true);
	selection.push_back (obj);
      }
    }
  }
#endif
  selBoxIsValid = false;
  updateHandle ();
  emit changed ();
  emit selectionChanged ();
}

void GDocument::setLastObject (GObject* obj) {
#ifdef NO_LAYERS
  if (obj == NULL || objects.findRef (obj) != -1) 
    last = obj;
#else
  if (obj == 0L || obj->getLayer () != 0L)
    last = obj;
#endif
}

void GDocument::updateHandle () {
  Rect r = boundingBoxForSelection ();
  if (selectionIsEmpty ())
    selHandle.show (false);
  else
    selHandle.setBox (r);
}

Rect GDocument::boundingBoxForSelection () {
  if (! selBoxIsValid) {
    if (! selectionIsEmpty ()) {
#ifdef NO_LAYERS
      QListIterator<GObject> it (selection);
      selBox = it.current ()->boundingBox ();
      ++it;
      for (; it.current (); ++it)
        selBox = selBox.unite (it.current ()->boundingBox ());
#else
      list<GObject*>::iterator i = selection.begin ();
      selBox = (*i++)->boundingBox ();
      for (; i != selection.end (); i++)
        selBox = selBox.unite ((*i)->boundingBox ());
#endif
    }
    else {
      selBox = Rect ();
    }
    selBoxIsValid = true;
  }
  return selBox;
}

Rect GDocument::boundingBoxForAllObjects () {
  Rect box;

#ifdef NO_LAYERS
  QListIterator<GObject> it (objects);
  box = it.current ()->boundingBox ();
  ++it;
  for (; it.current (); ++it)
    box = box.unite (it.current ()->boundingBox ());
#else
  bool init = false;

  for (vector<GLayer*>::iterator li = layers.begin (); 
       li != layers.end (); li++) {
    GLayer* layer = *li;
    if (layer->isEditable ()) {
      list<GObject*>& contents = layer->objects ();
      list<GObject*>::iterator oi = contents.begin ();
      if (! init) {
	box = (*oi++)->boundingBox ();
	init = true;
      }
      for (; oi != contents.end (); oi++)
	box = box.unite ((*oi)->boundingBox ());
    }
  }
#endif
  return box;
}

void GDocument::deleteSelectedObjects () {
  if (! selectionIsEmpty ()) {
#ifdef NO_LAYERS
    GObject* obj = objects.first ();
    while (obj != NULL) {
      if (obj->isSelected ()) {
	disconnect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
	obj->unref ();
	objects.remove ();
	obj = objects.current ();
      }
      else
	obj = objects.next ();
    }
#else
    // TODO
    for (list<GObject*>::iterator i = selection.begin ();
	 i != selection.end (); i++) {
      GObject* obj = *i;
      disconnect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
      obj->getLayer ()->deleteObject (obj);
    }
#endif
    selection.clear ();
    last = 0L;
    setModified ();
    selBoxIsValid = false;
    emit changed ();
    emit selectionChanged ();
  }
}

void GDocument::deleteObject (GObject* obj) {
  bool selected = false;

#ifdef NO_LAYERS
  if (objects.findRef (obj) != -1) {
    if (obj->isSelected ()) {
      selected = true;
      selection.removeRef (obj);
    }
    objects.removeRef (obj);
    last = 0L;
    disconnect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
    obj->unref ();
    if (selected) {
      selBoxIsValid = false;
      updateHandle ();
      emit selectionChanged ();
    }
    emit changed ();
  }
#else
  GLayer* layer = obj->getLayer ();
  if (layer->isEditable ()) {
    selected = obj->isSelected ();
    if (selected) 
      selection.remove (obj);
    last = 0L;
    disconnect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
    layer->deleteObject (obj);
    if (selected) {
      selBoxIsValid = false;
      updateHandle ();
      emit selectionChanged ();
    }
    emit changed ();
  } 
#endif
}

/**
 * Looks for an object of type <tt>otype</tt> which endpoints are distant
 * not more than <tt>max_dist</tt> from the point <tt>x, y</tt>.
 * The method returns <tt>true</tt> if an object was found as well as
 * the object in <tt>obj</tt> and the index of the nearest point in
 * <tt>pidx</tt>.
 */
bool GDocument::findNearestObject (const char* otype, int x, int y,
				   float max_dist, GObject*& obj, int& pidx) {
  float d, distance = MAXFLOAT;
  obj = 0L;
  Coord p (x, y);

#ifdef NO_LAYERS
  QListIterator<GObject> it (objects);

  for (; it.current (); ++it) {
    if (otype == 0L || it.current ()->isA (otype)) {
      if (it.current ()->findNearestPoint (p, max_dist, d, pidx) &&
	  d < distance) {
	obj = it.current ();
	distance = d;
      }
    }
  }
#else
  for (vector<GLayer*>::reverse_iterator li = layers.rbegin (); 
       li != layers.rend (); li++) {
    GLayer* layer = *li;
    if (layer->isEditable ()) {
      list<GObject*>& contents = layer->objects ();
      for (list<GObject*>::iterator oi = contents.begin ();
	   oi != contents.end (); oi++) {
	if (otype == 0L || (*oi)->isA (otype)) {
	  if ((*oi)->findNearestPoint (p, max_dist, d, pidx) &&
	      d < distance) {
	    obj = *oi;
	    distance = d;
	  }
	}
      }
    }
  }
#endif
  if (obj == 0L)
    pidx = -1;
  return obj != 0L;
}

GObject* GDocument::findContainingObject (int x, int y) {
#ifdef NO_LAYERS
  QListIterator<GObject> it (objects);
  // We are looking for the most relevant object, that means the object 
  // in front of all others. So, we have to start at the end of the
  // list ... 
  it.toLast ();
  for (; it.current (); --it) {
    if (it.current ()->contains (Coord (x, y)))
      return it.current ();
  }
#else
  GObject* result = 0L;
  // We are looking for the most relevant object, that means the object 
  // in front of all others. So, we have to start at the upper layer
  vector<GLayer*>::reverse_iterator i = layers.rbegin ();
  for (; i != layers.rend (); i++) {
    GLayer* layer = *i;
    if (layer->isEditable ()) {
      result = layer->findContainingObject (x, y);
      if (result)
	break;
    }
  }
#endif
  return result;
}

bool GDocument::findContainingObjects (int x, int y, QList<GObject>& olist) {
#ifdef NO_LAYERS
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    if (it.current ()->contains (Coord (x, y)))
      olist.append (it.current ());
  }
#else
  Coord coord (x, y);
  for (vector<GLayer*>::iterator li = layers.begin (); 
       li != layers.end (); li++) {
    if ((*li)->isEditable ()) {
      list<GObject*>& contents = (*li)->objects ();
      for (list<GObject*>::iterator oi = contents.begin ();
	   oi != contents.end (); oi++)
	if ((*oi)->contains (coord))
	  olist.append (*oi);
    }
  }
#endif
  return olist.count () > 0;
}

bool GDocument::findObjectsContainedIn (const Rect& r, QList<GObject>& olist) {
#ifdef NO_LAYERS
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    if (r.contains (it.current ()->boundingBox ()))
      olist.append (it.current ());
  }
#else
  for (vector<GLayer*>::iterator li = layers.begin (); 
       li != layers.end (); li++) {
    if ((*li)->isEditable ()) {
      list<GObject*>& contents = (*li)->objects ();
      for (list<GObject*>::iterator oi = contents.begin ();
	   oi != contents.end (); oi++)
	if (r.contains ((*oi)->boundingBox ()))
	  olist.append (*oi);
    }
  }
#endif
  return olist.count () > 0;
}

void GDocument::layerChanged () {
  emit changed ();
}

void GDocument::objectChanged () {
  if (! selectionIsEmpty ()) {
    selBoxIsValid = false;
    updateHandle ();
    GObject* obj = (GObject *) sender ();
    if (obj->isSelected () && autoUpdate) {
      emit selectionChanged ();
    }
  }
  setModified ();
  if (autoUpdate)
      emit changed ();
}

bool GDocument::saveToXml (const char* fname) {
  static const char* formats[] = {
    "a3", "a4", "a5", "us_letter", "us_legal", "screen", "custom"
  };
  static const char* orientations[] = {
    "portrait", "landscape"
  };

  ofstream os (fname);
  if (os.fail ())
    return false;
  XmlWriter xml (os, XML_DOCTYPE, XML_DTD);

  xml.startTag ("kiml"); // <kiml>
  xml.startTag ("head"); // <head>

  xml.startTag ("layout", false);
  xml.addAttribute ("format", formats[pLayout.format]);
  xml.addAttribute ("orientation", orientations[pLayout.orientation]);
  xml.addAttribute ("width", pLayout.width);
  xml.addAttribute ("height", pLayout.height);
  xml.addAttribute ("lmargin", pLayout.left);
  xml.addAttribute ("tmargin", pLayout.top);
  xml.addAttribute ("rmargin", pLayout.right);
  xml.addAttribute ("bmargin", pLayout.bottom);
  xml.closeTag (true);

  xml.endTag (); // </head>

#ifdef NO_LAYERS
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) 
    it.current ()->writeToXml (xml);
#else
  // TODO: save layer information
  for (vector<GLayer*>::iterator li = layers.begin (); 
       li != layers.end (); li++) {
    list<GObject*>& contents = (*li)->objects ();
    for (list<GObject*>::iterator oi = contents.begin ();
	 oi != contents.end (); oi++)
      (*oi)->writeToXml (xml);
  }
#endif

  xml.endTag (); // </kiml>

  setModified (false);
  filename = fname;
  return ! os.fail ();
}

bool GDocument::readFromXml (const char* fname) {
  bool endOfHeader = false, endOfBody = false;

  ifstream is (fname);
  if (is.fail ())
    return false;

  XmlReader xml (is);
  if (! xml.validHeader ()) {
    return false;
  }
  if (xml.doctype () != XML_DOCTYPE || xml.dtd () != XML_DTD) {
    cout << "Wrong doctype !" << endl;
    return false;
  }

  XmlElement elem;
  if (! xml.readElement (elem) || elem.tag () != "kiml") return false;
  if (! xml.readElement (elem) || elem.tag () != "head") return false;

  setAutoUpdate (false);
  do {
    if (! xml.readElement (elem))
      return false;
    if (elem.tag () == "layout") {
      // setup layout 
      list<XmlAttribute>::const_iterator first = 
	elem.attributes ().begin ();
 
      while (first != elem.attributes ().end ()) {
        if ((*first).name () == "format") {
	  const string& v = (*first).stringValue ();
	  if (v == "a3")            pLayout.format = PG_DIN_A3;
	  else if (v == "a4")       pLayout.format = PG_DIN_A4;
	  else if (v == "a5")       pLayout.format = PG_DIN_A5;
	  else if (v == "usletter") pLayout.format = PG_US_LETTER;
	  else if (v == "uslegal")  pLayout.format = PG_US_LEGAL;
	  else if (v == "custom")   pLayout.format = PG_CUSTOM;
	  else                      pLayout.format = PG_DIN_A4;
	}
        else if ((*first).name () == "orientation") {
	  const string& v = (*first).stringValue ();
	  if (v == "portrait")       pLayout.orientation = PG_PORTRAIT;
	  else if (v == "landscape") pLayout.orientation = PG_LANDSCAPE;
	  else                       pLayout.orientation = PG_PORTRAIT;
	}
        else if ((*first).name () == "width")
	  pLayout.width = (*first).floatValue ();
        else if ((*first).name () == "height")
	  pLayout.height = (*first).floatValue ();
        else if ((*first).name () == "lmargin")
	  pLayout.left = (*first).floatValue ();
        else if ((*first).name () == "tmargin")
	  pLayout.top = (*first).floatValue ();
        else if ((*first).name () == "rmargin")
	  pLayout.right = (*first).floatValue ();
        else if ((*first).name () == "bmargin")
	  pLayout.bottom = (*first).floatValue ();
        first++;
      }
    }
    else if (elem.tag () == "author")
      ; 
    else if (elem.tag () == "head" && elem.isEndTag ())
      endOfHeader = true;
    else
      return false;
  } while (! endOfHeader);

  // update page layout
  setPageLayout (pLayout);

  GObject* obj = 0L;
  stack<GGroup*, vector<GGroup*> > groups;
  bool finished = false;

  do {
    if (! xml.readElement (elem))
      break;
    if (elem.tag () == "kiml") {
      if (! elem.isEndTag ())
        break;
      else
        endOfBody = true;
    }
    else if (elem.isEndTag ()) {
      finished = true;
      if (elem.tag () == "group") {
	groups.pop ();
      }
    }
    else {
      finished = elem.isClosed () && elem.tag () != "point";

      if (elem.tag () == "polyline")
	obj = new GPolyline (elem.attributes ());
      else if (elem.tag () == "ellipse")
	obj = new GOval (elem.attributes ());
      else if (elem.tag () == "bezier")
	obj = new GBezier (elem.attributes ());
      else if (elem.tag () == "rectangle")
	obj = new GPolygon (elem.attributes (), GPolygon::PK_Rectangle);
      else if (elem.tag () == "polygon")
	obj = new GPolygon (elem.attributes ());
      else if (elem.tag () == "clipart")
	obj = new GClipart (elem.attributes ());
      else if (elem.tag () == "text") {
	obj = new GText (elem.attributes ());
	// read font attributes 
	if (! xml.readElement (elem) || elem.tag () != "font") 
	  break;

	list<XmlAttribute>::const_iterator first = elem.attributes ().begin ();
	QFont font = QFont::defaultFont ();

	while (first != elem.attributes ().end ()) {
	  const string& attr = (*first).name ();
	  if (attr == "face")
	    font.setFamily ((*first).stringValue ().c_str ());
	  else if (attr == "point-size")
	    font.setPointSize ((*first).intValue ());
	  else if (attr == "weight")
	    font.setWeight ((*first).intValue ());
	  else if (attr == "italic")
	    font.setItalic ((*first).intValue () != 0);
	  first++;
	}
	((GText *)obj)->setFont (font);

	// and the text
	finished = false;
	QString text_str;
	do {
	  if (! xml.readElement (elem))
	    // something goes wrong
	    break;
	  if (elem.tag () == "#PCDATA")
	    text_str += xml.getText ().c_str ();
	  else if (elem.tag () == "font" && elem.isEndTag ()) 
	    // end of font tag - ignore it
	    ;
	  else if (elem.tag () == "br")
	    // newline
	    text_str += "\n";

	  // end of element
	  if (elem.tag () == "text" && elem.isEndTag ()) {
	    ((GText *) obj)->setText (text_str);
	    finished = true;
	  }
	} while (! finished);
      }
      else if (elem.tag () == "group") {
	GGroup* group = new GGroup (elem.attributes ());
	group->setLayer (active_layer);
	//	group->ref ();

	if (!groups.empty ())
	  groups.top ()->addObject (group);
	else
	  insertObject (group);
	groups.push (group);
      }
      else if (elem.tag () == "point") {
	// add a point to the object
	list<XmlAttribute>::const_iterator first = elem.attributes ().begin ();
	Coord point;
	
	while (first != elem.attributes ().end ()) {
	  if ((*first).name () == "x")
	    point.x ((*first).floatValue ());
	  else if ((*first).name () == "y")
	    point.y ((*first).floatValue ());
	  first++;
	}
	assert (obj != 0L);
	if (obj->inherits ("GPolyline")) {
	  GPolyline* poly = (GPolyline *) obj;
	  poly->_addPoint (poly->numOfPoints (), point);
	}
      }
      else
	cout << "invalid object type: " << elem.tag () << endl;
    }
    if (obj && finished) {
      if (!groups.empty ()) {
	obj->setLayer (active_layer);
	//	obj->ref ();
	groups.top ()->addObject (obj);
      }
      else 
	insertObject (obj);
      obj = 0L;
      finished = false;
    }
  } while (! endOfBody);

  setModified (false);
  filename = fname;
  setAutoUpdate (true);
  return true;
}

unsigned int GDocument::findIndexOfObject (GObject *obj) {
#ifdef NO_LAYERS
  return objects.findRef (obj);
#else
  return obj->getLayer ()->findIndexOfObject (obj);
#endif
}

void GDocument::insertObjectAtIndex (GObject* obj, unsigned int idx) {
#ifdef NO_LAYERS
  if (idx > objects.count ())
    idx = objects.count ();
  objects.insert (idx, obj);
  connect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
#else
  obj->ref ();
  GLayer* layer = obj->getLayer ();
  if (layer == 0L)
    layer = active_layer;
  layer->insertObjectAtIndex (obj, idx);
  connect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
#endif
  setModified ();
  emit changed ();
  emit selectionChanged ();
}

void GDocument::moveObjectToIndex (GObject* obj, unsigned int idx) {
#ifdef NO_LAYERS
  int pos;

  if ((pos = objects.findRef (obj)) == -1)
    return;
  
  objects.take (pos);
  objects.insert (idx, obj);

#else
  GLayer* layer = obj->getLayer ();
  if (layer == 0L)
    layer = active_layer;
  layer->moveObjectToIndex (obj, idx);
#endif

  setModified ();
  emit changed ();
  emit selectionChanged ();
}

KoPageLayout GDocument::pageLayout () {
  return pLayout;
}

void GDocument::setPageLayout (const KoPageLayout& layout) {
  pLayout = layout;
  paperWidth = (int) (pLayout.width / 25.4 * 72.0);
  paperHeight = (int) (pLayout.height / 25.4 * 72.0);
  modifyFlag = true;
  emit sizeChanged ();
}

bool GDocument::requiredFonts (set<string>& fonts) {
#ifdef NO_LAYERS
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    if (it.current ()->isA ("GText")) {
      GText* tobj = (GText *) it.current ();
      const QFont& font = tobj->getFont ();
      fonts.insert (getPSFont (font));
    }
  }
#else
  for (vector<GLayer*>::iterator li = layers.begin (); 
       li != layers.end (); li++) {
    GLayer* layer = *li;
    if (layer->isPrintable ()) {
      list<GObject*>& contents = layer->objects ();
      for (list<GObject*>::iterator oi = contents.begin ();
	   oi != contents.end (); oi++) {
	if ((*oi)->isA ("GText")) {
	  GText* tobj = (GText *) (*oi);
	  const QFont& font = tobj->getFont ();
	  fonts.insert (getPSFont (font));
	}
      }
    }
  }
#endif
  return ! fonts.empty ();
}


const char* GDocument::getPSFont (const QFont& qfont) {
  if (fontMap.isEmpty ()) {
    QString psFontmapPath = kapp->kde_datadir () + "/killustrator/fontmap";
    ifstream fin ((const char *) psFontmapPath);
    //    fin.ignore (INT_MAX, '\n');
    char key[128], value[128], c;
    while (! fin.eof ()) {
      fin.get (c);
      if (c == '#') {
	// just a comment, ignore the rest of line
	fin.ignore (INT_MAX, '\n');
	continue;
      }
      else
	fin.unget ();
      fin >> key >> value;
      if (key[0] == '\0')
	break;
      fontMap.insert (key, new QString (value));
    }
  }
  QString family = qfont.family ();
  family = family.lower ();
  bool italic = qfont.italic ();
  int weight = qfont.weight ();
  
  QString key = family;
  if (italic)
    key += ".italic";
  if (weight >= QFont::Bold)
    key += ".bold";
  else if (weight >= QFont::DemiBold)
    key += ".demibold";
  else if (weight <= QFont::Light)
    key += ".light";

  QString* font = fontMap[key];
  if (font)
    return (const char *) *font;
  else
    return "/Times-Roman";
}

bool GDocument::writePSProlog (ostream& os) {
  ifstream prolog (psPrologPath);
  if (!prolog) 
    return false;
  
  char buf[128];
  while (!prolog.eof ()) {
    prolog.getline (buf, 128);
    os << buf << '\n';
  }
  return true;
}

void GDocument::writeToPS (ostream& os) {
#ifdef NO_LAYERS
  QListIterator<GObject> it = getObjects ();
  for (; it.current (); ++it) 
    it.current ()->writeToPS (os);
#else
  for (vector<GLayer*>::iterator li = layers.begin (); 
       li != layers.end (); li++) {
    GLayer* layer = *li;
    if (layer->isPrintable ()) {
      list<GObject*>& contents = layer->objects ();
      for (list<GObject*>::iterator oi = contents.begin ();
	   oi != contents.end (); oi++) {
	(*oi)->writeToPS (os);
      }
    }
  }
#endif
}

/*
 * Get an array with all layers of the document
 */
const vector<GLayer*>& GDocument::getLayers () {
  return layers;
}

/*
 * Set the active layer where further actions take place
 */
void GDocument::setActiveLayer (GLayer *layer) {
  vector<GLayer*>::iterator i = layers.begin ();
  for (; i != layers.end (); i++) {
    if (*i == layer) {
      active_layer = layer;
      break;
    }
  }
}

/*
 * Retrieve the active layer
 */
GLayer* GDocument::activeLayer () {
    return active_layer;
}

/*
 * Raise the given layer
 */
void GDocument::raiseLayer (GLayer *layer) {
  if (layer == layers.back ())
    // layer is already on top
    return;
  
  vector<GLayer*>::iterator i = layers.begin ();
  for (; i != layers.end (); i++) {
    if (*i == layer) {
      vector<GLayer*>::iterator j = layers.erase (i);
      layers.insert (++j, layer);
      break;
    }
  }
  emit changed ();
}

/*
 * Lower the given layer
 */
void GDocument::lowerLayer (GLayer *layer) {
  if (layer == layers.front ())
    // layer is already at bottom
    return;
  
  vector<GLayer*>::iterator i = layers.begin ();
  for (; i != layers.end (); i++) {
    if (*i == layer) {
      vector<GLayer*>::iterator j = layers.erase (i);
      layers.insert (--j, layer);
      break;
    }
  }
  emit changed ();
}

/*
 * Add a new layer on top of existing layers
 */
GLayer* GDocument::addLayer () {
  GLayer* layer = new GLayer ();
  connect (layer, SIGNAL(propertyChanged ()), this, SLOT(layerChanged ()));
  layers.push_back (layer);
  return layer;
}

/*
 * Delete the given layer as well as all contained objects
 */
void GDocument::deleteLayer (GLayer *layer) {
  if (layers.size () == 1)
    // we need at least one layer
    return;

  bool update = (active_layer == layer);

  vector<GLayer*>::iterator i = layers.begin ();
  for (; i != layers.end (); i++) {
    if (*i == layer) {
      // remove the layer from the array
      vector<GLayer*>::iterator n = layers.erase (i);
      // and delete the layer
      disconnect (layer, SIGNAL(propertyChanged ()), 
		  this, SLOT(layerChanged ()));
      delete layer;
      
      if (update) {
	// the removed layer was the active layer !
	
	if (n == layers.end ()) {
	  // this was the upper layer, so the
	  // the active layer to the last one
	  active_layer = layers.back ();
	}
	else
	  active_layer = *n;
      }
      break;
    }
  }
  emit changed ();
}

