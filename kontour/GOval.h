/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Jansen (rm@kde.org)

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

#ifndef __GOval_h__
#define __GOval_h__

#include "GObject.h"

namespace Kontour
{
class GOval : public GObject
{
  Q_OBJECT
public:
  enum Type{ Arc, Sector, Segment };

  GOval(double rx, double ry, Type aType = Arc, double sa = 0.0, double a = 360.0);
  GOval(const QDomElement &element);
  GOval(const GOval &obj);

  GObject *copy() const;

  Type type() const {return mType; }
  void type(Type t);

  bool isCircle() const;

  QString typeName () const;
  QDomElement writeToXml(QDomDocument &document);
  void draw(KoPainter *p, const QWMatrix &m, bool withBasePoints = false, bool outline = false, bool withEditMarks = true);

  int getNeighbourPoint(const KoPoint &point, const double distance);
  void movePoint(int idx, double dx, double dy, bool ctrlPressed = false);
  void removePoint(int idx);
  bool contains(const KoPoint &p);

  void calcBoundingBox();
  GPath *convertToPath() const;
  bool isConvertible() const;

private:
  double      mRX;                // X radius
  double      mRY;                // Y radius
  Type        mType;              // Oval type
  double      mStartAngle;        // Start angle
  double      mAngle;             // Arc angle
  KoPoint     segPoint[2];        //
};
};
using namespace Kontour;

#endif
