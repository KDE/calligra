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

#ifndef GPolygon_h_
#define GPolygon_h_

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

class GPolygon : public GPolyline {
  Q_OBJECT
public:
  enum Kind { PK_Square, PK_Rectangle, PK_Polygon };

  GPolygon (Kind pkind = PK_Polygon);
  GPolygon (const list<XmlAttribute>& attribs, Kind pkind = PK_Polygon);
  GPolygon (const GPolygon& obj);
  GPolygon (QList<Coord>& coords);
  ~GPolygon () {}

  virtual void movePoint (int idx, float dx, float dy);

  virtual void draw (Painter& p, bool withBasePoints = false,
		     bool outline = false);
  virtual bool contains (const Coord& p);
  virtual int getNeighbourPoint (const Coord& p);

  bool isRectangle () const;
  bool isFilled () const;

  // a shortcut for rectangles and squares
  void setEndPoint (const Coord& p);

  void setSymmetricPolygon (const Coord& sp, const Coord& ep, int nCorners, 
			    bool concave, int sharpness);
  
  virtual void getPath (vector<Coord>& path);

  virtual const char* typeName ();

  virtual GObject* copy ();

  virtual void writeToXml (XmlWriter&);

protected:
  void calcBoundingBox ();
  void updateGradientShape (QPainter& p);

  bool inside_polygon (const Coord& p);
  void update_rpoints ();

private:
  Kind kind;
  QList<Coord> rpoints;
};

#endif

