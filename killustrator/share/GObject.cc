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
#include "GObject.h"
#include "GObject.moc"

#include "GPolyline.h"
#include "GPolygon.h"
#include "GOval.h"
#include "GText.h"
#include "GGroup.h"
#include "GClipart.h"
#include "GBezier.h"

QDict<GObject> GObject::prototypes;
GObject::OutlineInfo GObject::defaultOutlineInfo;
GObject::FillInfo GObject::defaultFillInfo;

void GObject::setDefaultOutlineInfo (const OutlineInfo& oi) {
  if (oi.mask & OutlineInfo::Color)
    defaultOutlineInfo.color = oi.color;
  if (oi.mask & OutlineInfo::Style)
    defaultOutlineInfo.style = oi.style;
  if (oi.mask & OutlineInfo::Width)
    defaultOutlineInfo.width = oi.width;
}

void GObject::setDefaultFillInfo (const FillInfo& fi) {
  if (fi.mask & FillInfo::Color) 
    defaultFillInfo.color = fi.color;
  if (fi.mask & FillInfo::Style)
    defaultFillInfo.style = fi.style;
}

GObject::OutlineInfo GObject::getDefaultOutlineInfo () {
  return defaultOutlineInfo;
}

GObject::FillInfo GObject::getDefaultFillInfo () {
  return defaultFillInfo;
}

GObject::GObject () {
  sflag = false;

  outlineInfo = defaultOutlineInfo;
  outlineInfo.mask = OutlineInfo::Color | OutlineInfo::Style | 
    OutlineInfo::Width | OutlineInfo::Custom;
  outlineInfo.roundness = 0;
  outlineInfo.shape = OutlineInfo::DefaultShape;
  outlineInfo.startArrowId = outlineInfo.endArrowId = 0;

  //  outlineInfo.ckind = OutlineInfo::Custom_None;
  fillInfo = defaultFillInfo;
  fillInfo.mask = FillInfo::Color | FillInfo::Style;

  rcount = 1;
}

GObject::GObject (const list<XmlAttribute>& attribs) {
  list<XmlAttribute>::const_iterator first = attribs.begin ();
	
  while (first != attribs.end ()) {
    const string& attr = (*first).name ();
    if (attr == "matrix") {
      tMatrix = (*first).matrixValue ();
      tmpMatrix = tMatrix;
    }
    else if (attr == "strokecolor")
      outlineInfo.color = (*first).colorValue ();
    else if (attr == "strokestyle")
      outlineInfo.style = (PenStyle) (*first).intValue ();
    else if (attr == "linewidth")
      outlineInfo.width = (*first).floatValue ();
    else if (attr == "fillcolor")
      fillInfo.color = (*first).colorValue ();
    else if (attr == "fillstyle")
      fillInfo.style = (BrushStyle) (*first).intValue ();
    first++;
  }
}

GObject::GObject (const GObject& obj) {
  sflag = false;
  outlineInfo = obj.outlineInfo;
  fillInfo = obj.fillInfo;
  tMatrix = obj.tMatrix;
  tmpMatrix = tMatrix;

  rcount = 1;
}

GObject::~GObject () {
}

void GObject::ref () {
  rcount++;
}

void GObject::unref () {
  if (--rcount == 0)
    delete this;
}

void GObject::transform (const QWMatrix& m, bool update) {
  tMatrix = tMatrix * m;
  initTmpMatrix ();
  if (update) {
    calcBoundingBox ();
    emit changed ();
  }
}

void GObject::initTmpMatrix () {
  tmpMatrix = tMatrix;
}

void GObject::ttransform (const QWMatrix& m, bool update) {
  tmpMatrix = tmpMatrix * m;
  if (update) {
    calcBoundingBox ();
    emit changed ();
  }
}

void GObject::setOutlineInfo (const GObject::OutlineInfo& info) {
  if (info.mask & OutlineInfo::Color)
    outlineInfo.color = info.color;
  if (info.mask & OutlineInfo::Style)
    outlineInfo.style = info.style;
  if (info.mask & OutlineInfo::Width)
    outlineInfo.width = info.width;
  if (info.mask & OutlineInfo::Custom) {
    outlineInfo.roundness = info.roundness;
    outlineInfo.shape = info.shape;
    outlineInfo.startArrowId = info.startArrowId;
    outlineInfo.endArrowId = info.endArrowId;
#if 0
    outlineInfo.ckind = info.ckind;
    switch (info.ckind) {
    case OutlineInfo::Custom_Line:
      outlineInfo.custom.arrow.startId = info.custom.arrow.startId;
      outlineInfo.custom.arrow.endId = info.custom.arrow.endId;
      break;
    case OutlineInfo::Custom_Rectangle:
      outlineInfo.custom.roundness = info.custom.roundness;
      break;
    case OutlineInfo::Custom_Ellipse:
      outlineInfo.custom.shape = info.custom.shape;
      break;
    default:
      break;
    }
#endif
  }
  emit changed ();
  emit propertiesChanged ();
}

GObject::OutlineInfo GObject::getOutlineInfo () const {
  return outlineInfo;
}
  
void GObject::setOutlineShape (OutlineInfo::Shape s) {
    outlineInfo.shape = s;
  emit changed ();
  emit propertiesChanged ();
}

void GObject::setOutlineColor (const QColor& color) {
  outlineInfo.color = color;
  emit changed ();
  emit propertiesChanged ();
}

void GObject::setOutlineStyle (PenStyle style) {
  outlineInfo.style = style;
  emit changed ();
  emit propertiesChanged ();
}

void GObject::setOutlineWidth (float width) {
  outlineInfo.width = width;
  emit changed ();
  emit propertiesChanged ();
}

const QColor& GObject::getOutlineColor () const {
  return outlineInfo.color;
}

PenStyle GObject::getOutlineStyle () const {
  return outlineInfo.style;
}

float GObject::getOutlineWidth () const {
  return outlineInfo.width;
}

void GObject::setFillInfo (const GObject::FillInfo& info) {
  if (info.mask & FillInfo::Color)
    fillInfo.color = info.color;
  if (info.mask & FillInfo::Style)
    fillInfo.style = info.style;
  emit changed ();
  emit propertiesChanged ();
}

GObject::FillInfo GObject::getFillInfo () const {
  return fillInfo;
}
  
void GObject::setFillColor (const QColor& color) {
  fillInfo.color = color;
  emit changed ();
  emit propertiesChanged ();
}

const QColor& GObject::getFillColor () const {
  return fillInfo.color;
}

void GObject::setFillStyle (BrushStyle b) {
  fillInfo.style = b;
  emit changed ();
  emit propertiesChanged ();
}

BrushStyle GObject::getFillStyle () const {
  return fillInfo.style;
}

void GObject::select (bool flag) {
  sflag = flag;
}

bool GObject::contains (const Coord& p) {
  return box.contains (p);
}

void GObject::draw (Painter&, bool) {
}

void GObject::writeToPS (ostream& os) {
  // line width
  os << outlineInfo.width << " setlinewidth\n";
  // pen style
  os << outlineInfo.style << " SPSt\n";
  // outline color
  os << outlineInfo.color.red () / 255.0 << ' ' 
     << outlineInfo.color.green () / 255.0
     << ' ' << outlineInfo.color.blue () / 255.0 << " DOCol\n";
  // fill color
  os << fillInfo.color.red () / 255.0 << ' ' 
     << fillInfo.color.green () / 255.0
     << ' ' << fillInfo.color.blue () / 255.0 << " DFCol\n";
  // transformation matrix
  os << '[' << tMatrix.m11 () << ' ' << tMatrix.m12 () << ' ' 
     << tMatrix.m21 () << ' ' << tMatrix.m22 () << ' '
     << tMatrix.dx () << ' ' << tMatrix.dy () << "] SMatrix\n";
}

void GObject::updateBoundingBox (const Rect& r) {
  box = r.normalize ();
}

void GObject::updateBoundingBox (const Coord& p1, const Coord& p2) {
  Rect r (p1, p2);
  updateBoundingBox (r);
}

GOState* GObject::saveState () {
  GOState* state = new GOState;
  GObject::initState (state);
  return state;
}

void GObject::initState (GOState* state) {
  state->matrix = tMatrix;
  state->fInfo = fillInfo;
  state->oInfo = outlineInfo;
}

void GObject::restoreState (GOState* state) {
  tMatrix = state->matrix;
  tmpMatrix = tMatrix;
  setFillInfo (state->fInfo);
  setOutlineInfo (state->oInfo);
  calcBoundingBox ();
  emit changed ();
}

void GObject::calcUntransformedBoundingBox (const Coord& tleft, 
					    const Coord& tright,
					    const Coord& bright, 
					    const Coord& bleft) {
  Coord p[4];
  Rect r;

  p[0] = tleft.transform (tmpMatrix);
  p[1] = tright.transform (tmpMatrix);
  p[2] = bleft.transform (tmpMatrix);
  p[3] = bright.transform (tmpMatrix);

  r.left (p[0].x ());
  r.top (p[0].y ());
  r.right (p[0].x ());
  r.bottom (p[0].y ());

  for (unsigned int i = 1; i < 4; i++) {
    r.left (QMIN(p[i].x (), r.left ()));
    r.top (QMIN(p[i].y (), r.top ()));
    r.right (QMAX(p[i].x (), r.right ()));
    r.bottom (QMAX(p[i].y (), r.bottom ()));
  }
  updateBoundingBox (r);
}

void GObject::writePropertiesToXml (XmlWriter& xml) {
  xml.addAttribute ("matrix", tMatrix);
  xml.addAttribute ("strokecolor", outlineInfo.color);
  xml.addAttribute ("strokestyle", (int) outlineInfo.style);
  xml.addAttribute ("linewidth", outlineInfo.width);
  xml.addAttribute ("fillcolor", fillInfo.color);
  xml.addAttribute ("fillstyle", (int) fillInfo.style);
}

