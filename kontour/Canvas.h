/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2000-2001 Igor Janssen (rm@linux.ru.net)

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

#ifndef __Canvas_h__
#define __Canvas_h__

#include <qwidget.h>

#include <koRect.h>
#include "GDocument.h"

class KontourView;
class KoPainter;
class QScrollBar;

namespace Kontour {

class GObject;
class Canvas : public QWidget
{
  Q_OBJECT
public:
  Canvas(GDocument *aGDoc, KontourView *aView, QScrollBar *hb, QScrollBar *vb, QWidget *parent = 0, const char *name = 0);
  ~Canvas();

  GDocument *document() const {return mGDoc; }

  double zoomFactor() const {return mGDoc->zoomFactor(); }
  
  int xOffset() const {return mXOffset; }
  int yOffset() const {return mYOffset; }

  KoPoint point(int x, int y);
  QRect onCanvas(const KoRect &r);

  void outlineMode(bool flag);
  void withBasePoints(bool flag);

  void updateBuf();
  void updateBuf(const QRect &rect);

  void setXimPosition(int x, int y, int w, int h);

  /**
   *  Put the given point in the center of the canvas
   *  x and y are relative to the center of the paper.
   */
  void center(int x = 0, int y = 0);

  void zoomToPoint(double scale, int x, int y);

  void snapPositionToGrid(double &x, double &y);
  KoRect snapTranslatedBoxToGrid(const KoRect &r);
  KoRect snapScaledBoxToGrid(const KoRect &r, int hmask);

public slots:
  void addHelpline(int x, int y, bool horizH);
  void drawTmpHelpline(int x, int y, bool horizH);

protected:
  bool eventFilter(QObject *o, QEvent *e);
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);
  void wheelEvent(QWheelEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);

signals:
  void offsetXChanged(int v);
  void offsetYChanged(int v);
  void mousePositionChanged(int x, int y);
  void coordsChanged(double x, double y);

  void rmbAtSelection(int x, int y);
  void backSpaceCalled();

private slots:
  void changePage();
  void changeZoomFactor(double scale);
  void updateScrollBars();
  void scrollX(int v);
  void scrollY(int v);
  void updateRegion(const KoRect &r, bool handle = false);

private:
  void propagateMouseEvent(QMouseEvent *e);
  void drawGrid(const QRect &rect);
  void drawHelplines(const QRect &rect);

  double snapXPositionToGrid(double pos);
  double snapYPositionToGrid(double pos);

private:
  GDocument *mGDoc;
  KontourView *mView;

  QScrollBar *hBar;
  QScrollBar *vBar;

  KoPainter *painter;

  int mXOffset;
  int mYOffset;

  int mXCenter;
  int mYCenter;

  int mWidthH;
  int mHeightH;

  bool mOutlineMode:1;
  bool mWithBasePoints:1;

  int mTmpHorizHelpline;
  int mTmpVertHelpline;
};
};
using namespace Kontour;

#endif
