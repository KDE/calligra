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

#include  <GPolyline.h>

class GPolygon : public GPolyline
{
  Q_OBJECT
public:
  enum Kind { PK_Square, PK_Rectangle, PK_Polygon };

  GPolygon (GDocument* parent, Kind pkind = PK_Polygon);
  GPolygon (GDocument* parent, const QDomElement &element, Kind pkind = PK_Polygon);
  GPolygon (const GPolygon& obj);
  GPolygon (GDocument* parent, QList<Coord>& coords);
  ~GPolygon () {}

  virtual void movePoint (int idx, float dx, float dy, bool ctrlPressed=false);

  virtual void draw (QPainter& p, bool withBasePoints = false,
                     bool outline = false, bool withEditMarks=true);
  virtual bool contains (const Coord& p);
  virtual int getNeighbourPoint (const Coord& p);

  bool isRectangle () const;
  bool isFilled () const;
  virtual bool isValid ();
  virtual void insertPoint (int idx, const Coord& p, bool update = true);

  // a shortcut for rectangles and squares
  void setEndPoint (const Coord& p);

  void setSymmetricPolygon (const Coord& sp, const Coord& ep, int nCorners,
                            bool concave, int sharpness);

  virtual void getPath (QValueList<Coord>& path);

  virtual QString typeName () const;

  virtual GObject* copy ();
  //virtual GObject* create (GDocument *doc, const QDomElement &element);

  virtual QDomElement writeToXml (QDomDocument &document);

  void setKind (GPolygon::Kind k);
  GPolygon::Kind getKind () { return kind; }

  virtual bool splitAt (unsigned int idx, GObject*& obj1, GObject*& obj2);

  virtual GCurve* convertToCurve () const;

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
