/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2002 Igor Janssen (rm@kde.org)

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

#ifndef __GPolygon_h__
#define __GPolygon_h__

#include "GObject.h"

class GPolygon : public GObject
{
  Q_OBJECT
public:
  GPolygon(bool roundness = false);
  GPolygon(const QDomElement &element);
  GPolygon(const GPolygon &obj);

  GObject *copy() const;

  bool isSquare() const;

  const KoPoint &startPoint() const {return sPoint; }
  void startPoint(const KoPoint &p);

  const KoPoint &endPoint() const {return ePoint; }
  void endPoint(const KoPoint &p);

  QString typeName () const;
  QDomElement writeToXml(QDomDocument &document);
  void draw(KoPainter *p, int aXOffset, int aYOffset, bool withBasePoints = false, bool outline = false, bool withEditMarks = true);

  int getNeighbourPoint(const KoPoint &point);
  void movePoint(int idx, double dx, double dy, bool ctrlPressed = false);
  void removePoint(int idx, bool update = true);
  bool contains(const KoPoint &p);
  bool findNearestPoint(const KoPoint &p, double max_dist, double &dist, int &pidx, bool all);

  void calcBoundingBox();
  GPath *convertToPath() const;
  bool isConvertible() const;

private:
  KoPoint     sPoint;           // Start point
  KoPoint     ePoint;           // End point
  double      mXRoundness;      // Rectangle roundness
  double      mYRoundness;      // Rectangle roundness
};

#endif
