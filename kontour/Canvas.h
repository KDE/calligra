/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
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

#ifndef Canvas_h_
#define Canvas_h_

#include <qwidget.h>
#include <qvaluelist.h>
#include <qscrollview.h>
#include <koprinter.h>

#include "Coord.h"

class GDocument;
class GObject;
class ToolController;
class QPixmap;
class QPainter;
class Rect;
class QColor;
class QSize;
class QScrollBar;
class KPrinter;

class Canvas : public QWidget {
  Q_OBJECT
public:
  Canvas (GDocument *doc, float res, QScrollBar *hb, QScrollBar *vb, QWidget *parent = 0, const char *name = 0);
  ~Canvas ();

  void setDocument (GDocument* doc);
  GDocument* getDocument () const { return document; };

  void setZoomFactor (float factor, int centerX, int centerY);
  void setZoomFactor (float factor);
  float getZoomFactor () const { return zoomFactor; };

  float scaleFactor () const { return resolution * zoomFactor / 72.0; };

  //int xOffset () const {return mXOffset; };
  //int yOffset () const {return mYOffset; };

  //put the given point in the center of the canvas
  //x and y are relative to the center of the paper
  void center(int x=0, int y=0);

  //return the rectangle covered by the paper in points on the screen
  //upper left corner is always (0,0)
  //it changes only when zooming or chaning the paper size
  QRect paperArea()             {return m_paperArea;};
  //return the currently visible rectangle
  //coordinates are relative to the upper left corner of the paper
  //it changes when scrolling, zooming, resizing, alex
  QRect visibleArea()           {return m_visibleArea;};

  //return the coordinates of the paper relative to current canvas size
  //((-10,-10),(-10,-10)) means that the paper is on every edge
  //10 points bigger than the current canvas
  //it changes when scrolling, zooming, resizing and changing the paper size, alex
  QRect relativePaperArea()     {return m_relativePaperArea;};

  void snapPositionToGrid (float& x, float& y);
  Rect snapTranslatedBoxToGrid (const Rect& r);
  Rect snapScaledBoxToGrid (const Rect& r, int hmask);

  void setToolController (ToolController *tc);

  /* Printing */
  void setupPrinter( KPrinter &printer );
  void print( KPrinter &printer );

  void showBasePoints (bool flag = true);
  void setOutlineMode (bool flag);

  void setXimPosition(int x, int y, int w, int h);

protected:
  QRect m_paperArea;
  QRect m_visibleArea;
  QRect m_relativePaperArea;

  //if you call more than one of them, thy should be called in the order
  //of the prepended numbers, since they depend on each other
  //(this doesn't mean you have to call all of them), alex
  //none of the adjust*() functions emits any signal to avoid multiple signals
  void adjustPaperArea1();
  void adjustScrollBarRanges2();
  void adjustScrollBarPositions3(int x, int y);
  void adjustVisibleArea4();
  void adjustRelativePaperArea5();

  float snapXPositionToGrid (float pos);
  float snapYPositionToGrid (float pos);

  bool eventFilter (QObject *, QEvent *);
  /* Events */
  void mousePressEvent (QMouseEvent *e);
  void mouseReleaseEvent (QMouseEvent *e);
  void mouseMoveEvent (QMouseEvent *e);
  void keyPressEvent (QKeyEvent *e);
  void paintEvent (QPaintEvent *e);
  void moveEvent (QMoveEvent *e);
  void resizeEvent (QResizeEvent *e);

signals:
  void rightButtonAtObjectClicked (int x, int y, GObject *obj);
  void rightButtonAtSelectionClicked (int x, int y);
  void rightButtonClicked (int x, int y);

  void visibleAreaChanged (const QRect& area);
  //void visibleAreaChanged (int x, int y);
  void zoomFactorChanged (float zoom);

  void mousePositionChanged (int x, int y);

    void backSpaceCalled();
public slots:
  void updateRegion (const Rect& r);
  void ensureVisibility (bool flag);
  void docSizeChanged();

  void addHelpline (int x, int y, bool horizH);
  void drawTmpHelpline (int x, int y, bool horizH);

private slots:
  void retryUpdateRegion ();
  void scroll();

private:
  GDocument *document;


  void propagateMouseEvent (QMouseEvent *e);
  void drawGrid (QPainter& p);
  void drawHelplines (QPainter& p);

  QPixmap *buffer;
  QScrollBar *hBar;
  QScrollBar *vBar;

  int m_width, m_height;
  int xPaper, yPaper;
  float resolution;
  float zoomFactor;
  ToolController *toolController;
  int pendingRedraws;
  Rect regionForUpdate, region;

  float tmpHorizHelpline, tmpVertHelpline;


  bool dragging:1;
  bool ensureVisibilityFlag:1;
  bool drawBasePoints:1;
  bool outlineMode:1;
  bool guiActive:1;
};

#endif
