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

#include <RotateCmd.h>
#include <klocale.h>
#include <kdebug.h>

RotateCmd::RotateCmd (GDocument* doc, const Coord& center, float a) :
  ObjectManipCmd (doc, i18n("Rotate")) {
  rcenter = center;
  angle = a;
  kdDebug() << "---------- angle: " << angle << endl;
}

void RotateCmd::execute () {
  QWMatrix m1, m2, m3;
  m1.translate (-rcenter.x (), -rcenter.y ());
  m2.rotate (angle);
  m3.translate (rcenter.x (), rcenter.y ());

  ObjectManipCmd::execute ();

  kdDebug() << "M1 - dx: " << m1.dx() << " dy: " << m1.dy() << " m11: " << m1.m11()
            << " m12: " << m1.m12() << " m21: " << m1.m21() << " m22: " << m1.m22() << endl;
  kdDebug() << "M2 - dx: " << m2.dx() << " dy: " << m2.dy() << " m11: " << m2.m11()
            << " m12: " << m2.m12() << " m21: " << m2.m21() << " m22: " << m2.m22() << endl;
  kdDebug() << "M3 - dx: " << m3.dx() << " dy: " << m3.dy() << " m11: " << m3.m11()
            << " m12: " << m3.m12() << " m21: " << m3.m21() << " m22: " << m3.m22() << endl;
  for (unsigned int i = 0; i < objects.count (); i++) {
    objects[i]->transform (m1);
    objects[i]->transform (m2);
    objects[i]->transform (m3, true);
  }
}

#include <RotateCmd.moc>
