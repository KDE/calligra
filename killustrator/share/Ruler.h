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

#ifndef Ruler_h_
#define Ruler_h_

#include <qwidget.h>
#include <qpixmap.h>

#include "PStateManager.h"

class Ruler : public QWidget {
  Q_OBJECT
public:
  //  enum MeasurementUnit { Point, Centimeter, Inch };
  enum Orientation { Horizontal, Vertical };

  Ruler (Orientation o, MeasurementUnit mu = UnitPoint, QWidget *parent = 0L,
	 const char* name = 0L);

  MeasurementUnit measurementUnit () const;
  void setMeasurementUnit (MeasurementUnit mu);

public slots:
  void setZoomFactor (float zf);
  void updatePointer (int x, int y);
  void updateVisibleArea (int xpos, int ypos);

  void hide ();
  void show ();

protected:
  void paintEvent (QPaintEvent *e);
  void resizeEvent (QResizeEvent *e);

  void recalculateSize (QResizeEvent *e);
  void drawRuler ();

  void initMarker (int w, int h);

private:
  float zoom;
  MeasurementUnit munit;
  Orientation orientation;
  QPixmap *buffer;
  int firstVisible;
  int currentPosition;
  QPixmap *marker, *bg;
};

#endif
