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

#ifndef GPolyline_h_
#define GPolyline_h_

#include <GObject.h>

class Arrow;
class GDocument;

#define NEAR_DISTANCE 4

class GPolyline : public GObject
{
   Q_OBJECT
   private:
      GPolyline ():GObject(0) {cout<<"GPolyline ctor"<<endl; exit(1);};
public:
  GPolyline (GDocument* doc);
  GPolyline (GDocument* doc, const QDomElement &element);
  GPolyline (const GPolyline& obj);
  ~GPolyline () {}

  virtual void draw (QPainter& p, bool withBasePoints = false,
                     bool outline = false);
  virtual bool contains (const Coord& p);

  virtual void setPoint (int idx, const Coord& p);
  virtual void addPoint (int idx, const Coord& p, bool update = true);
  virtual void insertPoint (int idx, const Coord& p, bool update = true);
  void _addPoint (int idx, const Coord& p);
  void movePoint (int idx, float dx, float dy, bool ctrlPressed=false);

  const Coord& getPoint (int idx);
  QList<Coord>& getPoints ();

  virtual int containingSegment (float xpos, float ypos);
  virtual bool isValid ();

  int getNeighbourPoint (const Coord& p);
  unsigned int numOfPoints () const;
  virtual void removePoint (int idx, bool update = true);
  void removeAllPoints ();

  virtual QString typeName () const;

  virtual GObject* copy ();
  //virtual GObject* create (GDocument *doc, const QDomElement &element);

  virtual QDomElement writeToXml (QDomDocument &document);

  virtual bool findNearestPoint (const Coord& p, float max_dist,
                                 float& dist, int& pidx, bool all = false);

  virtual void getPath (QValueList<Coord>& path);
  virtual GCurve* convertToCurve () const;

  virtual bool splitAt (unsigned int idx, GObject*& obj1, GObject*& obj2);

  void calcBoundingBox ();

protected:
  virtual void updateProperties (GObject::Property prop, int mask);
  float calcArrowAngle (const Coord& p1, const Coord& p2, int direction);

  Rect calcEnvelope ();

protected:
  QList<Coord> points;
  Arrow *sArrow, *eArrow;
  float sAngle, eAngle;
  float sdx, sdy, edx, edy;
};

#endif
