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

#include "GPart.h"
#include "GPart.moc"

#include <klocale.h>
#include <kapp.h>

#include "KIllustrator_view.h"
#include "KIllustrator_doc.h"

GPart::GPart (KIllustratorChild *c) {
  child = c;
  view = 0L;
  QRect r = child->geometry ();
  cout << "part size = " << r.width () << ", " << r.height () << endl;
  tMatrix.translate (r.x () - tMatrix.dx (), 
		     r.y () - tMatrix.dy ());
  iMatrix = tMatrix.invert ();
  initTmpMatrix ();
  calcBoundingBox ();
}

GPart::GPart (const list<XmlAttribute>& attribs) : GObject (attribs) {
  list<XmlAttribute>::const_iterator first = attribs.begin ();
	
  while (first != attribs.end ()) {
    const string& attr = (*first).name ();
    first++;
  }
  calcBoundingBox ();
}

GPart::GPart (const GPart& obj) : GObject (obj) {
  calcBoundingBox ();
}

GPart::~GPart () {
}

const char* GPart::typeName () {
  return i18n ("Embedded Part");
}

void GPart::draw (Painter& p, bool withBasePoints, bool outline) {
  p.save ();
  QRect r = child->geometry ();
  p.setWorldMatrix (tmpMatrix, true);
  if (outline) {
    p.setPen (black);
    p.drawRect (0, 0, r.width (), r.height ());
  }
  else {
    QRect win = p.window ();
    QRect vPort = p.viewport ();
    QPicture *pic = child->draw ();
    QWMatrix mx = p.worldMatrix ();
    QRect mr = mx.map (QRect (0, 0, r.width (), r.height ()));
    p.setViewport (0, 0, vPort.width (), vPort.height ());
    //    p.setWorldMatrix (QWMatrix ());
    p.drawPicture (*pic);
    p.setViewport (vPort);
    p.setWindow (win);
  }
  p.restore ();
}

void GPart::calcBoundingBox () {
  QRect r = child->geometry ();
  cout << "part : " << 0 << ", " << 0 << " -- " 
       << r.width () << ", " << r.height () << endl;
  calcUntransformedBoundingBox (Coord (0, 0), 
				Coord (r.width (), 0),
				Coord (r.width (), r.height ()), 
				Coord (0, r.height ()));
}

GObject* GPart::copy () {
  return new GPart (*this);
}

void GPart::writeToXml (XmlWriter& xml) {
}
