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

#include <qpainter.h>
#include "Gradient.h"

enum { 
    Horizontal, Vertical, Radial, Rectangular, Diagonal1, Diagonal2 
  } Style;

Gradient::Gradient (const QColor& c1, const QColor& c2, Style s) 
  : color1 (c1), color2 (c2), style (s) {
}

void Gradient::setColor1 (const QColor& c) {
  color1 = c;
}

void Gradient::setColor2 (const QColor& c) {
  color2 = c;
}

void Gradient::setStyle (Gradient::Style s) {
  style = s;
}

const QColor& Gradient::getColor1 () const {
  return color1;
}

const QColor& Gradient::getColor2 () const {
  return color2;
}

Gradient::Style Gradient::getStyle () const {
  return style;
}

QPixmap Gradient::createPixmap (unsigned int width, unsigned int height) {
  QPixmap pix (width, height);
  QPainter p;
  p.begin (&pix);
  switch (style) {
  case Horizontal:
  case Vertical:
    createLinearGradient (p, width, height);
    break;
  default:
    break;
  }
  p.end ();
  return pix;
}

void Gradient::createLinearGradient (QPainter& p, unsigned int width, 
					 unsigned int height) {
  QColor col;
  QPen pen;
  double delta, dd;
  int r, g, b;

  if (style == Horizontal) {
    delta = 1.0 / (double) width;
    dd = width;
  }
  else {
    delta = 1.0 / (double) height;
    dd = height;
  }
  int rdiff = color1.red () - color2.red ();
  int gdiff = color1.green () - color2.green ();
  int bdiff = color1.blue () - color2.blue ();

  for (double d = 0.0; d < 1.0; d += delta) {
    r = color1.red () - qRound (rdiff * d);
    g = color1.green () - qRound (gdiff * d);
    b = color1.blue () - qRound (bdiff * d);
    col.setRgb (r, g, b);
    pen.setColor (col);
    p.setPen (pen);
    int x = qRound (dd * d);
    if (style == Horizontal)
      p.drawLine (x, 0, x, height);
    else
      p.drawLine (0, x, width, x);
  }
}
