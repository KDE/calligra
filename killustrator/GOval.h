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

#include <GObject.h>

class GOval : public GObject
{
  Q_OBJECT
   private:
      GOval():GObject(0) {cout<<"GOval ctor"<<endl; exit(1);};
public:
  GOval (GDocument* parent, bool cFlag = false);
  GOval (GDocument* parent, const QDomElement &element, bool cFlag = false);
  GOval (const GOval& obj);
  ~GOval () {}

  virtual void draw (QPainter& p, bool withBasePoints = false,
                     bool outline = false, bool withEditMarks=true);
  virtual bool contains (const Coord& p);

  void setStartPoint (const Coord& p);
  void setEndPoint (const Coord& p);

  void setAngles (float a1, float a2);

  const Coord& startPoint () const { return sPoint; }
  const Coord& endPoint () const { return ePoint; }
  bool isCircle () const { return circleFlag; }

  virtual bool isValid ();

  virtual GObject* copy ();
  //virtual GObject* create (GDocument *doc, const QDomElement &element);

  virtual QDomElement writeToXml(QDomDocument &document);

  virtual int getNeighbourPoint (const Coord& p);
  virtual void movePoint (int idx, float dx, float dy, bool ctrlPressed=false);

  virtual QString typeName () const;

  virtual void getPath (QValueList<Coord>& path);
  virtual GCurve* convertToCurve () const;

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
