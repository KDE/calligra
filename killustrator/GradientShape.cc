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

#include <GradientShape.h>
#include <qpainter.h>
#include <iostream.h>

GradientShape::GradientShape () : isValid (false) {
}

void GradientShape::setRegion (const QRegion& r) {
  region = r;
  setInvalid ();
}

void GradientShape::setBox (const Rect& r) {
  box = r.normalize ();
  setInvalid ();
}

void GradientShape::setGradient (const Gradient& g) {
  gradient = g;
  setInvalid ();
}

void GradientShape::draw (QPainter& p)
{
   cout<<"GShape::draw: box: x: "<<box.x()<<" y: "<<box.y()<<" w: "<<box.width()<<" h: "<<box.height()<<endl;
   cout<<"region consists of "<<region.rects().count()<<" rects"<<endl;
   if (region.rects().count()>0)
      cout<<"GShape::draw: region: x: "<<region.rects()[0].x()<<" y: "<<region.rects()[0].y()<<" w: "<<region.rects()[0].width()<<" h: "<<region.rects()[0].height()<<endl;
   p.save ();
   p.setClipRegion (region);
   p.setClipping (true);
   p.drawPixmap (qRound (box.left ()), qRound (box.top ()), pixmap);
   p.restore ();
}

void GradientShape::updatePixmap () {
  if (!region.isEmpty () && !box.empty ()) {
    isValid = true;
    pixmap = gradient.createPixmap (qRound (box.width ()), qRound (box.height ()));
  }
}
