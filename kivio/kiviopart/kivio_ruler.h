/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef KIVIO_RULER_H
#define KIVIO_RULER_H

#include "tkunits.h"

#include <qframe.h>
#include <qpainter.h>
#include <qpixmap.h>

class KivioRuler : public QFrame
{ Q_OBJECT
public:
  enum Orientation { Horizontal, Vertical };

  KivioRuler(Orientation, QWidget* parent=0, const char* name=0);
  ~KivioRuler();

  int unit();

public slots:
  void setZoom(float);
  void updatePointer(int x, int y);
  void updateVisibleArea(int xpos, int ypos);

  void setUnit(int);

  void hide();
  void show();

protected:
  void paintEvent(QPaintEvent*);
  void resizeEvent(QResizeEvent*);

  void recalculateSize(QResizeEvent*);
  void drawKivioRuler();

  void initMarker(int w, int h);
  void drawNums(QPainter*, int x, int y, QString& num, bool orientationHoriz);

private:
  float m_pZoom;
  int munit;
  Orientation orientation;
  QPixmap *buffer;
  int firstVisible;
  int currentPosition;
  QPixmap* marker;
  QPixmap* m_pNums;
};

#endif
