/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 2000 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

/*
 * TODO:
 * - multi-lined text
 * - font styles (bold, italic etc.)
 * - text on path
 * - arcs
 * - pies
 * - fill patterns
 * - gradients
 * - pixmaps
 * - line styles
 * - arrows
 */
#include <fstream.h>
#include <strstream.h>
#include "GDocument.h"
#include "xmlutils/XmlWriter.h"
#include "SVGExport.h"

#include "GPolygon.h"
#include "GPolyline.h"
#include "GOval.h"
#include "GCurve.h"
#include "GBezier.h"
#include "GGroup.h"
#include "GText.h"
#include "GPixmap.h"

SVGExport::SVGExport () {
}

SVGExport::~SVGExport () {
}

bool SVGExport::setup (GDocument *, const char*) {
  return true;
}

bool SVGExport::exportToFile (GDocument* doc) { 
  char buf[200];

  ofstream os (outputFileName ());
  if (os.fail ())
    return false;

  XmlWriter xw (os, false);
  /*  xw.docType ("svg PUBLIC \"-//W3C//DTD SVG December 1999//EN\"\n\"http:://www.w3.org/Graphics/SVG/SVG-19991203.dtd\"");
   */
  xw.docType ("svg SYSTEM \"svg-19990812.dtd\"");

  xw.startTag ("svg", false);
  sprintf (buf, "%dpt", doc->getPaperWidth ());
  xw.addAttribute ("width", buf);
  sprintf (buf, "%dpt", doc->getPaperHeight ());
  xw.addAttribute ("height", buf);
  xw.closeTag ();

  // contents
  const vector<GLayer*>& layers = doc->getLayers ();
  for (vector<GLayer*>::const_iterator li = layers.begin (); 
       li != layers.end (); li++) {
    if ((*li)->isInternal ())
      continue;

    const list<GObject*>& contents = (*li)->objects ();
    for (list<GObject*>::const_iterator oi = contents.begin ();
	 oi != contents.end (); oi++) {
      exportObject (xw, *oi);
    }
  }

  xw.endTag ();
  return false;
}

void SVGExport::exportObject (XmlWriter& xw, GObject* obj) {
  if (obj->isA ("GPolygon")) 
    exportPolygon (xw, (GPolygon *) obj);
  else if (obj->isA ("GPolyline")) 
    exportPolyline (xw, (GPolyline *) obj);
  else if (obj->isA ("GText")) 
    exportText (xw, (GText *) obj);
  else if (obj->isA ("GOval")) 
    exportEllipse (xw, (GOval *) obj);
  else if (obj->isA ("GBezier")) 
    exportBezier (xw, (GBezier *) obj);
  else if (obj->isA ("GGroup"))
    exportGroup (xw, (GGroup *) obj);
  else if (obj->isA ("GCurve"))
    exportCurve (xw, (GCurve *) obj);
  else if (obj->isA ("GPixmap")) 
    exportPixmap (xw, (GPixmap *) obj);
}

void SVGExport::exportText (XmlWriter& xw, GText* obj) {
  if (obj->lines () > 1) {
    GText::TextInfo tInfo = obj->getTextInfo ();
    /*
    xw.startTag ("g", false);
    addTransformationAttribute (xw, obj);
    xw.closeTag ();
    */
    QFontMetrics fm (tInfo.font);
    float yoff = fm.ascent ();

    for (int i = 0; i < obj->lines (); i++) {
      int ws = fm.width (obj->line (i));
      float xoff = 0;
      if (tInfo.align == GText::TextInfo::AlignCenter)
	xoff = -ws / 2;
      else if (tInfo.align == GText::TextInfo::AlignRight)
	xoff = -ws;
      exportTextLine (xw, obj, i, xoff, yoff);
      yoff += fm.height ();
    }

    //    xw.endTag ();
  }
  else 
    exportTextLine (xw, obj, 0, 0, 0);
}

void SVGExport::exportTextLine (XmlWriter& xw, GText* obj, int line,
				float xoff, float yoff) {
  QString text = obj->line (line);
  xw.startTag ("text", false);
  xw.addAttribute ("x", xoff);
  xw.addAttribute ("y", yoff);
  addTransformationAttribute (xw, obj);
  addTextStyleAttribute (xw, obj);
  xw.closeTag ();
  xw.writeText ((const char *) text);
  xw.endTag ();
}

void SVGExport::exportBezier (XmlWriter& xw, GBezier* obj) {
  xw.startTag ("path", false);
  ostrstream s;
  Coord p = obj->getPoint (1);
  s << "M " << p.x () << " " << p.y () << " ";
  unsigned int i = 2;
  while (i < obj->numOfPoints () - 2) {
    s << "C ";
    for (int n = 0; n < 3; n++) {
      p = obj->getPoint (i + n);
      s << p.x () << " " << p.y () << " ";
    }
    i += 3;
  }
  s << ends;
  xw.addAttribute ("d", s.str ());
  addTransformationAttribute (xw, obj);
  addStyleAttribute (xw, obj);
  xw.closeTag (true);
}

void SVGExport::exportCurve (XmlWriter& xw, GCurve* obj) {
  xw.startTag ("path", false);
  bool first = true;
  ostrstream s;
  for (int i = 0; i < obj->numOfSegments (); i++) {
    const GSegment& seg = obj->getSegment (i);
    if (first) {
      s << "M " << seg.pointAt (0).x () << " " 
	<< seg.pointAt (0).y () << " ";
      first = false;
    }
    if (seg.kind () == GSegment::sk_Line) {
      s << "L " << seg.pointAt (1).x () << " " 
	<< seg.pointAt (1).y () << " ";
    }
    else if (seg.kind () == GSegment::sk_Bezier) {
      s << "C ";
      for (int n = 1; n < 4; n++)
	s << seg.pointAt (n).x () << " " 
	  << seg.pointAt (n).y () << " ";
    }
  }
  if (obj->isClosed ())
    s << "Z";

  s << ends;
  xw.addAttribute ("d", s.str ());
  addTransformationAttribute (xw, obj);
  addStyleAttribute (xw, obj);
  xw.closeTag (true);
}

void SVGExport::exportGroup (XmlWriter& xw, GGroup* obj) {
  xw.startTag ("g", false);
  addTransformationAttribute (xw, obj);
  addStyleAttribute (xw, obj);
  xw.closeTag ();
  const list<GObject*>& objs = obj->getMembers ();
  for (list<GObject*>::const_iterator i = objs.begin (); 
       i != objs.end (); i++) {
    exportObject (xw, *i);
  }
  
  xw.endTag ();
}

void SVGExport::exportPolyline (XmlWriter& xw, GPolyline* obj) {
  xw.startTag ("polyline", false);
  ostrstream s;
  for (unsigned int i = 0; i < obj->numOfPoints (); i++) {
    Coord p = obj->getPoint (i);
    s << p.x () << "," << p.y () << " ";
  }
  s << ends;
  xw.addAttribute ("points", s.str ());
  addTransformationAttribute (xw, obj);
  addStyleAttribute (xw, obj);
  xw.closeTag (true);
}

void SVGExport::exportEllipse (XmlWriter& xw, GOval* obj) {
  const Coord& p0 = obj->startPoint ();
  const Coord& p1 = obj->endPoint ();
  if (obj->isCircle ()) {
    xw.startTag ("circle", false);
    xw.addAttribute ("cx", (p1.x () + p0.x ()) / 2.0);
    xw.addAttribute ("cy", (p1.y () + p0.y ()) / 2.0);
    xw.addAttribute ("r", (p1.x () - p0.x ()) / 2.0);
    addTransformationAttribute (xw, obj);
    addStyleAttribute (xw, obj);
    xw.closeTag (true);
  }
  else {
    GObject::OutlineInfo oInfo = obj->getOutlineInfo ();
    if (oInfo.shape == GObject::OutlineInfo::DefaultShape) {
      xw.startTag ("ellipse", false);
      xw.addAttribute ("cx", (p1.x () + p0.x ()) / 2.0);
      xw.addAttribute ("cy", (p1.y () + p0.y ()) / 2.0);
      xw.addAttribute ("rx", (p1.x () - p0.x ()) / 2.0);
      xw.addAttribute ("ry", (p1.y () - p0.y ()) / 2.0);
      addTransformationAttribute (xw, obj);
      addStyleAttribute (xw, obj);
      xw.closeTag (true);
    }
    else if (oInfo.shape == GObject::OutlineInfo::ArcShape) {
      // TODO
    }
    else {
      // TODO
    }
  }

}

void SVGExport::exportPolygon (XmlWriter& xw, GPolygon* obj) {
  if (obj->isRectangle ()) {
    Coord p0 = obj->getPoint (0); 
    Coord p2 = obj->getPoint (2); 
    //    const QWMatrix& m = obj->matrix ();
    xw.startTag ("rect", false);
    xw.addAttribute ("x", p0.x ());
    xw.addAttribute ("y", p0.y ());
    xw.addAttribute ("width", p2.x () - p0.x ());
    xw.addAttribute ("height", p2.y () - p0.y ());
    addTransformationAttribute (xw, obj);
    addStyleAttribute (xw, obj);
    xw.closeTag (true);
  }
  else {
    xw.startTag ("polygon", false);
    ostrstream s;
    for (unsigned int i = 0; i < obj->numOfPoints (); i++) {
      Coord p = obj->getPoint (i);
      s << p.x () << "," << p.y () << " ";
    }
    s << ends;
    xw.addAttribute ("points", s.str ());
    addTransformationAttribute (xw, obj);
    addStyleAttribute (xw, obj);
    xw.closeTag (true);
  }
}

void SVGExport::exportPixmap (XmlWriter& xw, GPixmap* obj) {
}

void SVGExport::addTransformationAttribute (XmlWriter& xw, GObject* obj) {
  ostrstream s;
  QWMatrix im;
  
  const QWMatrix& m = obj->matrix ();
  if (m != im) {
    s << "matrix(" << m.m11 () << " " << m.m12 () << " " << m.m21 () 
      << " " << m.m22 () << " " << m.dx () << " " << m.dy () 
      << ")" << ends;
    xw.addAttribute ("transform", s.str ());
  }
}
 
void SVGExport::addStyleAttribute (XmlWriter& xw, GObject* obj) {
  ostrstream s;
  GObject::FillInfo fInfo = obj->getFillInfo ();
  GObject::OutlineInfo oInfo = obj->getOutlineInfo ();

  s << "fill:";
  if (fInfo.fstyle == GObject::FillInfo::NoFill || obj->isA ("GPolyline") ||
      (obj->isA ("GBezier") && ! ((GBezier *) obj)->isClosed ()) ||
      (obj->isA ("GCurve") && ! ((GCurve *) obj)->isClosed ()))
    s << "none ; " ;
  else if (fInfo.fstyle == GObject::FillInfo::SolidFill) {
    s << "rgb(" << fInfo.color.red () << "," << fInfo.color.green ()
      << "," << fInfo.color.blue () << ") ; ";
  }
  s << "stroke:";
  if (oInfo.style == Qt::NoPen)
    s << "none ; ";
  else
    s << "rgb(" << oInfo.color.red () << "," << oInfo.color.green ()
      << "," << oInfo.color.blue () << ") ; ";
  s << "stroke-width:" << oInfo.width << ends;
  xw.addAttribute ("style", s.str ());
}

void SVGExport::addTextStyleAttribute (XmlWriter& xw, GText* obj) {
  ostrstream s;
  GObject::OutlineInfo oInfo = obj->getOutlineInfo ();
  GText::TextInfo tInfo = obj->getTextInfo ();

  s << "font-family:" << tInfo.font.family ();
  s << "; font-size:" << tInfo.font.pointSize ();
  s << "; fill:";
  if (oInfo.style == Qt::NoPen)
    s << "none" ;
  else {
    s << "rgb(" << oInfo.color.red () << "," << oInfo.color.green ()
      << "," << oInfo.color.blue () << ")";
  }
  s << ends;
  xw.addAttribute ("style", s.str ());
}
