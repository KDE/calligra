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
#include <Coord.h>
#include <qscrollview.h>

class GDocument;
class GObject;
class ToolController;
class QPixmap;
class QPainter;
class Rect;
class QColor;
class QSize;
class QScrollBar;

class Canvas : public QWidget {
  Q_OBJECT
public:
  Canvas (GDocument *doc, float res, QScrollBar *hb, QScrollBar *vb, QWidget *parent = 0, const char *name = 0);
  ~Canvas ();

  void setDocument (GDocument* doc);
  GDocument* getDocument () const { return document; };
  
  void setZoomFactor (float factor);
  float getZoomFactor () const { return zoomFactor; };

  float scaleFactor () const { return resolution * zoomFactor / 72.0; };
  
  int xOffset () const {return mXOffset; };
  int yOffset () const {return mYOffset; };
  
  void centerPage();
  
/*
  Helplines
*/ 
  void setHorizHelplines(const QValueList<float>& lines);
  void setVertHelplines(const QValueList<float>& lines);
  const QValueList<float>& getHorizHelplines () const;
  const QValueList<float>& getVertHelplines () const;
  void alignToHelplines (bool flag);
  bool alignToHelplines ();
  void showHelplines (bool flag);
  bool showHelplines ();
  int indexOfHorizHelpline (float pos);
  int indexOfVertHelpline (float pos);
  void updateHorizHelpline (int idx, float pos);
  void updateVertHelpline (int idx, float pos);
  void updateHelplines (); 

/*
    GRID
*/
  void showGrid (bool flag);
  bool showGrid () const { return gridIsOn; }

  void snapToGrid (bool flag);
  bool snapToGrid () const { return gridSnapIsOn; }
  
  void setGridColor(QColor color);
  QColor gridColor() const { return mGridColor; };

  void setGridDistance (float hdist, float vdist);
  float getHorizGridDistance () const { return hGridDistance; }
  float getVertGridDistance () const { return vGridDistance; }
  void saveGridProperties ();

  void snapPositionToGrid (float& x, float& y);
  Rect snapTranslatedBoxToGrid (const Rect& r);
  Rect snapScaledBoxToGrid (const Rect& r, int hmask);

  void setToolController (ToolController *tc);

  /* Printing */
  void setupPrinter( QPrinter &printer );
  void print( QPrinter &printer );

  void showBasePoints (bool flag = true);
  void setOutlineMode (bool flag);

protected:
   //return the size of the canvas with current zoom and resolution in pixels
  QSize actualSize();
  QSize actualPaperSizePt() const;
  void updateScrollBars();
  
  float snapXPositionToGrid (float pos);
  float snapYPositionToGrid (float pos);

  void readGridProperties ();

  bool eventFilter (QObject *, QEvent *);
  /* Events */
  void mousePressEvent (QMouseEvent *e);
  void mouseReleaseEvent (QMouseEvent *e);
  void mouseMoveEvent (QMouseEvent *e);
  void keyPressEvent (QKeyEvent *e);
  void paintEvent (QPaintEvent *e);
  void moveEvent (QMoveEvent *e);
  void resizeEvent (QResizeEvent *e);
  
  void addHorizHelpline (float pos);
  void addVertHelpline (float pos);

signals:
  void sizeChanged ();
  //void gridStatusChanged ();
  void rightButtonAtObjectClicked (int x, int y, GObject *obj);
  void rightButtonAtSelectionClicked (int x, int y);
  void rightButtonClicked (int x, int y);

  void visibleAreaChanged (int x, int y);
  void zoomFactorChanged (float zoom);

  void mousePositionChanged (int x, int y);

public slots:
  void updateRegion (const Rect& r);
  void ensureVisibility (bool flag);
  void calculateSize ();
  void updateGridInfos ();
  
  
  void addHelpline (int x, int y, bool horizH);
  void drawTmpHelpline (int x, int y, bool horizH);

private slots:
  void retryUpdateRegion ();
  void scrollX(int v);
  void scrollY(int v);

private:
  void propagateMouseEvent (QMouseEvent *e);
  void drawGrid (QPainter& p);
  void drawHelplines (QPainter& p);

  QPixmap *buffer;
  QScrollBar *hBar;
  QScrollBar *vBar;

  int m_width, m_height;
  int mXOffset, mYOffset;
  int xPaper, yPaper;
  float resolution;
  float zoomFactor;
  GDocument *document;
  ToolController *toolController;
  QColor mGridColor;
  float hGridDistance, vGridDistance;
  int pendingRedraws;
  Rect regionForUpdate, region;

  QValueList<float> horizHelplines, vertHelplines;
  float tmpHorizHelpline, tmpVertHelpline;

  bool gridSnapIsOn;
  bool helplinesSnapIsOn;
  bool helplinesAreOn:1;
  bool gridIsOn:1;
  bool dragging:1;
  bool ensureVisibilityFlag:1;
  bool drawBasePoints:1;
  bool outlineMode:1;
};

#endif
