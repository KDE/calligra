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

// #include "KIllustrator.h"
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

#include <stack>
#include <vector>

#include "xmlutils/XmlWriter.h"
#include "xmlutils/XmlReader.h"

// default papersize in mm
#define PAPER_WIDTH 210.0
#define PAPER_HEIGHT 298.0

GDocument::GDocument () {
  initialize ();
}

GDocument::~GDocument () {
  if (! objects.isEmpty ()) {
    QListIterator<GObject> it (objects);
    for (; it.current (); ++it)
      it.current ()->unref ();
    objects.clear ();
  }
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

  if (! selection.isEmpty ())
    selection.clear ();
  if (! objects.isEmpty ()) {
    QListIterator<GObject> it (objects);
    for (; it.current (); ++it)
      it.current ()->unref ();
    objects.clear ();
  }
  selBoxIsValid = false;
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

void GDocument::insertObject (GObject* obj) {
  objects.append (obj);
  connect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
  setModified ();
  emit changed ();
}

void GDocument::selectObject (GObject* obj) {
  if (selection.findRef (obj) == -1) {
    obj->select (true);
    selection.append (obj);
    selBoxIsValid = false;
    updateHandle ();
    emit changed ();
    emit selectionChanged ();
  }
}

void GDocument::unselectObject (GObject* obj) {
  if (selection.removeRef (obj)) {
    obj->select (FALSE);
    selBoxIsValid = false;
    updateHandle ();
    emit changed ();
    emit selectionChanged ();
  }
}

void GDocument::unselectAllObjects () {
  if (selection.isEmpty ())
    return;

  QListIterator<GObject> it (selection);
  for (; it.current (); ++it) {
    it.current ()->select (false);
  }
  selection.clear ();
  selBoxIsValid = false;
  emit changed ();
  emit selectionChanged ();
}

void GDocument::selectAllObjects () {
  selection.clear ();
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    it.current ()->select (true);
    selection.append (it.current ());
  }
  selBoxIsValid = false;
  updateHandle ();
  emit changed ();
  emit selectionChanged ();
}

void GDocument::setLastObject (GObject* obj) {
  if (obj == NULL || objects.findRef (obj) != -1) 
    last = obj;
}

void GDocument::updateHandle () {
  Rect r = boundingBoxForSelection ();
  if (r.empty ())
    selHandle.show (false);
  else
    selHandle.setBox (r);
}

Rect GDocument::boundingBoxForSelection () {
  if (! selBoxIsValid) {
    if (! selectionIsEmpty ()) {
      QListIterator<GObject> it (selection);
      selBox = it.current ()->boundingBox ();
      ++it;
      for (; it.current (); ++it)
        selBox = selBox.unite (it.current ()->boundingBox ());
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

  QListIterator<GObject> it (objects);
  box = it.current ()->boundingBox ();
  ++it;
  for (; it.current (); ++it)
    box = box.unite (it.current ()->boundingBox ());
  return box;
}

void GDocument::deleteSelectedObjects () {
  if (! selectionIsEmpty ()) {
    GObject* obj = objects.first ();
    while (obj != NULL) {
      if (obj->isSelected ()) {
	obj->unref ();
	objects.remove ();
	obj = objects.current ();
      }
      else
	obj = objects.next ();
    }
    //    selection.setAutoDelete (true);
    selection.clear ();
    //    selection.setAutoDelete (false);
    last = 0L;
    setModified ();
    selBoxIsValid = false;
    emit changed ();
    emit selectionChanged ();
  }
}

void GDocument::deleteObject (GObject* obj) {
  bool selected = false;

  if (objects.findRef (obj) != -1) {
    if (obj->isSelected ()) {
      selected = true;
      selection.removeRef (obj);
    }
    objects.removeRef (obj);
    last = 0L;
    obj->unref ();
    if (selected) {
      selBoxIsValid = false;
      updateHandle ();
      emit selectionChanged ();
    }
    emit changed ();
  }
}

GObject* GDocument::findContainingObject (int x, int y) {
  QListIterator<GObject> it (objects);
  // We are looking for the most relevant object, that means the object 
  // in front of all others. So, we have to start at the end of the
  // list ... 
  it.toLast ();
  for (; it.current (); --it) {
    if (it.current ()->contains (Coord (x, y)))
      return it.current ();
  }
  // nothing found
  return 0;
}

bool GDocument::findContainingObjects (int x, int y, QList<GObject>& olist) {
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    if (it.current ()->contains (Coord (x, y)))
      olist.append (it.current ());
  }
  return olist.count () > 0;
}

bool GDocument::findObjectsContainedIn (const Rect& r, QList<GObject>& olist) {
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) {
    if (r.contains (it.current ()->boundingBox ()))
      olist.append (it.current ());
  }
  return olist.count () > 0;
}

void GDocument::objectChanged () {
  if (! selectionIsEmpty ()) {
    selBoxIsValid = false;
    updateHandle ();
    GObject* obj = (GObject *) sender ();
    if (obj->isSelected ()) {
      emit selectionChanged ();
    }
  }
  setModified ();
  emit changed ();
}

bool GDocument::saveToXml (const char* fname) {
  static const char* formats[] = {
    "a3", "a4", "a5", "us_letter", "us_legal", "screen", "custom"
  };
  static const char* orientations[] = {
    "portrait", "landscape"
  };
  static const char* units[] = {
    "mm", "cm", "inch"
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
  xml.addAttribute ("unit", "pt");
  xml.closeTag (true);

  xml.endTag (); // </head>

  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) 
    it.current ()->writeToXml (xml);

  xml.endTag (); // </kiml>

  setModified (false);
  return ! os.fail ();
}

#if 0
bool GDocument::saveToFile (QFile& file) {
  if (! file.open (IO_WriteOnly))
    return false;

  filename = file.name ();
  QDataStream strm (&file);
  
  /*
   * file identifier
   */
  strm << FILE_ID_STRING;

  /*
   * save the document properties
   */
  strm << (Q_INT8) SECTION_ID_DOCUMENT;
  // paper size
  strm << (Q_INT8) pLayout.format << (Q_INT8) pLayout.orientation
       << pLayout.width << pLayout.height;
  // margins
  strm << pLayout.left << pLayout.right
       << pLayout.top << pLayout.bottom;
  strm << (Q_INT8) pLayout.unit;

  /*
   * save the dictionary of user-defined arrows
   */
  strm << (Q_INT8) SECTION_ID_ARROWS;
  // the number of arrows
  strm << (Q_UINT32) 0;

  /*
   * save the dictionary of user-defined line styles
   */
  strm << (Q_INT8) SECTION_ID_LINESTYLES;
  // the number of styles
  strm << (Q_UINT32) 0;

  /*
   * save the dictionary of user-defined fill styles
   */
  strm << (Q_INT8) SECTION_ID_FILLSTYLES;
  // the number of styles
  strm << (Q_UINT32) 0;

  /*
   * now save the objects
   */
  strm << (Q_INT8) SECTION_ID_OBJECTS;
 
  QListIterator<GObject> it (objects);
  for (; it.current (); ++it) 
    it.current ()->write (strm);

  file.close ();
  setModified (false);
  return true;
}
#endif

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
        else if ((*first).name () == "unit")
	  ;
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
      return false;
    if (elem.tag () == "kiml") {
      if (! elem.isEndTag ())
        return false;
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
	  return false;

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
	    return false;
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
	group->ref ();
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
	obj->ref ();
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

  return true;
}

#if 0
bool GDocument::restoreFromFile (QFile& file) {
  Q_INT8 id, value8; 
  Q_UINT32 value32;

  if (! file.open (IO_ReadOnly))
    return false;
  QDataStream strm (&file);
  
  char *code;
  strm >> code;
  if (::strcmp (code, FILE_ID_STRING)) {
    delete code;
    return false;
  }
  delete code;

  /*
   * restore document properties
   */
  strm >> id;
  if (id != SECTION_ID_DOCUMENT)
    return false;
  // paper size
  strm >> value8;
  pLayout.format = value8;
  strm >> value8;
  pLayout.orientation = value8;
  strm >> pLayout.width >> pLayout.height;
  // margins
  strm >> pLayout.left >> pLayout.right
       >> pLayout.top >> pLayout.bottom;
  strm >> value8;
  pLayout.unit = value8;
  setPageLayout (pLayout);

  if (strm.eof ())
    return false;

  /*
   * restore user-defined arrows
   */
  strm >> id;
  if (id != SECTION_ID_ARROWS)
    return false;
  strm >> value32;
  if (strm.eof ())
    return false;

  /*
   * restore user-defined line styles
   */
  strm >> id;
  if (id != SECTION_ID_LINESTYLES)
    return false;
  strm >> value32;
  if (strm.eof ())
    return false;

  /*
   * restore user-defined fill styles
   */
  strm >> id;
  if (id != SECTION_ID_FILLSTYLES)
    return false;
  strm >> value32;
  if (strm.eof ())
    return false;

  /*
   * now restore the objects
   */

  strm >> id;
  if (id != SECTION_ID_OBJECTS)
    return false;

  while (! strm.eof ()) {
    GObject* obj = GObject::restore (strm);
    if (obj != 0L)
      insertObject (obj);
  }
  file.close ();
  setModified (false);
  filename = file.name ();
  return true;
}
#endif

unsigned int GDocument::findIndexOfObject (GObject *obj) {
  return objects.findRef (obj);
}

void GDocument::insertObjectAtIndex (GObject* obj, unsigned int idx) {
  if (idx > objects.count ())
    idx = objects.count ();
  objects.insert (idx, obj);
  connect (obj, SIGNAL(changed()), this, SLOT(objectChanged ()));
  setModified ();
  emit changed ();
  emit selectionChanged ();
}

void GDocument::moveObjectToIndex (GObject* obj, unsigned int idx) {
  int pos;

  if ((pos = objects.findRef (obj)) == -1)
    return;
  
  objects.take (pos);
  objects.insert (idx, obj);

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

