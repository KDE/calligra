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

#ifndef GOval_h_
#define GOval_h_

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

class GOval : public GObject {
  Q_OBJECT
public:
  GOval (bool cFlag = false);
  GOval (const list<XmlAttribute>& attribs, bool cFlag = false);
  GOval (const GOval& obj);
  ~GOval () {}
  
  virtual void draw (Painter& p, bool withBasePoints = false);
  virtual bool contains (const Coord& p);
  
  void setStartPoint (const Coord& p);
  void setEndPoint (const Coord& p);

  void setAngles (float a1, float a2);

  const Coord& startPoint () const { return sPoint; }
  const Coord& endPoint () const { return ePoint; }
  bool isCircle () const { return circleFlag; }

  virtual GObject* copy ();

  virtual void writeToXml (XmlWriter&);

  virtual int getNeighbourPoint (const Coord& p);
  virtual void movePoint (int idx, float dx, float dy);

  virtual const char* typeName ();

protected:
  void updateGradientShape (QPainter& p);
  void calcBoundingBox ();

  void update_segments ();

private:
  Coord ePoint, sPoint, segPoint[2];
  float sAngle, eAngle;
  bool circleFlag;
};

#endif

