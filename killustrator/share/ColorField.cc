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

#include <qpushbutton.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qdrawutil.h>

#include "ColorField.h"
#include "ColorField.moc"

ColorField::ColorField (int idx, const QBrush& b, 
			QWidget* parent, const char* name) 
  : QButton (parent, name) {
  setFixedSize (15, 15);
  resize (15, 15);
  brush = b;
  highlighted = false;
  id = idx;
}

void ColorField::setBrush (const QBrush& b) {
  brush = b;
  repaint ();
}

void ColorField::highlight (bool flag) {
  if (highlighted != flag) {
    highlighted = flag;
    repaint ();
  }
}

void ColorField::drawButton (QPainter* p) {
  QRect r = rect ();
  QBrush fill (brush.color ());
  qDrawShadeRect (p, r, colorGroup (), TRUE, 1, 1, &fill);
  if (brush.style () == NoBrush) {
    p->setPen (darkGray);
    p->drawLine (1, 1, 13, 13);
    p->drawLine (13, 1, 1, 13);
  }
  if (highlighted)
    p->drawWinFocusRect (1, 1, 13, 13);
}

void ColorField::mouseReleaseEvent (QMouseEvent* e) {
  if (e->state () == RightButton)
    emit colorSelected (0, id, brush);
  else if (e->state () == LeftButton)
    emit colorSelected (1, id, brush);
}
