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

#include <qobject.h>
#include <qcolor.h>
#include <qfont.h>
#include <qwmatrix.h>
#include <qpainter.h>
#include <qdstream.h>
#include <qdict.h>
#include <qlist.h>

#include "Coord.h"
#include "GObject.h"
#include "GPolyline.h"

class GBezier : public GPolyline {
  Q_OBJECT
public:
  GBezier ();
  GBezier (const list<XmlAttribute>& attribs);
  GBezier (const GBezier& obj);
  ~GBezier () {}

  virtual void draw (Painter& p, bool withBasePoints = false,
		     bool outline = false);
  virtual bool contains (const Coord& p);

  virtual void setPoint (int idx, const Coord& p);
  virtual void movePoint (int idx, float dx, float dy);
  virtual void removePoint (int idx, bool update = true);
  virtual void insertPoint (int idx, const Coord& p, bool update = true);
  
  virtual const char* typeName ();

  virtual GObject* copy ();
  virtual GObject* clone (const list<XmlAttribute>& attribs);

  virtual void writeToXml (XmlWriter&);

  virtual bool findNearestPoint (const Coord& p, float max_dist, 
				 float& dist, int& pidx);
  virtual int containingSegment (float xpos, float ypos);

  bool isEndPoint (int idx) { return (idx % 3) == 1; }
  void initBasePoint (int idx);
  void setWorkingSegment (int seg);
  void setClosed (bool flag);

  virtual void getPath (vector<Coord>& path);

  virtual bool intersects (const Rect& r);
  virtual const Rect& redrawBox () const { return rbox; }

protected:
  void calcBoundingBox ();

  int cPoint (int idx);
  void drawHelpLines (Painter& p);
  void drawHelpLinesForWorkingSegment (Painter& p);
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

