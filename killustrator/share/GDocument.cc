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

#include <qfile.h>

#include "GDocument.h"
#include "GDocument.moc"
#include "GPolygon.h"
#include "GText.h"
#include "GPolyline.h"
#include "GOval.h"
#include "GBezier.h"
#include "GClipart.h"
#include "GGroup.h"
#include "GPixmap.h"
#include "GCurve.h"

#include <string>
#include <map>
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>

#ifdef __FreeBSD__
#include <math.h>
#else
#include <values.h>
#endif

#include <stack>
#include <vector>
#include <algorithm>

#include "xmlutils/XmlWriter.h"
#include "xmlutils/XmlReader.h"

#include "units.h"
#include <klocale.h>

// default papersize in mm
#define PAPER_WIDTH 210.0
#define PAPER_HEIGHT 298.0

#define LAYER_VISIBLE   1
#define LAYER_EDITABLE  2
#define LAYER_PRINTABLE 4

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

GDocument::GDocument () {
  initialize ();
}

GDocument::~GDocument () {
  for_each (layers.begin (), layers.end (), del_obj<GLayer> ()); 
  layers.clear ();
  selection.clear ();
}

void GDocument::setAutoUpdate (bool flag) { 
  autoUpdate = flag; 
  if (autoUpdate) {
    selBoxIsValid = false;
    updateHandle ();
    emit changed ();
  }
}

void GDocument::initialize () {
  gridx = gridy = 20.0;
  snapToGrid = snapToHelplines = false;

  pLayout.format = PG_DIN_A4;
  pLayout.orientation = PG_PORTRAIT;
  pLayout.mmWidth = PG_A4_WIDTH;
  pLayout.mmHeight = PG_A4_HEIGHT;
  pLayout.left = 0; pLayout.right = 0;
  pLayout.top = 0; pLayout.bottom = 0;
  pLayout.unit = PG_MM;

  // in pt !!
  paperWidth = (int) cvtMmToPt (pLayout.mmWidth);
  paperHeight = (int) cvtMmToPt (pLayout.mmHeight);
  last = NULL;
  modifyFlag = false;
  filename = UNNAMED_FILE;

  selection.clear ();
  if (! layers.empty ()) {
    vector<GLayer*>::iterator i = layers.begin ();
    for (; i != layers.end (); i++)
      delete *i;
    layers.clear ();
  }
  active_layer = addLayer ();

  selBoxIsValid = false;
  autoUpdate = true;
  emit changed ();
}

void GDocument::setModified (bool flag) {
  modifyFlag = flag;
  emit wasModified (flag);
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

void GDocument::drawContents (Painter& p, bool withBasePoints, bool outline) {
  vector<GLayer*>::iterator i = layers.begin ();
  for (; i != layers.end (); i++) {
    GLayer* layer = *i;
    if (layer->isVisible ()) {
      const list<GObject*>& contents = layer->objects ();
      for (list<GObject*>::const_iterator oi = contents.begin ();
	   oi != contents.end (); oi++)
	(*oi)->draw (p, withBasePoints && (*oi)->isSelected (), outline);
    }
  }
}

void GDocument::drawContentsInRegion (Painter& p, const Rect& r, 
				      bool withBasePoints, bool outline) {
  vector<GLayer*>::iterator i = layers.begin ();
  for (; i != layers.end (); i++) {
    GLayer* layer = *i;
    if (layer->isVisible ()) {
      const list<GObject*>& contents = layer->objects ();
      for (list<GObject*>::const_iterator oi = contents.begin ();
	   oi != contents.end (); oi++) {
	// draw the object only if its bounding box 
	// intersects the active region 
	//	const Rect& bbox = (*oi)->boundingBox ();
	//	if (r.intersects (bbox))
        if ((*oi)->intersects (r))
	  (*oi)->draw (p, withBasePoints && (*oi)->isSelected (), outline);
      }
    }
  }
}

unsigned int GDocument::objectCount () const { 
  unsigned int num = 0;
  vector<GLayer*>::const_iterator i = layers.begin ();
  for (; i != layers.end (); i++) 
    num += (*i)->objectCount ();
  return num;
}

void GDocument::insertObject (GObject* obj) {
  obj->ref ();
  active_layer->insertObject (obj);
  connect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
  connect (obj, SIGNAL(changed(const Rect&)), 
	   this, SLOT(objectChanged (const Rect&)));
  setModified ();
  if (autoUpdate)
    emit changed ();
}

void GDocument::selectObject (GObject* obj) {
  list<GObject*>::iterator i = find (selection.begin (), selection.end (),
				     obj);
  if (i == selection.end ()) {
    // object isn't yet in selection list
    obj->select (true);
    selection.push_back (obj);
    selBoxIsValid = false;
    updateHandle ();
    if (autoUpdate) {
      emit changed ();
      emit selectionChanged ();
    }
  }
}

void GDocument::unselectObject (GObject* obj) {
  list<GObject*>::iterator i = find (selection.begin (), selection.end (),
				     obj);
  if (i != selection.end ()) {
    // remove object from the selection list
    obj->select (false);
    selection.erase (i);
    selBoxIsValid = false;
    updateHandle ();
    if (autoUpdate) {
      emit changed ();
      emit selectionChanged ();
    }
  }
}

void GDocument::unselectAllObjects () {
  if (selection.empty ())
    return;

  for_each (selection.begin (), selection.end (), unselect_obj ());
  selection.clear ();
  selBoxIsValid = false;
  if (autoUpdate) {
    emit changed ();
    emit selectionChanged ();
  }
}

void GDocument::selectAllObjects () {
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
  selBoxIsValid = false;
  updateHandle ();
  if (autoUpdate) {
    emit changed ();
    emit selectionChanged ();
  }
}

void GDocument::setLastObject (GObject* obj) {
  if (obj == 0L || obj->getLayer () != 0L)
    last = obj;
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
      list<GObject*>::iterator i = selection.begin ();
      selBox = (*i++)->boundingBox ();
      for (; i != selection.end (); i++)
        selBox = selBox.unite ((*i)->boundingBox ());
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
  return box;
}

void GDocument::deleteSelectedObjects () {
  if (! selectionIsEmpty ()) {
    for (list<GObject*>::iterator i = selection.begin ();
	 i != selection.end (); i++) {
      GObject* obj = *i;
      disconnect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
      disconnect (obj, SIGNAL(changed(const Rect&)), 
		  this, SLOT(objectChanged (const Rect&)));
      obj->getLayer ()->deleteObject (obj);
    }
    selection.clear ();
    last = 0L;
    setModified ();
    selBoxIsValid = false;
    if (autoUpdate) {
      emit changed ();
      emit selectionChanged ();
    }
  }
}

void GDocument::deleteObject (GObject* obj) {
  bool selected = false;

  GLayer* layer = obj->getLayer ();
  assert (layer);
  if (layer->isEditable ()) {
    selected = obj->isSelected ();
    if (selected) 
      selection.remove (obj);
    last = 0L;
    disconnect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
    disconnect (obj, SIGNAL(changed(const Rect&)), 
		this, SLOT(objectChanged (const Rect&)));
    layer->deleteObject (obj);
    if (selected) {
      selBoxIsValid = false;
      updateHandle ();
      if (autoUpdate)
	emit selectionChanged ();
    }
    if (autoUpdate)
      emit changed ();
  } 
}

/**
 * Looks for an object of type <tt>otype</tt> which endpoints are distant
 * not more than <tt>max_dist</tt> from the point <tt>x, y</tt>.
 * The method returns <tt>true</tt> if an object was found as well as
 * the object in <tt>obj</tt> and the index of the nearest point in
 * <tt>pidx</tt>.
 */
bool GDocument::findNearestObject (const char* otype, int x, int y,
				   float max_dist, GObject*& obj, 
				   int& pidx, bool all) {
  float d, distance = MAXFLOAT;
  obj = 0L;
  Coord p (x, y);

  for (vector<GLayer*>::reverse_iterator li = layers.rbegin (); 
       li != layers.rend (); li++) {
    GLayer* layer = *li;
    if (layer->isEditable ()) {
      list<GObject*>& contents = layer->objects ();
      for (list<GObject*>::iterator oi = contents.begin ();
	   oi != contents.end (); oi++) {
	if (otype == 0L || (*oi)->isA (otype)) {
	  if ((*oi)->findNearestPoint (p, max_dist, d, pidx, all) &&
	      d < distance) {
	    obj = *oi;
	    distance = d;
	  }
	}
      }
    }
  }
  if (obj == 0L)
    pidx = -1;
  return obj != 0L;
}

GObject* GDocument::findContainingObject (int x, int y) {
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
  return result;
}

bool GDocument::findContainingObjects (int x, int y, QList<GObject>& olist) {
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
  return olist.count () > 0;
}

bool GDocument::findObjectsContainedIn (const Rect& r, QList<GObject>& olist) {
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
  return olist.count () > 0;
}

void GDocument::layerChanged () {
  if (!autoUpdate)
    return;

  emit changed ();
}

void GDocument::objectChanged () {
  if (!autoUpdate)
    return;

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

void GDocument::objectChanged (const Rect& r) {
  if (!autoUpdate)
    return;

  if (! selectionIsEmpty ()) {
    selBoxIsValid = false;
    updateHandle ();
    /*
    GObject* obj = (GObject *) sender ();
    if (obj->isSelected () && autoUpdate) {
      emit selectionChanged ();
    }
    */
  }
  setModified ();
  if (autoUpdate)
      emit changed (r);
}

bool GDocument::saveToXml (ostream& os) {
  static const char* formats[] = {
    "a3", "a4", "a5", "us_letter", "us_legal", "screen", "custom"
  };
  static const char* orientations[] = {
    "portrait", "landscape"
  };

  XmlWriter xml (os);

  xml.startTag ("doc", false);
  //  xml.addAttribute ("author", "Kai-Uwe Sattler");
  //  xml.addAttribute ("email", "kus@iti.cs.uni-magdeburg.de");
  xml.addAttribute ("editor", "KIllustrator");
  xml.addAttribute ("mime", KILLUSTRATOR_MIMETYPE);
  xml.addAttribute ("comment",(const char *)comment);
  xml.addAttribute ("keywords",(const char *)keywords);
  xml.closeTag ();

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

  xml.startTag ("grid", false);
  xml.addAttribute ("dx", gridx);
  xml.addAttribute ("dy", gridy);
  xml.addAttribute ("align", snapToGrid ? 1 : 0);
  xml.closeTag (true);

  xml.startTag ("helplines", false);
  xml.addAttribute ("align", snapToHelplines ? 1 : 0);
  xml.closeTag ();
  vector<float>::iterator hi;
  for (hi = hHelplines.begin (); hi != hHelplines.end (); hi++) {
    xml.startTag ("hl", false);
    xml.addAttribute ("pos", *hi);
    xml.closeTag (true);
  }
  for (hi = vHelplines.begin (); hi != vHelplines.end (); hi++) {
    xml.startTag ("vl", false);
    xml.addAttribute ("pos", *hi);
    xml.closeTag (true);
  }
  xml.endTag ();

  xml.endTag (); // </head>

  bool save_layer_info = (layers.size () > 1);
  for (vector<GLayer*>::iterator li = layers.begin (); 
       li != layers.end (); li++) {
    if (save_layer_info) {
      int flags = ((*li)->isVisible () ? LAYER_VISIBLE : 0) +
	  ((*li)->isPrintable () ? LAYER_PRINTABLE : 0) +
	  ((*li)->isEditable () ? LAYER_EDITABLE : 0);
      xml.startTag ("layer", false);
      xml.addAttribute ("id", (*li)->name ());
      xml.addAttribute ("flags", flags);
      xml.closeTag ();
    }
    list<GObject*>& contents = (*li)->objects ();
    for (list<GObject*>::iterator oi = contents.begin ();
	 oi != contents.end (); oi++)
      (*oi)->writeToXml (xml);
    if (save_layer_info)
      xml.endTag (); // </layer>
  }

  xml.endTag (); // </doc>

  setModified (false);
  return ! os.fail ();
}

bool GDocument::insertFromXml (istream& is, list<GObject*>& newObjs) {
  XmlReader xml (is);
  XmlElement elem;

  if (! xml.validHeader ())
    return false;

  if (! xml.readElement (elem) || (elem.tag () != "doc"))
    return false;

  // check mime type
  list<XmlAttribute>::const_iterator first =  elem.attributes ().begin ();
  while (first != elem.attributes ().end ()) {
      if ((*first).name () == "mime") {
	  const string& v = (*first).stringValue ();
	  if (v != KILLUSTRATOR_MIMETYPE)
	      return false;
      }
      first++;
  }
  return parseBody (xml, newObjs, true);
}

bool GDocument::parseBody (XmlReader& xml, list<GObject*>& newObjs,
			   bool markNew) {
  GObject* obj = 0L;
  stack<GGroup*, vector<GGroup*> > groups;
  bool finished = false;
  XmlElement elem;
  bool endOfBody = false;
  map<string, GObject*> idtable;

  do {
    if (! xml.readElement (elem))
      break;
    if (elem.tag () == "kiml" || elem.tag () == "doc") {
      if (! elem.isEndTag ())
        break;
      else
        endOfBody = true;
    }
    else if (elem.isEndTag ()) {
      finished = true;
      if (elem.tag () == "group") {
	  // group object is finished -> recalculate bbox
	groups.top ()->calcBoundingBox ();
	groups.pop ();
      }
    }
    else {
      finished = elem.isClosed () && elem.tag () != "point";

      if (elem.tag () == "layer") {
	  if (layers.size () == 1 && active_layer->objectCount () == 0) 
	      // add objects to the current layer
	      ;
	  else 
	      active_layer = addLayer ();
	  list<XmlAttribute>::const_iterator first = 
	      elem.attributes ().begin ();
	  while (first != elem.attributes ().end ()) {
	      const string& attr = (*first).name ();
	      if (attr == "id")
		  active_layer->setName ((*first).stringValue ().c_str ());
	      else if (attr == "flags") {
		  int flags = (*first).intValue ();
		  active_layer->setVisible (flags & LAYER_VISIBLE);
		  active_layer->setPrintable (flags & LAYER_EDITABLE);
		  active_layer->setEditable (flags & LAYER_PRINTABLE);
	      }
	      first++;
	  }
      }
      else if (elem.tag () == "polyline")
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
      else if (elem.tag () == "pixmap")
	obj = new GPixmap (elem.attributes ());
      else if (elem.tag () == "curve") {
	obj = new GCurve (elem.attributes ());
	finished = false;
	if (! xml.readElement (elem))
	  // something goes wrong
	  return false;

	do {
	  GSegment::Kind kind = GSegment::sk_Line;
	  if (elem.tag () != "seg") 
	    return false;

	  list<XmlAttribute>::const_iterator first = 
	    elem.attributes ().begin ();
	  while (first != elem.attributes ().end ()) {
	    const string& attr = (*first).name ();
	    if (attr == "kind")
	      kind = (GSegment::Kind) (*first).intValue ();
	    first++;
	  }
	  GSegment seg (kind);
	  if (kind == GSegment::sk_Line) {
	    for (int i = 0; i < 2; i++) {
	      Coord p;
	      if (! xml.readElement (elem) || elem.tag () != "point") 
		return false;
	      first = elem.attributes ().begin ();
	      
	      while (first != elem.attributes ().end ()) {
		if ((*first).name () == "x")
		  p.x ((*first).floatValue ());
		else if ((*first).name () == "y")
		  p.y ((*first).floatValue ());
		first++;
	      }
	      seg.setPoint (i, p);
	    }
	  }
	  else {
	    for (int i = 0; i < 4; i++) {
	      Coord p;
	      if (! xml.readElement (elem) || elem.tag () != "point") 
		return false;
	      first = elem.attributes ().begin ();
	      
	      while (first != elem.attributes ().end ()) {
		if ((*first).name () == "x")
		  p.x ((*first).floatValue ());
		else if ((*first).name () == "y")
		  p.y ((*first).floatValue ());
		first++;
	      }
	      seg.setPoint (i, p);
	    }
	  }
	  if (! xml.readElement (elem) || elem.tag () != "seg" || 
	      ! elem.isEndTag ()) 
	    return false;
	  ((GCurve *) obj)->addSegment (seg);

	  if (! xml.readElement (elem))
	    return false;
	  // end of element
	  if (elem.tag () == "curve" && elem.isEndTag ()) 
	    finished = true;
	} while (! finished);
      }
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

	if (!groups.empty ()) {
	  groups.top ()->addObject (group);
	}
	else {
	    if (markNew)
		newObjs.push_back (group);
	  insertObject (group);
	}
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
      else {
	GObject *proto = GObject::lookupPrototype (elem.tag ().c_str ());
	if (proto != 0L) {
	  obj = proto->clone (elem.attributes ());
	}
	else
	  cout << "invalid object type: " << elem.tag () << endl;
      }
    }
    if (finished) {
      if (obj) {
        if (!groups.empty ()) {
	  obj->setLayer (active_layer);
 	  groups.top ()->addObject (obj);
        }
        else { 
	  if (markNew)
	    newObjs.push_back (obj);
	  if (obj->hasId ())
	    idtable.insert (pair<string, GObject*> (obj->getId (), obj));
	  
	  insertObject (obj);
	}
        obj = 0L;
      }
      finished = false;
    }
  } while (! endOfBody);

  // update object connections
  vector<GLayer*>::iterator i = layers.begin ();
  for (; i != layers.end (); i++) {
    GLayer* layer = *i;
    list<GObject*>& contents = layer->objects ();
    for (list<GObject*>::iterator oi = contents.begin ();
	 oi != contents.end (); oi++) {
      // this should be more general !!
      if ((*oi)->hasRefId () && (*oi)->isA ("GText")) {
	const char* id = (*oi)->getRefId ();
	map<string, GObject*>::iterator mi = idtable.find (id);
	if (mi != idtable.end ()) {
	  GText *tobj = (GText *) *oi;
	  tobj->setPathObject (mi->second);
	}
      }
    }
  }

  setAutoUpdate (true);
  return true;
}

bool GDocument::readFromXml (istream& is) {
  bool endOfHeader = false;

  XmlReader xml (is);
  if (! xml.validHeader ()) {
    return false;
  }
  /*
  if (xml.doctype () != XML_DOCTYPE || xml.dtd () != XML_DTD) {
    cout << "Wrong doctype !" << endl;
    return false;
  }
  */
  XmlElement elem;
  if (! xml.readElement (elem) || 
      (elem.tag () != "kiml" && // for backward compatibility
       elem.tag () != "doc")) 
    return false;

  if (elem.tag () == "doc") {
    // check mime type
    list<XmlAttribute>::const_iterator first = 
      elem.attributes ().begin ();
    QString strComment="", strKeywords="";
    while (first != elem.attributes ().end ()) {
      if ((*first).name () == "mime") {
	const string& v = (*first).stringValue ();
	if (v != KILLUSTRATOR_MIMETYPE)
	  return false;
      }
      if ((*first).name () == "comment") {//TB: Note This is not unicode!
	strComment =  (*first).stringValue().c_str();
	debug(":"+strComment+":");
      }
      if ((*first).name () == "keywords") { //TB: Note this isn't unicode!
        strKeywords = (*first).stringValue().c_str();
	debug(":"+strKeywords+":");
      }
      first++;
    }
    comment  = strComment;
    keywords = strKeywords;
  }

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
    else if (elem.tag () == "grid") {
      list<XmlAttribute>::const_iterator first = 
	elem.attributes ().begin ();
       while (first != elem.attributes ().end ()) {
        if ((*first).name () == "dx")
	  gridx = (*first).floatValue ();
        else if ((*first).name () == "dy")
	  gridy = (*first).floatValue ();
        else if ((*first).name () == "align")
	  snapToGrid = ((*first).intValue () == 1);
        first++;
      }
    }
    else if (elem.tag () == "helplines") {
      bool endOfHelplines = false;
      list<XmlAttribute>::const_iterator first = 
	elem.attributes ().begin ();
       while (first != elem.attributes ().end ()) {
	 if ((*first).name () == "align")
	  snapToHelplines = ((*first).intValue () == 1);
        first++;
      }

      while (!endOfHelplines) {
	if (! xml.readElement (elem))
	  return false;
	
	if (elem.tag () == "helplines" && elem.isEndTag ()) {
	  endOfHelplines = true;
	  continue;
	}
	else if (elem.tag () == "hl") {
	  list<XmlAttribute>::const_iterator first = 
	    elem.attributes ().begin ();
	  while (first != elem.attributes ().end ()) {
	    if ((*first).name () == "pos")
	      hHelplines.push_back ((*first).floatValue ());
	    first++;
	  }
	}
	else if (elem.tag () == "vl") {
	  list<XmlAttribute>::const_iterator first = 
	    elem.attributes ().begin ();
	  while (first != elem.attributes ().end ()) {
	    if ((*first).name () == "pos")
	      vHelplines.push_back ((*first).floatValue ());
	    first++;
	  }
	}
      }
    }
    else if (elem.tag () == "head" && elem.isEndTag ())
      endOfHeader = true;
    else
      return false;
  } while (! endOfHeader);

  // update page layout
  setPageLayout (pLayout);

  list<GObject*> dummy;
  bool result = parseBody (xml, dummy, false);

  setModified (false);
  emit gridChanged ();
  return result;
}

unsigned int GDocument::findIndexOfObject (GObject *obj) {
  assert (obj->getLayer () != 0L);
  return obj->getLayer ()->findIndexOfObject (obj);
}

void GDocument::insertObjectAtIndex (GObject* obj, unsigned int idx) {
  obj->ref ();
  GLayer* layer = obj->getLayer ();
  if (layer == 0L)
    layer = active_layer;
  layer->insertObjectAtIndex (obj, idx);
  connect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
  connect (obj, SIGNAL(changed(const Rect&)), 
	   this, SLOT(objectChanged (const Rect&)));
  setModified ();
  if (autoUpdate) {
    emit changed ();
    emit selectionChanged ();
  }
}

void GDocument::moveObjectToIndex (GObject* obj, unsigned int idx) {
  GLayer* layer = obj->getLayer ();
  if (layer == 0L)
    layer = active_layer;
  layer->moveObjectToIndex (obj, idx);

  setModified ();
  if (autoUpdate) {
    emit changed ();
    emit selectionChanged ();
  }
}

KoPageLayout GDocument::pageLayout () {
  return pLayout;
}

void GDocument::setPageLayout (const KoPageLayout& layout) {
  pLayout = layout;
  switch (layout.unit) {
  case PG_MM:
    paperWidth = (int) cvtMmToPt (pLayout.mmWidth);
    paperHeight = (int) cvtMmToPt (pLayout.mmHeight);
    break;
  case PG_PT:
    paperWidth = pLayout.ptWidth;
    paperHeight = pLayout.ptHeight;
    break;
  case PG_INCH:
    paperWidth = (int) cvtInchToPt (pLayout.inchWidth);
    paperHeight = (int) cvtInchToPt (pLayout.inchHeight);
    break;
  }
  modifyFlag = true;
  emit sizeChanged ();
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
  GLayer* layer = new GLayer (this);
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
  emit selectionChanged ();
  emit changed ();
}

void GDocument::printInfo (QString& s) {
    ostrstream os;
    int n = 0;

    for (vector<GLayer*>::iterator li = layers.begin (); 
	 li != layers.end (); li++) {
	GLayer* layer = *li;
	list<GObject*>& contents = layer->objects ();
	n += contents.size ();
    }
    os << i18n ("Document") << ": "<< (const char *) filename << '\n'
       << i18n ("Layers") << ": " << layers.size () << '\n'
       << i18n ("Objects") << ": " << n << ends;
    s += os.str ();
}
  
void GDocument::invalidateClipRegions () {
  for (vector<GLayer*>::iterator li = layers.begin (); 
       li != layers.end (); li++) {
    GLayer* layer = *li;
    if (layer->isVisible ()) {
      list<GObject*>& contents = layer->objects ();
      list<GObject*>::iterator oi = contents.begin ();
      for (; oi != contents.end (); oi++)
	(*oi)->invalidateClipRegion ();
    }
  }
}

void GDocument::setGrid (float dx, float dy, bool snap) {
  gridx = dx;
  gridy = dy;
  snapToGrid = snap;
}

void GDocument::getGrid (float& dx, float& dy, bool& snap) {
  dx = gridx;
  dy = gridy;
  snap = snapToGrid;
}

void GDocument::setHelplines (const vector<float>& hlines, 
			      const vector<float>& vlines,
			      bool snap) {
  hHelplines = hlines;
  vHelplines = vlines;
  snapToHelplines = snap;
}

void GDocument::getHelplines (vector<float>& hlines, vector<float>& vlines,
			      bool& snap) {
  hlines = hHelplines;
  vlines = vHelplines;
  snap = snapToHelplines;
}

void GDocument::setComment(QString s){
  comment = s;
}

void GDocument::getComment(QString &s){
  s = comment;
}

void GDocument::setKeywords(QString s){
  keywords = s;
}

void GDocument::getKeywords(QString &s){
  s = keywords;
}
