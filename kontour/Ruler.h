/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#ifndef __Ruler_h__
#define __Ruler_h__

#include <qframe.h>
#include "units.h"

class QPixmap;
class KontourDocument;

namespace Kontour {
class Ruler : public QFrame
{
  Q_OBJECT
public:
  enum Orientation { Horizontal, Vertical };

  Ruler(KontourDocument *aDoc, Orientation o, MeasurementUnit mu = UnitPoint, QWidget *parent = 0L, const char* name = 0L);
  ~Ruler();

  MeasurementUnit unit() const {return mUnit; }
  void unit(MeasurementUnit mu);

  void zoomFactor(int xpos, int ypos);

public slots:
  void updatePointer(int x, int y);
  void updateOffset(int o);

  void show();
  void hide();


signals:
  /*emit signal for drawing a ruler, note: the position is in sceen positions
    and might be out of the drawing area; in such a case the position should
    be ignored by the slot of drawRuler and -- if drawn -- remove the
    draw'ed-Ruler; note: the can be only one drawRuler at a time
    orientationHoriz = true <=> horizontal
   */
  void drawHelpline(int x, int y, bool orientationHoriz);
  void addHelpline(int x, int y, bool orientationHoriz);
  void rmbPressed();

protected:
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);

protected slots:
  void mousePressEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);

private:
  void drawRuler();
  void drawMarker();
  void drawNum(QPainter &p, int x, int y, int a, bool orient);
  void recalculateSize(QResizeEvent *e);

private:
  KontourDocument *mDoc;  

  MeasurementUnit mUnit;
  Orientation mOrientation;
  bool isMousePressed;
  double zoom;
  int zeroPoint;
  int currentPosition;

  QPixmap *buffer;
  QPixmap *bg;
  QPixmap *mNums;
};
};
using namespace Kontour;

#endif
