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

#ifndef GBezier_h_
#define GBezier_h_

#include <Coord.h>
#include <GPolyline.h>

class GBezier : public GPolyline
{
  Q_OBJECT
public:
  GBezier (GDocument* parent);
  GBezier (GDocument* parent, const QDomElement &element);
  GBezier (const GBezier& obj);
  ~GBezier () {}

  virtual void draw (QPainter& p, bool withBasePoints = false,
                     bool outline = false, bool withEditMarks=true);
  virtual bool contains (const Coord& p);

  virtual void setPoint (int idx, const Coord& p);
  virtual void movePoint (int idx, float dx, float dy, bool ctrlPressed=false);
  virtual void removePoint (int idx, bool update = true);
  virtual void insertPoint (int idx, const Coord& p, bool update = true);

  virtual QString typeName () const;

  virtual GObject* copy ();
  //virtual GObject* create (GDocument *doc, const QDomElement &element);

  virtual QDomElement writeToXml (QDomDocument &document);

  virtual bool findNearestPoint (const Coord& p, float max_dist,
                                 float& dist, int& pidx, bool all = false);
  virtual int containingSegment (float xpos, float ypos);

  bool isEndPoint (int idx) { return (idx % 3) == 1; }
  void initBasePoint (int idx);
  void setWorkingSegment (int seg);
  void setClosed (bool flag);
  bool isClosed () const { return closed; }

  virtual void getPath (QValueList<Coord>& path);

  virtual bool intersects (const Rect& r);
  virtual const Rect& redrawBox () const { return rbox; }

  virtual bool splitAt (unsigned int idx, GObject*& obj1, GObject*& obj2);

  virtual GCurve* convertToCurve () const;

  static bool bezier_segment_contains (const Coord& p0, const Coord& p1,
                                       const Coord& p2, const Coord& p3,
                                       const Coord& c);
protected:
  void calcBoundingBox ();

  int cPoint (int idx);
  void drawHelpLines (QPainter& p);
  void drawHelpLinesForWorkingSegment (QPainter& p);
  void updateBasePoint (int idx);

  void computePPoints ();
  int createPolyline (int index, int pidx);
  void updateGradientShape (QPainter& p);

private:
  Rect rbox;                // the bounding box for redrawing
  int wSegment;
  QPointArray ppoints; // Points for the computed polygon
  bool closed;
};

#endif
