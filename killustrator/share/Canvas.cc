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

#include <iostream.h>
#include <fstream.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qprintdialog.h>
#include <qcolor.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <kmsgbox.h>
#include "Canvas.h"
#include "Canvas.moc"
#include "GDocument.h"
#include "Handle.h"
#include "ToolController.h"
#include "QwViewport.h"
#include "version.h"
#include <kconfig.h>

QArray<float> Canvas::zoomFactors;

Canvas::Canvas (GDocument* doc, float res, QwViewport* vp, QWidget* parent,
		const char* name) : QWidget (parent, name) {
  document = doc;
  resolution = res;
  zoomFactor = 1.0;
  drawBasePoints = false;
  viewport = vp;

  connect (document, SIGNAL (changed ()), this, SLOT (updateView ()));
  connect (document, SIGNAL (changed (const Rect&)),
	   this, SLOT (updateRegion (const Rect&)));
  connect (document, SIGNAL (sizeChanged ()), this, SLOT (calculateSize ()));
  connect (&(document->handle ()), SIGNAL (handleChanged ()),
	   this, SLOT (updateView ()));
  connect (document, SIGNAL (gridChanged ()), this, SLOT (updateGridInfos ()));

  pixmap = 0L;

  helplinesAreOn = helplinesSnapIsOn = false;

  readGridProperties ();
  updateGridInfos ();

  calculateSize ();
  setFocusPolicy (ClickFocus);
  setMouseTracking (true);
  setBackgroundMode (NoBackground);

  dragging = false;
  ensureVisibilityFlag = false;
  outlineMode = false;
  pendingRedraws = 0;
}

Canvas::~Canvas () {
  if (pixmap != 0L)
    delete pixmap;
}

void Canvas::ensureVisibility (bool flag) {
  ensureVisibilityFlag = flag;
}

void Canvas::calculateSize () {
  width = (int) (document->getPaperWidth () * resolution *
		 zoomFactor / 72.0) + 4;
  height = (int) (document->getPaperHeight () * resolution *
		  zoomFactor / 72.0 + 4);
  resize (width, height);

  if (pixmap != 0L)
    delete pixmap;
  pixmap = 0L;
  if (zoomFactor < 3.0)
    pixmap = new QPixmap (width, height);
  updateView ();
  emit sizeChanged ();
}

void Canvas::initZoomFactors (QArray<float>& factors) {
  zoomFactors.duplicate (factors);
}


void Canvas::setZoomFactor (float factor) {
  zoomFactor = factor;
  calculateSize ();
  // recompute pixmaps of fill areas
  document->invalidateClipRegions ();
  updateView ();
  emit sizeChanged ();
  emit zoomFactorChanged (zoomFactor);
}

float Canvas::getZoomFactor () const {
  return zoomFactor;
}

void Canvas::showGrid (bool flag) {
  if (gridIsOn != flag) {
    gridIsOn = flag;
    updateView ();
    emit gridStatusChanged ();
    saveGridProperties ();
  }
}

void Canvas::snapToGrid (bool flag) {
  if (gridSnapIsOn != flag) {
    gridSnapIsOn = flag;
    saveGridProperties ();
    emit gridStatusChanged ();
    document->setGrid (hGridDistance, vGridDistance, gridSnapIsOn);
  }
}

void Canvas::setGridDistance (float hdist, float vdist) {
  hGridDistance = hdist;
  vGridDistance = vdist;
  saveGridProperties ();
  document->setGrid (hGridDistance, vGridDistance, gridSnapIsOn);
}

void Canvas::snapPositionToGrid (float& x, float& y) {
  bool snap = false;

  if (helplinesSnapIsOn) {
    // try to snap to help lines
    vector<float>::iterator i;
    for (i = horizHelplines.begin (); i != horizHelplines.end (); i++) {
      if (fabs (*i - y) <= 10.0) {
	y = *i;
	snap = true;
	break;
      }
    }
    for (i = vertHelplines.begin (); i != vertHelplines.end (); i++) {
      if (fabs (*i - x) <= 10.0) {
	x = *i;
	snap = true;
	break;
      }
    }
  }
  if (gridSnapIsOn && ! snap) {
    int n = (int) (x / hGridDistance);
    float r = fmod (x, hGridDistance);
    if (r > (hGridDistance / 2.0))
      n++;
    x = hGridDistance * n;

    n = (int) (y / vGridDistance);
    r = fmod (y, vGridDistance);
    if (r > (vGridDistance / 2.0))
      n++;
    y = vGridDistance * n;
  }
}

void Canvas::setToolController (ToolController* tc) {
  toolController = tc;
}

void Canvas::propagateMouseEvent (QMouseEvent *e) {
  // transform position of the mouse pointer according to current
  // zoom factor
  QPoint new_pos (qRound (e->x () * 72 / (resolution * zoomFactor)) - 1,
		  qRound (e->y () * 72 / (resolution * zoomFactor)) - 1);
  QMouseEvent new_ev (e->type (), new_pos, e->button (), e->state ());

  emit mousePositionChanged (new_pos.x (), new_pos.y ());

  // ensure visibility
  if (ensureVisibilityFlag) {
    if (e->type () == QEvent::MouseButtonPress && e->button () == LeftButton)
      dragging = true;
    else if (e->type () == QEvent::MouseButtonRelease &&
	     e->button () == LeftButton)
      dragging = false;
    else if (e->type () == QEvent::MouseMove && dragging)
      viewport->ensureVisible (e->x (), e->y (), 10, 10);
  }

  if (e->button () == RightButton &&
      e->type () == QEvent::MouseButtonPress &&
      ! toolController->getActiveTool ()->consumesRMBEvents ()) {
    if (document->selectionIsEmpty ()) {
      GObject* obj = document->findContainingObject (new_pos.x (),
						     new_pos.y ());
      if (obj) {
	// pop up menu for the picked object
	emit rightButtonAtObjectClicked (e->x (), e->y (), obj);
      }
      else {
	emit rightButtonClicked (e->x (), e->y ());
      }
    }
    else {
      // pop up menu for the current selection
      emit rightButtonAtSelectionClicked (e->x (), e->y ());
    }
    return;
  }
  else
    if (toolController) {
      // the tool controller processes the event
      toolController->delegateEvent (&new_ev, document, this);
    }
}

void Canvas::propagateKeyEvent (QKeyEvent *e) {
  if (toolController) {
    toolController->delegateEvent (e, document, this);
  }
}

void Canvas::mousePressEvent (QMouseEvent* e) {
  propagateMouseEvent (e);
}

void Canvas::mouseReleaseEvent (QMouseEvent* e) {
  propagateMouseEvent (e);
}

void Canvas::mouseMoveEvent (QMouseEvent* e) {
  propagateMouseEvent (e);
}

void Canvas::keyPressEvent (QKeyEvent* e) {
  propagateKeyEvent (e);
}

void Canvas::paintEvent (QPaintEvent* e) {
  const QRect& rect = e->rect ();
  if (pixmap != 0L)
    bitBlt (this, rect.x (), rect.y (), pixmap,
	    rect.x (), rect.y (), rect.width (), rect.height ());
  else
    // For large zoom levels there is now pixmap to copy. So we
    // have to redraw the whole document, but without to call
    // repaint !!!
    redrawView (false);
}

void Canvas::moveEvent (QMoveEvent *e) {
    emit visibleAreaChanged (e->pos ().x (), e->pos ().y ());
}

void Canvas::setDocument (GDocument* doc) {
  document = doc;
  updateGridInfos ();
  connect (document, SIGNAL (changed ()), this, SLOT (updateView ()));
  connect (document, SIGNAL (gridChanged ()), this, SLOT (updateGridInfos ()));
}

GDocument* Canvas::getDocument () {
  return document;
}

void Canvas::showBasePoints (bool flag) {
  drawBasePoints = flag;
  updateView ();
}

void Canvas::setOutlineMode (bool flag) {
  if (outlineMode != flag) {
    outlineMode = flag;
    updateView ();
  }
}

float Canvas::scaleFactor () const {
  return resolution * zoomFactor / 72.0;
}

void Canvas::updateView () {
  redrawView (true);
}

void Canvas::redrawView (bool repaintFlag) {
  QPaintDevice *pdev;
  pendingRedraws = 0;

  Painter p;
  float s = scaleFactor ();

  // setup the painter
  pdev = ((QPaintDevice*)pixmap ? (QPaintDevice*)pixmap : this);
  p.begin (pdev);
  p.setBackgroundColor (white);
  if (pixmap)
    pixmap->fill (backgroundColor ());

  p.scale (s, s);

  p.setPen (black);
  //  p.drawRect (0, 0, width - 2, height - 2);
  p.setPen (QPen (darkGray, 2));
  p.moveTo (width - 1, 2);
  p.lineTo (width - 1, height - 1);
  p.lineTo (2, height - 1);
  p.setPen (black);

  // clear the canvas
  //  p.translate (1, 1);
  p.eraseRect (0, 0, document->getPaperWidth (),
	       document->getPaperHeight ());

  // draw the grid
  if (gridIsOn)
    drawGrid (p);

  // draw the help lines
  if (helplinesAreOn)
    drawHelplines (p);

  // next the document contents
  document->drawContents (p, drawBasePoints, outlineMode);

  // and finally the handle
  if (! document->selectionIsEmpty ())
    document->handle ().draw (p);

  p.end ();
  // Don't repaint if called form paintEvent () !!
  if (repaintFlag)
    repaint ();
}

void Canvas::retryUpdateRegion () {
  updateRegion (region);
}

void Canvas::updateRegion (const Rect& reg) {
  if (pendingRedraws == 0 && document->selectionCount () > 1) {
    // we have to update a multiple selection, so we collect
    // the update regions and redraw it in one call
    pendingRedraws = document->selectionCount () - 1;
    regionForUpdate = reg;
    return;
  }

  Rect r = reg;

  if (pendingRedraws > 0) {
    regionForUpdate = regionForUpdate.unite (r);
    pendingRedraws--;
    if (pendingRedraws > 0)
      // not the last redraw call
      return;
    else
      r = regionForUpdate;
  }

  Painter p;
  float s = scaleFactor ();

  // compute the clipping region
  QWMatrix m;
  m.scale (s, s);

  QRect clip = m.map (QRect (int (r.left ()), int (r.top ()),
			     int (r.width ()), int (r.height ())));

  QPaintDevice *pdev = ((QPaintDevice*)pixmap ? (QPaintDevice*)pixmap : this);
  if (pdev->paintingActive ()) {
    // this occurs only in KOffice, when a embedded part tries
    // to draw in our canvas
    region = reg;
    QTimer::singleShot (50, this, SLOT(retryUpdateRegion ()));
    return;
  }

  // setup the painter
  p.begin (pdev);
  p.setBackgroundColor (white);
  // setup the clip region
  if (clip.x () <= 1) clip.setX (1);
  if (clip.y () <= 1) clip.setY (1);

  int mw = (int) ((float) document->getPaperWidth () * s);
  int mh = (int) ((float) document->getPaperHeight () * s);

  if (clip.right () >= mw)
    clip.setRight (mw);
  if (clip.bottom () >= mh)
    clip.setBottom (mh);

  //  cout << "clip: " << clip.left () << ", " << clip.top ()
  //       << ", " << clip.width () << ", " << clip.height () << endl;

  p.setClipRect (clip);

  // clear the canvas
  p.scale (s, s);
  //  p.translate (1, 1);
  p.eraseRect (r.left (), r.top (), r.width (), r.height ());

  // draw the grid
  if (gridIsOn)
    drawGrid (p);

  // draw the help lines
  if (helplinesAreOn)
    drawHelplines (p);

  // next the document contents
  document->drawContentsInRegion (p, r, drawBasePoints, outlineMode);

  // and finally the handle
  if (! document->selectionIsEmpty ())
    document->handle ().draw (p);

  p.end ();
  repaint (clip, false);
}

void Canvas::drawHelplines (Painter& p) {
  int pw = document->getPaperWidth ();
  int ph = document->getPaperHeight ();
  unsigned int i;

  QPen pen (blue, 0, DashLine);

  p.save ();
  p.setPen (pen);
  for (i = 0; i < horizHelplines.size (); i++) {
    int hi = qRound (horizHelplines[i]);
    p.drawLine (0, hi, pw, hi);
  }
  for (i = 0; i < vertHelplines.size (); i++) {
    int vi = qRound (vertHelplines[i]);
    p.drawLine (vi, 0, vi, ph);
  }

  p.restore ();
}

void Canvas::drawGrid (Painter& p) {
  int pw = document->getPaperWidth ();
  int ph = document->getPaperHeight ();
  float h, v;

  QPen pen1 (blue, 0, DotLine);

  p.save ();
  p.setPen (pen1);
  for (h = hGridDistance; h < pw; h += hGridDistance) {
    int hi = qRound (h);
    p.drawLine (hi, 0, hi, ph);
  }
  for (v = vGridDistance; v < ph; v += vGridDistance) {
    int vi = qRound (v);
    p.drawLine (0, vi, pw, vi);
  }
  p.restore ();
}

void Canvas::printDocument () {
  QPrinter printer;
  printer.setDocName (document->fileName ());
  printer.setCreator ("KIllustrator");
  switch (document->pageLayout ().format) {
  case PG_DIN_A4:
    printer.setPageSize (QPrinter::A4);
    break;
  case PG_DIN_A5:
    printer.setPageSize (QPrinter::B5);
    break;
  case PG_US_LETTER:
    printer.setPageSize (QPrinter::Letter);
    break;
  case PG_US_LEGAL:
    printer.setPageSize (QPrinter::Legal);
    break;
  default:
    break;
  }
  printer.setOrientation (document->pageLayout ().orientation == PG_PORTRAIT ?
			  QPrinter::Portrait : QPrinter::Landscape);
  if (printer.setup (this)) {
    Painter paint;
    paint.begin (&printer);
    paint.setClipping (false);
    document->drawContents (paint);
    paint.end ();
  }
}

void Canvas::zoomIn (int x, int y) {
  int pos = zoomFactors.find (getZoomFactor ());
  assert (pos != -1);
  if (pos < (int) zoomFactors.size () - 1) {
    setZoomFactor (zoomFactors[pos + 1]);
    viewport->centerOn (x, y);
    emit zoomFactorChanged (zoomFactors[pos + 1]);
  }
}

void Canvas::zoomOut () {
  int pos = zoomFactors.find (getZoomFactor ());
  assert (pos != -1);
  if (pos > 0)
    setZoomFactor (zoomFactors[pos - 1]);
}

void Canvas::readGridProperties () {
  KConfig* config = kapp->getConfig ();
  QString oldgroup = config->group ();

  config->setGroup ("Grid");

  vGridDistance = (float) config->readDoubleNumEntry ("vGridDistance", 50.0);
  hGridDistance = (float) config->readDoubleNumEntry ("hGridDistance", 50.0);
  gridIsOn = config->readBoolEntry ("showGrid", false);
  gridSnapIsOn = config->readBoolEntry ("snapTopGrid", false);

  config->setGroup ("Helplines");
  helplinesAreOn = config->readBoolEntry ("showHelplines");
  helplinesSnapIsOn = config->readBoolEntry ("snapTopHelplines");

  config->setGroup (oldgroup);
}

void Canvas::saveGridProperties () {
  KConfig* config = kapp->getConfig ();
  QString oldgroup = config->group ();

  config->setGroup ("Grid");

  config->writeEntry ("vGridDistance", (double) vGridDistance);
  config->writeEntry ("hGridDistance", (double) hGridDistance);
  config->writeEntry ("showGrid", gridIsOn);
  config->writeEntry ("snapTopGrid", gridSnapIsOn);

  config->setGroup ("Helplines");
  config->writeEntry ("showHelplines", helplinesAreOn);
  config->writeEntry ("snapTopHelplines", helplinesSnapIsOn);

  config->setGroup (oldgroup);
  config->sync ();
}

void Canvas::setHorizHelplines (const vector<float>& lines) {
  horizHelplines = lines;
  if (helplinesAreOn)
    updateView ();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}

void Canvas::setVertHelplines (const vector<float>& lines) {
  vertHelplines = lines;
  if (helplinesAreOn)
    updateView ();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}

const vector<float>& Canvas::getHorizHelplines () const {
  return horizHelplines;
}

const vector<float>& Canvas::getVertHelplines () const {
  return vertHelplines;
}

void Canvas::alignToHelplines (bool flag) {
  helplinesSnapIsOn = flag;
  emit gridStatusChanged ();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}

bool Canvas::alignToHelplines () {
  return helplinesSnapIsOn;
}

void Canvas::showHelplines (bool flag) {
  if (helplinesAreOn != flag) {
    helplinesAreOn = flag;
    updateView ();
    emit gridStatusChanged ();
    saveGridProperties ();
  }
}

bool Canvas::showHelplines () {
  return helplinesAreOn;
}

void Canvas::updateGridInfos () {
  document->getGrid (hGridDistance, vGridDistance, gridSnapIsOn);
  document->getHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
  saveGridProperties ();
  emit gridStatusChanged ();
}
