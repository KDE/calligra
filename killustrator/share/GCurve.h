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

#ifndef GCurve_h_
#define GCurve_h_

#include <qobject.h>
#include <qcolor.h>
#include <qfont.h>
#include <qwmatrix.h>
#include <qpainter.h>
#include <list.h>

#include "Coord.h"
#include "GObject.h"

class GSegment {
public:
  enum Kind { sk_Line, sk_Bezier };

  GSegment (Kind skind);

  const Coord& pointAt (int i) const;
  void setPoint (int i, const Coord& c);

  void writeToXml (XmlWriter& xml);
  void draw (QPainter& p, bool withBasePoints, bool outline, bool drawFirst);
  void movePoint (int idx, float dx, float dy);

  Rect boundingBox ();
  bool contains (const Coord& p);

  Kind kind () const { return skind; }
  QPointArray getPoints () const;

  float length () const;

private:
  Kind skind;
  Coord points[4];
  QPointArray bpoints;
};
  
class GCurve : public GObject {
  Q_OBJECT
public:
  GCurve ();
  GCurve (const list<XmlAttribute>& attribs);
  GCurve (const GCurve& obj);
  ~GCurve () {}

  virtual void draw (QPainter& p, bool withBasePoints = false,
		     bool outline = false);
  virtual bool contains (const Coord& p);
  virtual QString typeName () const;

  virtual GObject* copy ();
  virtual GObject* clone (const list<XmlAttribute>& attribs);

  virtual void writeToXml (XmlWriter&);

  virtual void movePoint (int /*idx*/, float /*dx*/, float /*dy*/);
  virtual void removePoint (int idx, bool update = true);
  virtual int getNeighbourPoint (const Coord& p);

  virtual void getPath (vector<Coord>& path);

  virtual GCurve* convertToCurve () const { return new GCurve (*this); }

  void addLineSegment (const Coord& p1, const Coord& p2);
  void addBezierSegment (const Coord& p1, const Coord& p2,
			 const Coord& p3, const Coord& p4);
  void addSegment (const GSegment& s);

  int numOfSegments () const { return segments.size (); }

  void setClosed (bool flag);
  bool isClosed () const { return closed; }

  static GCurve* blendCurves (GCurve *start, GCurve *end, int step, int num);

protected:
  static QColor blendColors (const QColor& c1, const QColor& c2,
			     int step, int num);

  void calcBoundingBox ();
  void updateGradientShape (QPainter& p);
  void updatePath ();
  list<GSegment>::iterator containingSegment (const Coord& p);

private:
  QPointArray points; // Points for the computed polygon
  list<GSegment> segments;
  bool closed;
};

#endif

