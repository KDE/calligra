/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <Canvas.h>

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

#include <GDocument.h>
#include <Handle.h>
#include <ToolController.h>
#include <qscrollview.h>
#include <kconfig.h>
#include <kapp.h>

#include <GPolyline.h> // for NEAR_DISTANCE
#include <GLayer.h>
#include <SelectionTool.h>

Canvas::Canvas (GDocument* doc, float res, QScrollView* sv, QWidget* parent,
                const char* name) : QWidget (parent, name) {
  
  document = doc;
  resolution = res;
  zoomFactor = 1.0;
  drawBasePoints = false;
  scrollview = sv;

  installEventFilter(this);

  connect (document, SIGNAL (changed ()), this, SLOT (updateView ()));
  connect (document, SIGNAL (changed (const Rect&)),
           this, SLOT (updateRegion (const Rect&)));
  connect (document, SIGNAL (sizeChanged ()), this, SLOT (calculateSize ()));
  connect (&(document->handle ()), SIGNAL (handleChanged ()),
           this, SLOT (updateView ()));
  connect (document, SIGNAL (gridChanged ()), this, SLOT (updateGridInfos ()));

  pixmap = 0L;

  helplinesAreOn = helplinesSnapIsOn = false;
  tmpHorizHelpline = tmpVertHelpline = -1;
  mGridColor = blue;

  readGridProperties ();
  updateGridInfos ();

  calculateSize ();
  setFocusPolicy (StrongFocus);
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
  m_width = (int) (document->getPaperWidth () * resolution *
                 zoomFactor / 72.0) + 4;
  m_height = (int) (document->getPaperHeight () * resolution *
                  zoomFactor / 72.0 + 4);
  resize (m_width, m_height);

  if (pixmap != 0L)
    delete pixmap;
  pixmap = 0L;
  if (zoomFactor < 3.0)
    pixmap = new QPixmap (m_width, m_height);
  //viewport->recalculateChildPosition (this);
  updateView ();
  emit sizeChanged ();
}

void Canvas::setZoomFactor (float factor) {
  zoomFactor = factor;
  calculateSize ();
  // recompute pixmaps of fill areas
  document->invalidateClipRegions ();
  int x = scrollview->viewport()->width()-width();
  int y = scrollview->viewport()->height()-height();
  if(x < 0)
   x = 0;
  if(y < 0)
   y = 0;
  move(x/2,y/2);
  updateView ();
  emit sizeChanged ();
  emit zoomFactorChanged (zoomFactor, x/2 ,y/2);
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

void Canvas::setGridColor(QColor color)
 {
  mGridColor = color;
  saveGridProperties ();
 }

void Canvas::setGridDistance (float hdist, float vdist) {
  hGridDistance = hdist;
  vGridDistance = vdist;
  saveGridProperties ();
  document->setGrid (hGridDistance, vGridDistance, gridSnapIsOn);
}

Rect Canvas::snapTranslatedBoxToGrid (const Rect& r) {
  float x1, x2, y1, y2;

  if (helplinesSnapIsOn || gridSnapIsOn) {
    x1 = snapXPositionToGrid (r.left ());
    x2 = snapXPositionToGrid (r.right ());
    y1 = snapYPositionToGrid (r.top ());
    y2 = snapYPositionToGrid (r.bottom ());

    float x = 0, y = 0;
    if (fabs (r.left () - x1) < fabs (r.right () - x2))
      x = x1;
    else
      x = x2 - r.width ();

    if (fabs (r.top () - y1) < fabs (r.bottom () - y2))
      y = y1;
    else
      y = y2 - r.height ();
    return Rect (x, y, r.width (), r.height ());
  }
  else
    return r;
}

Rect Canvas::snapScaledBoxToGrid (const Rect& r, int hmask) {
  float x1, x2, y1, y2;

  if (helplinesSnapIsOn || gridSnapIsOn) {
    x1 = snapXPositionToGrid (r.left ());
    x2 = snapXPositionToGrid (r.right ());
    y1 = snapYPositionToGrid (r.top ());
    y2 = snapYPositionToGrid (r.bottom ());

    Rect retval (r);
    if (hmask & Handle::HPos_Left)
      retval.left (x1);
    if (hmask & Handle::HPos_Top)
      retval.top (y1);
    if (hmask & Handle::HPos_Right)
      retval.right (x2);
    if (hmask & Handle::HPos_Bottom)
      retval.bottom (y2);
    return retval;
  }
  else
    return r;
}

float Canvas::snapXPositionToGrid (float pos) {
  bool snap = false;

  if (helplinesSnapIsOn) {
    // try to snap to help lines
    QValueList<float>::Iterator i;
    for (i = vertHelplines.begin (); i != vertHelplines.end (); ++i) {
      if (fabs (*i - pos) <= 10.0) {
        pos = *i;
        snap = true;
        break;
      }
    }
  }
  if (gridSnapIsOn && ! snap) {
    int n = (int) (pos / hGridDistance);
    float r = fmod (pos, hGridDistance);
    if (r > (hGridDistance / 2.0))
      n++;
    pos = hGridDistance * n;
  }
  return pos;
}

float Canvas::snapYPositionToGrid (float pos) {
  bool snap = false;

  if (helplinesSnapIsOn) {
    // try to snap to help lines
    QValueList<float>::Iterator i;
    for (i = horizHelplines.begin (); i != horizHelplines.end (); ++i) {
      if (fabs (*i - pos) <= 10.0) {
        pos = *i;
        snap = true;
        break;
      }
    }
  }
  if (gridSnapIsOn && ! snap) {
    int n = (int) (pos / vGridDistance);
    float r = fmod (pos, vGridDistance);
    if (r > (vGridDistance / 2.0))
      n++;
    pos = vGridDistance * n;
  }
  return pos;
}

void Canvas::snapPositionToGrid (float& x, float& y) {
  bool snap = false;

  if (helplinesSnapIsOn) {
    // try to snap to help lines
    QValueList<float>::Iterator i;
    for (i = horizHelplines.begin (); i != horizHelplines.end (); ++i) {
      if (fabs (*i - y) <= 10.0) {
        y = *i;
        snap = true;
        break;
      }
    }
    for (i = vertHelplines.begin (); i != vertHelplines.end (); ++i) {
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
    else if (e->type () == QEvent::MouseButtonRelease && e->button () == LeftButton)
      dragging = false;
    else if (e->type () == QEvent::MouseMove && dragging)
      scrollview->ensureVisible (e->x (), e->y (), 10, 10);
  }

  if (e->button () == RightButton &&
      e->type () == QEvent::MouseButtonPress && ! toolController->getActiveTool ()->consumesRMBEvents ()) {
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
    // For large zoom levels there is no pixmap to copy. So we
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

  QPainter p;
  float s = scaleFactor ();
  int w = document->getPaperWidth (), h = document->getPaperHeight ();

  // setup the painter
  pdev = (pixmap ? (QPaintDevice *) pixmap : (QPaintDevice *) this);
  p.begin (pdev);
  p.setBackgroundColor(white);
  if (pixmap)
    pixmap->fill (backgroundColor ());

  p.scale (s, s);

  // clear the canvas
  //  p.translate (1, 1);
  p.eraseRect (0, 0, w, h);

  p.setPen(Qt::black);
  p.drawRect (0, 0, w - 2, h - 2);
  p.setPen (QPen(Qt::darkGray, 2));
  p.moveTo (w-1, 0);
  p.lineTo (w-1, h);
  p.moveTo(w, h-1);
  p.lineTo (0, h-1);
  p.setPen(Qt::black);

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

  QPainter p;
  float s = scaleFactor ();

  // compute the clipping region
  QWMatrix m;
  m.scale (s, s);

  QRect clip = m.map (QRect (int (r.left ()), int (r.top ()),
                             int (r.width ()), int (r.height ())));

  QPaintDevice *pdev = (pixmap ? (QPaintDevice *) pixmap : (QPaintDevice *) this);
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

void Canvas::drawHelplines (QPainter& p) {
  int pw = document->getPaperWidth ();
  int ph = document->getPaperHeight ();

  QPen pen (blue, 0, DashLine);

  p.save ();
  p.setPen (pen);
  QValueList<float>::Iterator i;
  for (i=horizHelplines.begin(); i!=horizHelplines.end(); ++i) {
    int hi = qRound (*i);
    p.drawLine (0, hi, pw, hi);
  }
  for (i = vertHelplines.begin(); i!=vertHelplines.end(); ++i) {
    int vi = qRound (*i);
    p.drawLine (vi, 0, vi, ph);
  }

  if (tmpHorizHelpline != -1) {
    int hi = qRound (tmpHorizHelpline);
    p.drawLine (0, hi, pw, hi);
  }

  if (tmpVertHelpline != -1) {
    int vi = qRound (tmpVertHelpline);
    p.drawLine (vi, 0, vi, ph);
  }
  p.restore ();
}

void Canvas::drawGrid (QPainter& p) {
  int pw = document->getPaperWidth ();
  int ph = document->getPaperHeight ();
  float h, v;

  QPen pen1 (mGridColor, 0, DotLine);

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

void Canvas::setupPrinter( QPrinter &printer )
{
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
  printer.setFullPage(true);
}

void Canvas::print( QPrinter &printer )
{
    QPainter paint;
    paint.begin (&printer);
    paint.setClipping (false);
    document->drawContents (paint);
    paint.end ();
}

void Canvas::readGridProperties () {
  KConfig* config = kapp->config ();
  QString oldgroup = config->group ();

  config->setGroup ("Grid");

  vGridDistance = (float) config->readDoubleNumEntry ("vGridDistance", 50.0);
  hGridDistance = (float) config->readDoubleNumEntry ("hGridDistance", 50.0);
  gridIsOn = config->readBoolEntry ("showGrid", false);
  gridSnapIsOn = config->readBoolEntry ("snapTopGrid", false);
  mGridColor = config->readColorEntry ("GridColor", &mGridColor);

  config->setGroup ("Helplines");
  helplinesAreOn = config->readBoolEntry ("showHelplines");
  helplinesSnapIsOn = config->readBoolEntry ("snapTopHelplines");
  document->layerForHelplines ()->setVisible (helplinesAreOn);

  config->setGroup (oldgroup);
}

void Canvas::saveGridProperties () {
  KConfig* config = kapp->config ();
  QString oldgroup = config->group ();

  config->setGroup ("Grid");

  config->writeEntry ("vGridDistance", (double) vGridDistance);
  config->writeEntry ("hGridDistance", (double) hGridDistance);
  config->writeEntry ("showGrid", gridIsOn);
  config->writeEntry ("snapTopGrid", gridSnapIsOn);
  config->writeEntry ("GridColor", mGridColor);
  
  config->setGroup ("Helplines");
  config->writeEntry ("showHelplines", helplinesAreOn);
  config->writeEntry ("snapTopHelplines", helplinesSnapIsOn);

  config->setGroup (oldgroup);
  config->sync ();
}

void Canvas::drawTmpHelpline (int x, int y, bool horizH) {
  float pos = -1;
  // convert into document coordinates
  // and add helpline
  if (horizH) {
    pos = (float) y / zoomFactor - this->y ();
    tmpHorizHelpline = pos;
  }
  else {
    pos = (float) x / zoomFactor - this->x ();
    tmpVertHelpline = pos;
  }
  // it makes no sense to hide helplines yet
  showHelplines (true);
  if (helplinesAreOn)
    updateView ();
}

void Canvas::addHelpline (int x, int y, bool horizH) {
  float pos = -1;
  tmpHorizHelpline = tmpVertHelpline = -1;
  // convert into document coordinates
  // and add helpline
  if (horizH) {
    pos = (float) y / zoomFactor - this->y ();
    addHorizHelpline (pos);
  }
  else {
    pos = (float) x / zoomFactor - this->x ();
    addVertHelpline (pos);
  }
}

void Canvas::addHorizHelpline  (float pos) {
  horizHelplines.append(pos);
  if (helplinesAreOn)
    updateView ();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}

void Canvas::addVertHelpline  (float pos) {
  vertHelplines.append(pos);
  if (helplinesAreOn)
    updateView ();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}

void Canvas::setHorizHelplines (const QValueList<float>& lines) {
  horizHelplines = lines;
  if (helplinesAreOn)
    updateView ();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}

void Canvas::setVertHelplines (const QValueList<float>& lines) {
  vertHelplines = lines;
  if (helplinesAreOn)
    updateView ();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}

const QValueList<float>& Canvas::getHorizHelplines () const {
  return horizHelplines;
}

const QValueList<float>& Canvas::getVertHelplines () const {
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
    document->layerForHelplines ()->setVisible (helplinesAreOn);
    updateView ();
    emit gridStatusChanged ();
    saveGridProperties ();
  }
}

bool Canvas::showHelplines () {
  return helplinesAreOn;
}

int Canvas::indexOfHorizHelpline (float pos) {
    int ret=0;
    for (QValueList<float>::Iterator i = horizHelplines.begin(); i!=horizHelplines.end(); ++i, ++ret) {
        if (pos - NEAR_DISTANCE < *i &&
            pos + NEAR_DISTANCE > *i)
            return ret;
    }
    return -1;
}

int Canvas::indexOfVertHelpline (float pos) {
    int ret=0;
    for (QValueList<float>::Iterator i = vertHelplines.begin(); i!=vertHelplines.end(); ++i, ++ret) {
        if (pos - NEAR_DISTANCE < *i &&
            pos + NEAR_DISTANCE > *i)
            return ret;
    }
    return -1;
}

void Canvas::updateHorizHelpline (int idx, float pos) {
  horizHelplines[idx] = pos;
  updateView ();
}

void Canvas::updateVertHelpline (int idx, float pos) {
  vertHelplines[idx] = pos;
  updateView ();
}

void Canvas::updateHelplines () {
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}

void Canvas::updateGridInfos () {
  document->getGrid (hGridDistance, vGridDistance, gridSnapIsOn);
  document->getHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
  if (helplinesAreOn != document->layerForHelplines ()->isVisible ())
    showHelplines (document->layerForHelplines ()->isVisible ());
  else {
    saveGridProperties ();
    emit gridStatusChanged ();
  }
}

bool Canvas::eventFilter (QObject *o, QEvent *e) {
  if (e->type () == QEvent::KeyPress) {
    QKeyEvent *ke = (QKeyEvent *) e;
    if (ke->key () == Key_Tab) {
      if (toolController->getActiveTool ()->isA ("SelectionTool"))
        ((SelectionTool *)
         toolController->getActiveTool ())->processTabKeyEvent (document,
                                                                this);
    }
    else
      keyPressEvent (ke);
    return true;
  }
  return QWidget::eventFilter(o, e);
}

#include <Canvas.moc>
