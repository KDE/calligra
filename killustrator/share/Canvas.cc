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

#include "Canvas.h"

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
#include <kconfig.h>
#include <kapp.h>
#include <kdebug.h>

#include <GPolyline.h> // for NEAR_DISTANCE
#include <GLayer.h>
#include <SelectionTool.h>

Canvas::Canvas(GDocument *doc, float res, QScrollBar *hb, QScrollBar *vb, QWidget *parent, const char *name)
   : QWidget (parent, name)
{
  document = doc;
  resolution = res;
  zoomFactor = 1.0;
  drawBasePoints = false;
  hBar = hb;
  vBar = vb;

  installEventFilter(this);

  vBar->setLineStep(1);
  hBar->setLineStep(1);

  vBar->setPageStep(10);
  hBar->setPageStep(10);
  
  connect(vBar, SIGNAL(valueChanged(int)), SLOT(scrollY(int)));
  connect(hBar, SIGNAL(valueChanged(int)), SLOT(scrollX(int)));

  connect (document, SIGNAL (changed ()), this, SLOT (repaint ()));
  connect (document, SIGNAL (changed (const Rect&)), this, SLOT (updateRegion (const Rect&)));
  connect (document, SIGNAL (sizeChanged ()), this, SLOT (calculateSize ()));
  connect (document, SIGNAL (handleChanged ()), this, SLOT (repaint ()));
  connect (document, SIGNAL (gridChanged ()), this, SLOT (updateGridInfos ()));

  buffer = new QPixmap();
  xPaper = 0;
  yPaper = 0;

  updateScrollBars();
  
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

Canvas::~Canvas()
 {
  if (buffer != 0L)
    delete buffer;
 }

void Canvas::resizeEvent(QResizeEvent *e)
 {
  buffer->resize(size());
  mXOffset = (width() - actualPaperSizePt().width())/2 - xPaper;
  mYOffset = (height() - actualPaperSizePt().height())/2 - yPaper;
  emit visibleAreaChanged(mXOffset, mYOffset);
 }

QSize Canvas::actualSize()
 {
  int w = width() + actualPaperSizePt().width();
  int h = height() + actualPaperSizePt().height();
  return QSize(w, h);
 }

QSize Canvas::actualPaperSizePt() const
 {
  int w = (int) (document->getPaperWidth () * resolution * zoomFactor / 72.0);
  int h = (int) (document->getPaperHeight () * resolution * zoomFactor / 72.0);
  return QSize(w, h);
 }

void Canvas::updateScrollBars()
{
   hBar->setRange(-actualSize().width()/2,actualSize().width()/2);
   vBar->setRange(-actualSize().height()/2,actualSize().height()/2);
  
   if(hBar->value() > hBar->maxValue() || hBar->value() < hBar->minValue())
      hBar->setValue(0);

   if(vBar->value() > vBar->maxValue() || vBar->value() < vBar->minValue())
      vBar->setValue(0);
 }

void Canvas::scrollX(int v)
 {
  xPaper = v;
  mXOffset = (width() - actualPaperSizePt().width())/2 - xPaper;
  repaint();
  emit visibleAreaChanged((width() - actualPaperSizePt().width())/2-xPaper,(height() - actualPaperSizePt().height())/2 -yPaper);
 }
 
void Canvas::scrollY(int v)
 {
  yPaper = v;

  mYOffset = (height() - actualPaperSizePt().height())/2 - yPaper;
  repaint();
  emit visibleAreaChanged((width() - actualPaperSizePt().width())/2-xPaper,(height() - actualPaperSizePt().height())/2 -yPaper);
 }

void Canvas::centerPage()
 {
  hBar->setValue(0);
  vBar->setValue(0);
 }

/*
    Draw
*/

void Canvas::paintEvent (QPaintEvent* e)
 {
   //kdDebug()<<"Canvas::paintEvent(): width: "<<width()<<" height: "<<height()<<endl;
  pendingRedraws = 0;

  QPainter p;
  float s = scaleFactor ();

  // setup the painter
  p.begin (buffer);
  p.setBackgroundColor(white);
  buffer->fill (white);


  // clear the canvas
  
  // draw the grid
  if(gridIsOn)
   drawGrid (p);
  
  p.save();
  
  int w = (int) (document->getPaperWidth () * resolution * zoomFactor / 72.0);
  int h = (int) (document->getPaperHeight () * resolution * zoomFactor / 72.0);
  p.setPen(Qt::black);
  p.translate(mXOffset, mYOffset);
  p.drawRect (0, 0, w, h);
  p.setPen (QPen(Qt::darkGray, 2));
  p.moveTo (w+1, 1);
  p.lineTo (w+1, h+1);
  p.moveTo(w, h+1);
  p.lineTo (1, h+1);
  p.setPen(Qt::black);

  // next the document contents
  p.scale (s, s);
  document->drawContents (p, drawBasePoints, outlineMode);

  // and finally the handle
  if (! document->selectionIsEmpty ())
   document->handle ().draw (p);
  
  p.restore();

  // draw the help lines
  if (helplinesAreOn)
    drawHelplines (p);

  p.end ();

  const QRect& rect = e->rect ();
  bitBlt (this, rect.x (), rect.y (), buffer,
            rect.x (), rect.y (), rect.width (), rect.height ());
 }

void Canvas::ensureVisibility (bool flag) {
  ensureVisibilityFlag = flag;
}

void Canvas::calculateSize ()
{
   //kdDebug()<<"Canvas::calcSize(): width: "<<width()<<" height: "<<height()<<endl;
   buffer->resize(size());
   repaint();
   emit sizeChanged ();
}

void Canvas::setZoomFactor (float factor)
 {
  zoomFactor = factor;
  // recompute pixmaps of fill areas
  document->invalidateClipRegions ();
  updateScrollBars();
  mXOffset = (width() - actualPaperSizePt().width())/2 - xPaper;
  mYOffset = (height() - actualPaperSizePt().height())/2 - yPaper;
  repaint();
  emit sizeChanged ();
  emit visibleAreaChanged((width() - actualPaperSizePt().width())/2-xPaper,(height() - actualPaperSizePt().height())/2 -yPaper);
  //emit zoomFactorChanged (zoomFactor, x/2 ,y/2);
  emit zoomFactorChanged (zoomFactor);
 }

void Canvas::setToolController (ToolController* tc)
 {
  toolController = tc;
 }

/*******************[Events]*******/

void Canvas::mousePressEvent (QMouseEvent* e)
 {
  propagateMouseEvent (e);
 }

void Canvas::mouseReleaseEvent (QMouseEvent* e)
 {
  propagateMouseEvent (e);
 }

void Canvas::mouseMoveEvent (QMouseEvent* e)
 {
  propagateMouseEvent (e);
 }

void Canvas::propagateMouseEvent (QMouseEvent *e)
{
  // transform position of the mouse pointer according to current
  // zoom factor
  QPoint new_pos (qRound (float(e->x() - mXOffset) / zoomFactor),
                  qRound (float(e->y() - mYOffset) / zoomFactor));
  QMouseEvent new_ev (e->type (), new_pos, e->button (), e->state ());
  
  emit mousePositionChanged (new_ev.x(), new_ev.y());

  // ensure visibility
  if (ensureVisibilityFlag)
  {
    if (e->type () == QEvent::MouseButtonPress && e->button () == LeftButton)
      dragging = true;
    else if (e->type () == QEvent::MouseButtonRelease && e->button () == LeftButton)
      dragging = false;
    else if (e->type () == QEvent::MouseMove && dragging);
//      scrollview->ensureVisible (e->x (), e->y (), 10, 10);
  }

  if (e->button()==RightButton && e->type()==QEvent::MouseButtonPress)
  {
     if (document->selectionIsEmpty ())
     {
        GObject* obj = document->findContainingObject (new_pos.x (),
                                                       new_pos.y ());
        if (obj)
        {
           // pop up menu for the picked object
           emit rightButtonAtObjectClicked (e->x (), e->y (), obj);
        }
        else
        {
           emit rightButtonClicked (e->x (), e->y ());
        }
     }
     else
     {
        // pop up menu for the current selection
        emit rightButtonAtSelectionClicked (e->x (), e->y ());
     }
     return;
  }
  else if (toolController)
  {
     // the tool controller processes the event
     toolController->delegateEvent (&new_ev, document, this);
  }
}

void Canvas::keyPressEvent (QKeyEvent* e)
{
   if (toolController)
      toolController->delegateEvent (e, document, this);
}

bool Canvas::eventFilter (QObject *o, QEvent *e)
{
   if(e->type () == QEvent::KeyPress)
   {
      QKeyEvent *ke = (QKeyEvent *) e;
      if (ke->key () == Key_Tab)
      {
         if (toolController->getActiveTool ()->id()==Tool::ToolSelect)
            ((SelectionTool *)
             toolController->getActiveTool ())->processTabKeyEvent (document,this);
      }
      else
         keyPressEvent (ke);
      return true;
   }
   return QWidget::eventFilter(o, e);
}

void Canvas::moveEvent(QMoveEvent *e) {
    emit visibleAreaChanged (e->pos ().x (), e->pos ().y ());
}



void Canvas::setDocument(GDocument* doc)
 {
  document = doc;
  updateGridInfos ();
  connect (document, SIGNAL (changed ()), this, SLOT (repaint ()));
  connect (document, SIGNAL (gridChanged ()), this, SLOT (updateGridInfos ()));
 }

void Canvas::showBasePoints (bool flag) {
  drawBasePoints = flag;
  repaint();
}

void Canvas::setOutlineMode (bool flag) {
  if (outlineMode != flag) {
    outlineMode = flag;
    repaint();
  }
}

void Canvas::retryUpdateRegion () {
  updateRegion (region);
}

void Canvas::updateRegion (const Rect& reg)
 {
  if (pendingRedraws == 0 && document->selectionCount () > 1)
   {
    // we have to update a multiple selection, so we collect
    // the update regions and redraw it in one call
    pendingRedraws = document->selectionCount () - 1;
    regionForUpdate = reg;
    return;
   }

  Rect r = reg;

  if (pendingRedraws > 0)
   {
    regionForUpdate = regionForUpdate.unite (r);
    pendingRedraws--;
    if (pendingRedraws > 0)
      // not the last redraw call
      return;
    else
      r = regionForUpdate;
   }

/*  QPainter p;
  float s = scaleFactor ();*/

  // compute the clipping region
  QWMatrix m;
  
  QRect clip = m.map (QRect (int (r.left ()*zoomFactor + mXOffset), int (r.top ()*zoomFactor + mYOffset),
                             int (r.width ()), int (r.height ())));
   
  //kdDebug(0) << "("<< clip.left() << "," << clip.top() << ")-(" << clip.right() << "," << r.bottom() << ")" << endl;

  

  // setup the clip region
  if (clip.x () <= 1) clip.setX (1);
  if (clip.y () <= 1) clip.setY (1);

//  int mw = (int) ((float) document->getPaperWidth () * s);
//  int mh = (int) ((float) document->getPaperHeight () * s);

  if (clip.right () >= width())
    clip.setRight (width());
  if (clip.bottom () >= height())
    clip.setBottom (height());
  repaint (clip, false);
  /*
  // setup the painter
  p.begin (buffer);
  p.setBackgroundColor(white);
  //buffer->fill (white);
  p.setClipRect (clip);

  p.eraseRect (rr.left (), rr.top (), rr.width (), rr.height ());
  
//  p.fillRect(rr.left (), rr.top (), rr.width (), rr.height (),red);
  
  if(gridIsOn)
   drawGrid (p);
   
  int w = (int) (document->getPaperWidth () * resolution * zoomFactor / 72.0);
  int h = (int) (document->getPaperHeight () * resolution * zoomFactor / 72.0);
  p.setPen(Qt::black);
  p.translate((width()-w)/2-xPaper, (height()-h)/2-yPaper);
  p.drawRect (0, 0, w, h);
  p.setPen (QPen(Qt::darkGray, 2));
  p.moveTo (w+1, 1);
  p.lineTo (w+1, h+1);
  p.moveTo(w, h+1);
  p.lineTo (1, h+1);
  p.setPen(Qt::black);
  // clear the canvas
  p.scale (s, s);
  //  p.translate (1, 1);
  

  // draw the grid
  

  // draw the help lines
//  if (helplinesAreOn)
//    drawHelplines (p);

  // next the document contents
  document->drawContentsInRegion (p, r, rr, drawBasePoints, outlineMode);

  // and finally the handle
  if (! document->selectionIsEmpty ())
    document->handle ().draw (p);

  p.end ();
  bitBlt (this, rr.x (), rr.y (), buffer,
            rr.x (), rr.y (), rr.width (), rr.height ());*/
 }

/***********************[PRINTER]*************************/

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

/*************************[GRID]*************************/

void Canvas::showGrid (bool flag)
 {
  if (gridIsOn != flag) {
    gridIsOn = flag;
    repaint();
    //emit gridStatusChanged ();
    //saveGridProperties ();
  }
 }

void Canvas::snapToGrid (bool flag)
{
   if (gridSnapIsOn != flag)
   {
      gridSnapIsOn = flag;
      //saveGridProperties ();
      //emit gridStatusChanged ();
      document->setGrid (hGridDistance, vGridDistance, gridSnapIsOn);
   }
}

void Canvas::setGridColor(QColor color)
{
    mGridColor = color;
    //saveGridProperties ();
}

void Canvas::setGridDistance (float hdist, float vdist)
{
   hGridDistance = hdist;
   vGridDistance = vdist;
   //saveGridProperties ();
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

  if (helplinesSnapIsOn || gridSnapIsOn)
   {
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

float Canvas::snapYPositionToGrid (float pos)
 {
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

void Canvas::drawGrid (QPainter& p)
{
   float h, v;
   float hd = hGridDistance * zoomFactor;
   float vd = vGridDistance * zoomFactor;
   QPen pen1 (mGridColor, 0);

   p.save ();
   p.setPen (pen1);
   h = ((width() - actualPaperSizePt().width())/2 - xPaper) % (int)hd;
   for (; h < width(); h += hd)
   {
      int hi = qRound (h);
      p.drawLine (hi, 0, hi, height());
   }
  
   v = ((height() - actualPaperSizePt().height())/2 - yPaper) % (int)vd;
  
   for (; v < height() ; v += vd)
   {
      int vi = qRound (v);
      p.drawLine (0, vi, width(), vi);
   }
   p.restore ();
}

void Canvas::readGridProperties ()
{
   kdDebug()<<"Canvas::readGridProps()"<<endl;
  KConfig* config = kapp->config ();

  config->setGroup ("Grid");

  vGridDistance = (float) config->readDoubleNumEntry ("vGridDistance", 50.0);
  hGridDistance = (float) config->readDoubleNumEntry ("hGridDistance", 50.0);
  gridIsOn = config->readBoolEntry ("showGrid", false);
  gridSnapIsOn = config->readBoolEntry ("snapToGrid", false);
  mGridColor = config->readColorEntry ("GridColor", &mGridColor);
  //kdDebug()<<"vGridDistance: "<<vGridDistance<<endl;
  //kdDebug()<<"hGridDistance: "<<hGridDistance<<endl;

  config->setGroup ("Helplines");
  helplinesAreOn = config->readBoolEntry ("showHelplines");
  helplinesSnapIsOn = config->readBoolEntry ("snapToHelplines");
  document->layerForHelplines ()->setVisible (helplinesAreOn);
}

void Canvas::saveGridProperties ()
{
   kdDebug()<<"Canvas::saveGridProps()"<<endl;
  KConfig* config = kapp->config ();

  config->setGroup ("Grid");

  config->writeEntry ("vGridDistance", (double) vGridDistance);
  config->writeEntry ("hGridDistance", (double) hGridDistance);
  config->writeEntry ("showGrid", gridIsOn);
  config->writeEntry ("snapToGrid", gridSnapIsOn);
  config->writeEntry ("GridColor", mGridColor);
  //kdDebug()<<"vGridDistance: "<<vGridDistance<<endl;
  //kdDebug()<<"hGridDistance: "<<hGridDistance<<endl;
  
  config->setGroup ("Helplines");
  config->writeEntry ("showHelplines", helplinesAreOn);
  config->writeEntry ("snapToHelplines", helplinesSnapIsOn);

  config->sync ();
}

void Canvas::updateGridInfos ()
{
   document->getGrid (hGridDistance, vGridDistance, gridSnapIsOn);
   document->getHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
   if (helplinesAreOn != document->layerForHelplines ()->isVisible ())
      showHelplines (document->layerForHelplines ()->isVisible ());
   else
   {
      //saveGridProperties ();
      //emit gridStatusChanged ();
   }
}

/***************************HELPLINES********************/

void Canvas::drawHelplines (QPainter& p)
 {
  QPen pen (blue, 0);//, DashLine);

  p.save ();
  p.setPen (pen);
  QValueList<float>::Iterator i;
  for (i=horizHelplines.begin(); i!=horizHelplines.end(); ++i) {
    int hi = qRound (*i * zoomFactor) + mYOffset;
    p.drawLine (0, hi, width(), hi);
  }
  for (i = vertHelplines.begin(); i!=vertHelplines.end(); ++i) {
    int vi = qRound (*i * zoomFactor) + mXOffset;
    p.drawLine (vi, 0, vi, height());
  }

  if (tmpHorizHelpline != -1) {
    int hi = qRound (tmpHorizHelpline * zoomFactor) + mYOffset;
    p.drawLine (0, hi, width(), hi);
  }

  if (tmpVertHelpline != -1) {
    int vi = qRound (tmpVertHelpline * zoomFactor) + mXOffset;
    p.drawLine (vi, 0, vi, height());
  }
  p.restore ();
 }

void Canvas::drawTmpHelpline (int x, int y, bool horizH)
 {
  float pos = -1;
  // convert into document coordinates
  // and add helpline
  if(horizH)
   {
    pos = float(y - mYOffset) / zoomFactor;
    tmpHorizHelpline = pos;
   }
  else
   {
    pos = float(x - mXOffset) / zoomFactor;
    tmpVertHelpline = pos;
   }
  // it makes no sense to hide helplines yet
  showHelplines (true);
  if(helplinesAreOn)
   repaint();
 }

void Canvas::addHelpline (int x, int y, bool horizH)
 {
  float pos = -1;
  tmpHorizHelpline = tmpVertHelpline = -1;
  // convert into document coordinates
  // and add helpline
  if (horizH)
   {
    pos = float(y - mYOffset) / zoomFactor;
    addHorizHelpline (pos);
   }
  else
   {
    pos = float(x - mXOffset) / zoomFactor;
    addVertHelpline (pos);
   }
 }

void Canvas::addHorizHelpline(float pos)
 {
  horizHelplines.append(pos);
  if (helplinesAreOn)
   repaint();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
 }

void Canvas::addVertHelpline(float pos)
 {
  vertHelplines.append(pos);
  if (helplinesAreOn)
   repaint();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
 }

void Canvas::setHorizHelplines (const QValueList<float>& lines) {
  horizHelplines = lines;
  if (helplinesAreOn)
    repaint();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}

void Canvas::setVertHelplines (const QValueList<float>& lines) {
  vertHelplines = lines;
  if (helplinesAreOn)
    repaint();
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}

const QValueList<float>& Canvas::getHorizHelplines () const {
  return horizHelplines;
}

const QValueList<float>& Canvas::getVertHelplines () const {
  return vertHelplines;
}

void Canvas::alignToHelplines (bool flag)
{
   if (helplinesSnapIsOn!=flag)
   {
      helplinesSnapIsOn = flag;
      //emit gridStatusChanged ();
      document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
   };
}

bool Canvas::alignToHelplines () {
  return helplinesSnapIsOn;
}

void Canvas::showHelplines (bool flag)
{
   if (helplinesAreOn != flag)
   {
      helplinesAreOn = flag;
      document->layerForHelplines ()->setVisible (helplinesAreOn);
      repaint();
      //emit gridStatusChanged ();
      //saveGridProperties ();
   }
}

bool Canvas::showHelplines ()
{
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
  repaint();
}

void Canvas::updateVertHelpline (int idx, float pos) {
  vertHelplines[idx] = pos;
  repaint();
}

void Canvas::updateHelplines () {
  document->setHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
}



#include <Canvas.moc>
