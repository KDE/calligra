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

#include <PolygonPreview.h>

#include <qpainter.h>
#include <qpointarray.h>

#include <GObject.h>
#include <math.h>

PolygonPreview::PolygonPreview (QWidget* parent, const char* name) :
  QFrame(parent, name) {
  nCorners = 3;
  sharpness = 0;
  isConcave = false;
  setBackgroundMode(PaletteBase);
  setFocusPolicy(QWidget::NoFocus);
  setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

void PolygonPreview::paintEvent (QPaintEvent *) {

    QPainter p;
    double angle = 2 * M_PI / nCorners;
    double diameter=static_cast<double>(QMAX(width(), height()) - 10);
    double radius=diameter * 0.5;

    p.begin(this);
    p.setWindow (qRound (-radius), qRound (-radius), qRound (diameter), qRound (diameter));
    p.setViewport (5, 5, width() - 10, height() - 10);
    p.setPen(colorGroup().text());

    QPointArray points(isConcave ? nCorners * 2 : nCorners);
    points.setPoint(0, 0, qRound (-radius));

    if (isConcave) {
        angle = angle / 2.0;
        double a = angle;
        double r = radius - (sharpness / 100.0 * radius);
        for (int i = 1; i < nCorners * 2; ++i) {
            double xp, yp;
            if (i % 2) {
                xp =  r * sin(a);
                yp = -r * cos(a);
            }
            else {
                xp = radius * sin(a);
                yp = -radius * cos(a);
            }
            a += angle;
            points.setPoint (i, (int) xp, (int) yp);
        }
    }
    else {
        double a = angle;
        for (int i = 1; i < nCorners; i++) {
            double xp = radius * sin(a);
            double yp = -radius * cos(a);
            a += angle;
            points.setPoint (i, (int) xp, (int) yp);
        }
    }
    p.drawPolygon(points);
    p.end();
}

void PolygonPreview::slotSharpness (int value) {
    sharpness = value;
    repaint();
}

void PolygonPreview::slotConcavePolygon () {
    isConcave = true;
    repaint();
}

void PolygonPreview::slotConvexPolygon () {
    isConcave = false;
    repaint();
}

void PolygonPreview::increaseNumOfCorners () {
    nCorners++;
    repaint();
}

void PolygonPreview::decreaseNumOfCorners () {
    if (nCorners > 3) {
        nCorners--;
        repaint();
    }
}

void PolygonPreview::setNumOfCorners (int value) {
    nCorners = value;
    repaint();
}

#include <PolygonPreview.moc>
