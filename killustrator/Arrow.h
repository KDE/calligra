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

#ifndef Arrow_h_
#define Arrow_h_

#include <qintdict.h>
#include <qpointarray.h>
#include <Coord.h>

class QPixmap;
class QPainter;

class Arrow {
public:
  Arrow (long aid, int npts, const QCOORD* pts, bool fillIt = true);
  ~Arrow ();

  long arrowID () const;
  QPixmap& leftPixmap ();
  QPixmap& rightPixmap ();
  void draw (QPainter& p, const Coord& c, const QColor& color,
             float width, float angle);

  Rect boundingBox (const Coord& c, float width, float angle);
  int length ();

  static void install (Arrow* arrow);
  static Arrow* getArrow (long id);
  static QIntDictIterator<Arrow> getArrows ();

private:
  static void initialize ();

  long id;
  QPixmap *lpreview, *rpreview;
  QPointArray points;
  bool fill;

  static QIntDict<Arrow> *arrows;
};

#endif
