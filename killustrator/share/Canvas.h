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

#ifndef Canvas_h_
#define Canvas_h_

#include <iostream.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qfont.h>
#include <qarray.h>

#include "Painter.h"

class GDocument;
class GObject;
class ToolController;
class QwViewport;

class Canvas : public QWidget {
  Q_OBJECT
public:
  Canvas (GDocument* doc, float res, QwViewport* vp, QWidget* parent = 0, 
	  const char* name = 0);
  ~Canvas ();

  void setZoomFactor (float factor);
  float getZoomFactor () const;

  void showGrid (bool flag);
  bool showGrid () const { return gridIsOn; }

  void snapToGrid (bool flag);
  bool snapToGrid () const { return gridSnapIsOn; }

  void setGridDistance (int hdist, int vdist);
  float getHorizGridDistance () const { return hGridDistance; }
  float getVertGridDistance () const { return vGridDistance; }

  void snapPositionToGrid (int& x, int& y);

  void setToolController (ToolController *tc);
  
  void setDocument (GDocument* doc);
  GDocument* getDocument ();
  
  void printDocument ();

  void printPSDocument ();
  void writePSHeader (ostream& os);

  static bool writePSProlog (ostream& os);
  static const char* getPSFont (const QFont& qfont);

  void zoomIn (int x, int y);
  void zoomOut ();

  void showBasePoints (bool flag = true);

  float scaleFactor () const;

  static void initZoomFactors (QArray<float>& factors);

protected:
  void mousePressEvent (QMouseEvent *e);
  void mouseReleaseEvent (QMouseEvent *e);
  void mouseMoveEvent (QMouseEvent *e);
  void keyPressEvent (QKeyEvent *e);
  void paintEvent (QPaintEvent *e);
  void moveEvent (QMoveEvent *e);

signals:
  void sizeChanged ();

  void rightButtonAtObjectClicked (int x, int y, GObject* obj);
  void rightButtonAtSelectionClicked (int x, int y);
  void rightButtonClicked (int x, int y);

  void visibleAreaChanged (int x, int y);
  void zoomFactorChanged (float zoom);

  void mousePositionChanged (int x, int y);

public slots:
  void updateView ();
  void ensureVisibility (bool flag);
  void calculateSize ();

private:
  void propagateMouseEvent (QMouseEvent *e);
  void propagateKeyEvent (QKeyEvent *e);
  void drawGrid (Painter& p);

  QwViewport* viewport;
  QPixmap* pixmap;
  int width, height;
  float resolution;
  float zoomFactor;
  GDocument* document;
  ToolController* toolController;
  bool gridIsOn;
  bool gridSnapIsOn;
  bool dragging, ensureVisibilityFlag;
  int hGridDistance, vGridDistance;
  bool drawBasePoints;
  static QArray<float> zoomFactors;
  static QString psPrologPath;
};

#endif
