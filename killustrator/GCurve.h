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

#include <GObject.h>

class GSegment {
public:
  enum Kind { sk_Line, sk_Bezier };

  GSegment(Kind skind=sk_Line);
  GSegment(const QDomElement &element);

  const Coord& pointAt (int i) const;
  void setPoint (int i, const Coord& c);

  QDomElement writeToXml(QDomDocument &document);
  void draw (QPainter& p, bool withBasePoints, bool outline, bool drawFirst);
  void movePoint (int idx, float dx, float dy, bool ctrlPressed=false);

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
   private:
      GCurve():GObject(0) {cout<<"GCurve ctor"<<endl; exit(1);};
public:
  GCurve (GDocument* parent);
  GCurve (GDocument* parent, const QDomElement &element);
  GCurve (const GCurve& obj);
  ~GCurve () {}

  virtual void draw (QPainter& p, bool withBasePoints = false,
                     bool outline = false, bool withEditMarks=true);
  virtual bool contains (const Coord& p);
  virtual QString typeName () const;

  virtual GObject* copy ();
  //virtual GObject* create (GDocument *doc, const QDomElement &element);

  virtual QDomElement writeToXml (QDomDocument &document);

  virtual void movePoint (int idx, float dx, float dy, bool ctrlPressed=false);
  virtual void removePoint (int idx, bool update = true);
  virtual int getNeighbourPoint (const Coord& p);

  virtual void getPath(QValueList<Coord>& path);

  virtual GCurve* convertToCurve () const { return new GCurve (*this); }

  void addLineSegment (const Coord& p1, const Coord& p2);
  void addBezierSegment (const Coord& p1, const Coord& p2,
                         const Coord& p3, const Coord& p4);
  void addSegment (const GSegment& s);
  const GSegment& getSegment (int idx);
  int numOfSegments () const { return segments.count(); }

  void setClosed (bool flag);
  bool isClosed () const { return closed; }

  static GCurve* blendCurves (GCurve *start, GCurve *end, int step, int num);

protected:
  static QColor blendColors (const QColor& c1, const QColor& c2,
                             int step, int num);

  void calcBoundingBox ();
  void updateGradientShape (QPainter& p);
  void updatePath ();
  QValueList<GSegment>::Iterator containingSegment (const Coord& p);

private:
  QPointArray points; // Points for the computed polygon
  QValueList<GSegment> segments;
  bool closed;
};

#endif
