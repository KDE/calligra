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

#include "PolygonPreview.h"
#include "PolygonPreview.moc"
#include "GObject.h"
#include <qpainter.h>
#include <qpointarray.h>
#include <math.h>

#define WIDTH 160
#define HEIGHT 160

PolygonPreview::PolygonPreview (QWidget* parent, const char* name) :
  QWidget (parent, name) {
  nCorners = 3;
  sharpness = 0;
  isConcave = false;
  resize (WIDTH, HEIGHT);
  setFixedSize (WIDTH, HEIGHT);
}

void PolygonPreview::paintEvent (QPaintEvent *) {
  QPainter p;
  double a, angle = 2 * M_PI / nCorners;
  double xp, yp;

#define RADIUS 100
  p.begin (this);
  p.setBackgroundColor (white);
  p.eraseRect (0, 0, WIDTH, HEIGHT);

  p.setWindow (-RADIUS, -RADIUS, 2 * RADIUS, 2 * RADIUS);
  p.setViewport (5, 5, WIDTH - 10, HEIGHT - 10);

  
  p.setPen (black);

  QPointArray points (isConcave ? nCorners * 2 : nCorners);
  points.setPoint (0, 0, -RADIUS);

  if (isConcave) {
    angle = angle / 2.0;
    a = angle;
    double r = RADIUS - (sharpness / 100.0 * RADIUS);
    for (int i = 1; i < nCorners * 2; i++) {
      if (i % 2) {
	xp =  r * sin (a);
	yp = -r * cos (a);
      }
      else {
	xp = RADIUS * sin (a);
	yp = - RADIUS * cos (a);
      }
      a += angle;
      points.setPoint (i, (int) xp, (int) yp);
    }
  }
  else {
    a = angle;
    for (int i = 1; i < nCorners; i++) {
      xp = RADIUS * sin (a);
      yp = - RADIUS * cos (a);
      a += angle;
      points.setPoint (i, (int) xp, (int) yp);
    }
  }
  p.drawPolygon (points);
  p.end ();
}

QSize PolygonPreview::sizeHint () const {
  return QSize (WIDTH, HEIGHT);
}

void PolygonPreview::slotSharpness (int value) {
  sharpness = value;
  repaint ();
}

void PolygonPreview::slotConcavePolygon () {
  isConcave = true;
  repaint ();
}

void PolygonPreview::slotConvexPolygon () {
  isConcave = false;
  repaint ();
}

void PolygonPreview::increaseNumOfCorners () {
  nCorners++;
  repaint ();
}

void PolygonPreview::decreaseNumOfCorners () {
  if (nCorners > 3) {
    nCorners--;
    repaint ();
  }
}

void PolygonPreview::setNumOfCorners (int value) {
  nCorners = value;
  repaint ();
};

