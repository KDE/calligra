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
#include <qrect.h>


#include <kconfig.h>
#include <kapp.h>
#include <kdebug.h>

#include "GDocument.h"
#include "GPage.h"
#include "Handle.h"
#include "ToolController.h"
#include "GPolyline.h" // for NEAR_DISTANCE
#include "GLayer.h"
#include "SelectionTool.h"


Canvas::Canvas(GDocument *doc, float res, QScrollBar *hb, QScrollBar *vb, QWidget *parent, const char *name)
:QWidget (parent, name)
,hBar(hb)
,vBar(vb)
,resolution(res)
,zoomFactor(1.0)
,document(doc)
,pendingRedraws(0)
,tmpHorizHelpline(-1.0)
,tmpVertHelpline(-1.)
,helplinesSnapIsOn(false)
,helplinesAreOn(false)
,dragging(false)
,ensureVisibilityFlag(false)
,drawBasePoints(false)
,outlineMode(false)
,guiActive(false)
{
  installEventFilter(this);

  vBar->setLineStep(5);
  hBar->setLineStep(5);

  vBar->setPageStep(50);
  hBar->setPageStep(50);
  
  //these connections shall only work when caused by the user
  connect(vBar, SIGNAL(valueChanged(int)), SLOT(scroll()));
  connect(hBar, SIGNAL(valueChanged(int)), SLOT(scroll()));

  connect (document, SIGNAL (changed ()), this, SLOT (repaint ()));
  connect (document, SIGNAL (changed (const Rect&)), this, SLOT (updateRegion (const Rect&)));

  connect (document, SIGNAL (sizeChanged ()), this, SLOT (docSizeChanged()));

  connect (document, SIGNAL (handleChanged ()), this, SLOT (repaint ()));
  connect (document, SIGNAL (gridChanged ()), this, SLOT (updateGridInfos ()));

  buffer = new QPixmap();
  
  mGridColor = blue;

  readGridProperties ();
  updateGridInfos ();
  
  setFocusPolicy (StrongFocus);
  setMouseTracking (true);
  setBackgroundMode (NoBackground);
}

Canvas::~Canvas()
 {
  if (buffer != 0L)
    delete buffer;
 }

void Canvas::resizeEvent(QResizeEvent *e)
{
   if (!guiActive) return;
   //kdDebug()<<"Canvas::resizeEvent() width() "<<width()<<"  height(): "<<height()<<endl;
   //this happens exactly once on app start, so doing nothing in this case saves some time, aleXXX
   if ((width()==10) && (height()==10)) return;
   buffer->resize(size());
   //at first this one, since the others depend on the scrollbars
   //this may trigger the signal QScrollBar::valueChanged(), which is connected to
   //scroll(), which emit visibleAreaCVhanged(), but the following calls
   //also change the visible areas, and we want to emit the signals only once when we
   //have finished
   blockSignals(true);
   hBar->blockSignals(true);
   vBar->blockSignals(true);
   adjustScrollBarRanges2();

   //this one changes
   adjustVisibleArea4();
   //and this one too and depends on the visibleArea
   adjustRelativePaperArea5();
   blockSignals(false);
   hBar->blockSignals(false);
   vBar->blockSignals(false);

   emit visibleAreaChanged(m_visibleArea);
   //kdDebug()<<"Canvas::resizeEvent() paperArea: ( "<<m_paperArea.left()<<" | "<<m_paperArea.top()<<" ) - ( "<<m_paperArea.right()<<" | "<<m_paperArea.bottom()<<" )"<<endl;
   //kdDebug()<<"Canvas::resizeEvent() visibleArea: ( "<<m_visibleArea.left()<<" | "<<m_visibleArea.top()<<" ) - ( "<<m_visibleArea.right()<<" | "<<m_visibleArea.bottom()<<" )"<<endl;
   //kdDebug()<<"Canvas::resizeEvent() relativePaperArea: ( "<<m_relativePaperArea.left()<<" | "<<m_relativePaperArea.top()<<" ) - ( "<<m_relativePaperArea.right()<<" | "<<m_relativePaperArea.bottom()<<" )"<<endl;
}

//return the rectangle covered by the paper in points
//upper left corner is always (0,0)
//it changes only when zooming or changing the paper size
void Canvas::adjustPaperArea1()
{
   int w = (int) (document->activePage()->getPaperWidth () * resolution * zoomFactor / 72.0);
   int h = (int) (document->activePage()->getPaperHeight () * resolution * zoomFactor / 72.0);
   m_paperArea=QRect(QPoint(0,0),QPoint(w,h));
};

//adjust the scrollbars to the current zoom level and canvas size
void Canvas::adjustScrollBarRanges2()
{
   //the range of a scrollbar depends on how much of the canvas is not visible
   QRect tmpRect=paperArea();
   //QSize tmpSize=currentPaperSizePt();
   int i=tmpRect.right()-width();
   if (i<=0)
      hBar->setRange(0,0);
   else
      hBar->setRange(-i/2-10,i/2+10);
      //hBar->setRange(-i/2-10,i/2+10);

   i=tmpRect.bottom()-height();
   if (i<=0)
      vBar->setRange(0,0);
   else
      vBar->setRange(-i/2-10,i/2+10);

   hBar->setValue(hBar->value());
   vBar->setValue(vBar->value());
   //kdDebug()<<"Canvas::adjustScrollBarRanges(): hBar min: "<<hBar->minValue()<<" max: "<<hBar->maxValue()<<" value: "<<hBar->value()<<endl;
   //kdDebug()<<"Canvas::adjustScrollBarRanges(): vBar min: "<<vBar->minValue()<<" max: "<<vBar->maxValue()<<" value: "<<vBar->value()<<endl;

}

//x and y are the coordinates of the paper which will beceome centered
//on the screen
void Canvas::adjustScrollBarPositions3(int x, int y)
{
   if (hBar->minValue()!=hBar->maxValue())
   {
      hBar->setValue(x-m_paperArea.right()/2);
      //kdDebug()<<"Canvas::adjustScrollBarPositions() new x pos: "<<hBar->value()<<endl;
   };
   if (vBar->minValue()!=vBar->maxValue())
      vBar->setValue(y-m_paperArea.bottom()/2);
};


void Canvas::adjustVisibleArea4()
{
   //kdDebug()<<"Canvas::adjustVisibleArea() hBar: "<<hBar->value()<<" vBar: "<<vBar->value()<<endl;

   QRect tmpRect=paperArea();
   int w=tmpRect.width();
   int h=tmpRect.height();

   int widgetCenterX(width()/2);
   int widgetCenterY(height()/2);

   //e.g. 600 point widget, 800 point paper, 100 point scroll to the right
   //gives 400-300-100=0
   int firstVisX=w/2-widgetCenterX+hBar->value();
   int firstVisY=h/2-widgetCenterY+vBar->value();

   m_visibleArea=QRect(QPoint(firstVisX,firstVisY),QSize(width(),height()));
};

//this one is called when zooming, resizing, scrolling
//coordinates are relative to the edges of the canvas widgets
//changes after adjustPaperArea()
void Canvas::adjustRelativePaperArea5()
{
   QRect visRect=visibleArea();
   QRect paperRect=paperArea();
   m_relativePaperArea=QRect(QPoint(-visRect.x(),-visRect.y()),
                             QPoint(+visRect.right()-paperRect.right(),+visRect.bottom()-paperRect.bottom()));

};

//connected to QScrollBar::valueChanged()
void Canvas::scroll()
{
   if (!guiActive) return;
   adjustVisibleArea4();
   adjustRelativePaperArea5();

   //kdDebug()<<"Canvas::scroll() width() "<<width()<<"  height "<<height()<<endl;
   //kdDebug()<<"Canvas::scroll() paperArea:         ( "<<m_paperArea.left()<<" | "<<m_paperArea.top()<<" ) - ( "<<m_paperArea.right()<<" | "<<m_paperArea.bottom()<<" )"<<endl;
   //kdDebug()<<"Canvas::scroll() visibleArea:       ( "<<m_visibleArea.left()<<" | "<<m_visibleArea.top()<<" ) - ( "<<m_visibleArea.right()<<" | "<<m_visibleArea.bottom()<<" )"<<endl;
   //kdDebug()<<"Canvas::scroll() relativePaperArea: ( "<<m_relativePaperArea.left()<<" | "<<m_relativePaperArea.top()<<" ) - ( "<<m_relativePaperArea.right()<<" | "<<m_relativePaperArea.bottom()<<" )"<<endl;
   repaint();
   emit visibleAreaChanged(m_visibleArea);
}

void Canvas::center(int x, int y)
{
   hBar->setValue(x);
   vBar->setValue(y);
}

//centerX and centerY are the coordinates of the unzoomed paper
//relative to the center of the paper
void Canvas::setZoomFactor (float factor, int centerX, int centerY)
{
   if (!guiActive) return;
   //kdDebug()<<"Canvas::setZoomFactor() new factor: "<<factor<<" old zoomFactor: "<<zoomFactor<<" centerX: "<<centerX<<" centerY: "<<centerY<<endl;
   //kdDebug()<<"Canvas::setZoomFactor(): visibleArea: ( "<<m_visibleArea.left()<<" | "<<m_visibleArea.top()<<" ) - ( "<<m_visibleArea.right()<<" | "<<m_visibleArea.bottom()<<" )"<<endl;
   //respect zoom factor
   centerX=int(centerX*factor/zoomFactor);
   centerY=int(centerY*factor/zoomFactor);
   //kdDebug()<<"Canvas::setZoomFactor() adjusted centerX: "<<centerX<<" centerY: "<<centerY<<endl;

   zoomFactor = factor;

   blockSignals(true);
   hBar->blockSignals(true);
   vBar->blockSignals(true);

   adjustPaperArea1();
   adjustScrollBarRanges2();
   adjustScrollBarPositions3(centerX,centerY);
   adjustVisibleArea4();
   adjustRelativePaperArea5();

   //kdDebug()<<"Canvas::setZoomFactor() width(): "<<width()<<"  height(): "<<height()<<endl;
   //kdDebug()<<"Canvas::setZoomFactor(): paperArea: ( "<<m_paperArea.left()<<" | "<<m_paperArea.top()<<" ) - ( "<<m_paperArea.right()<<" | "<<m_paperArea.bottom()<<" )"<<endl;
   //kdDebug()<<"Canvas::setZoomFactor(): visibleArea: ( "<<m_visibleArea.left()<<" | "<<m_visibleArea.top()<<" ) - ( "<<m_visibleArea.right()<<" | "<<m_visibleArea.bottom()<<" )"<<endl;
   //kdDebug()<<"Canvas::setZoomFactor(): relativePaperArea: ( "<<m_relativePaperArea.left()<<" | "<<m_relativePaperArea.top()<<" ) - ( "<<m_relativePaperArea.right()<<" | "<<m_relativePaperArea.bottom()<<" )"<<endl;
   // recompute pixmaps of fill areas
   document->activePage()->invalidateClipRegions ();

   repaint();
   blockSignals(false);
   hBar->blockSignals(false);
   vBar->blockSignals(false);

   emit zoomFactorChanged (zoomFactor);
   emit visibleAreaChanged(m_visibleArea);
};

void Canvas::setZoomFactor (float factor)
{
   if (!guiActive) return;
   //the old center should also become the new center
   int centerX((m_visibleArea.right()+m_visibleArea.left())/2);
   int centerY((m_visibleArea.bottom()+m_visibleArea.top())/2);
   //kdDebug()<<"Canvas::setZoomFactor("<<factor<<"): centerX: "<<centerX<<" centerY: "<<centerY<<endl;
   setZoomFactor(factor,centerX, centerY);
}

void Canvas::docSizeChanged()
{
   //kdDebug()<<"Canvas::docSizeChanged()"<<endl;
   guiActive=true;
   blockSignals(true);
   hBar->blockSignals(true);
   vBar->blockSignals(true);

   adjustPaperArea1();
   adjustScrollBarRanges2();
   adjustScrollBarPositions3(0,0);
   adjustVisibleArea4();
   adjustRelativePaperArea5();
   //kdDebug()<<"Canvas::calcSize(): width: "<<width()<<" height: "<<height()<<endl;
   buffer->resize(size());
   repaint();

   blockSignals(false);
   hBar->blockSignals(false);
   vBar->blockSignals(false);
}

/*
    Draw
*/

void Canvas::paintEvent (QPaintEvent* e)
{
   if (!guiActive) return;
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

   int w = (int) (document->activePage()->getPaperWidth () * resolution * zoomFactor / 72.0);
   int h = (int) (document->activePage()->getPaperHeight () * resolution * zoomFactor / 72.0);
   p.setPen(Qt::black);
   p.translate(m_relativePaperArea.left(),m_relativePaperArea.top());
   p.drawRect (0, 0, w, h);
   p.setPen (QPen(Qt::darkGray, 2));
   p.moveTo (w+1, 1);
   p.lineTo (w+1, h+1);
   p.moveTo(w, h+1);
   p.lineTo (1, h+1);
   p.setPen(Qt::black);

   // next the document contents
   p.scale (s, s);
   document->activePage()->drawContents (p, drawBasePoints, outlineMode);

   // and finally the handle
  if (! document->activePage()->selectionIsEmpty ())
   document->activePage()->handle ().draw (p);
  
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
  QPoint new_pos (qRound (float(e->x() + m_visibleArea.left()) / zoomFactor),
                  qRound (float(e->y() + m_visibleArea.top()) / zoomFactor));
  QMouseEvent new_ev (e->type (), new_pos, e->button (), e->state ());
  
  emit mousePositionChanged (e->x(), e->y());

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
     if (document->activePage()->selectionIsEmpty ())
     {
        GObject* obj = document->activePage()->findContainingObject (new_pos.x (),
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

void Canvas::moveEvent(QMoveEvent *e)
{
   //kdDebug()<<"Canvas::moveEvent() ****************"<<endl;
   emit visibleAreaChanged(m_visibleArea);
   //emit visibleAreaChanged (e->pos ().x (), e->pos ().y ());
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
  if (pendingRedraws == 0 && document->activePage()->selectionCount () > 1)
   {
    // we have to update a multiple selection, so we collect
    // the update regions and redraw it in one call
    pendingRedraws = document->activePage()->selectionCount () - 1;
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
  
  QRect clip = m.map (QRect (int (r.left ()*zoomFactor + m_relativePaperArea.left()), int (r.top ()*zoomFactor + m_relativePaperArea.top()),
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
  switch (document->activePage()->pageLayout ().format) {
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
  printer.setOrientation (document->activePage()->pageLayout ().orientation == PG_PORTRAIT ?
                          QPrinter::Portrait : QPrinter::Landscape);
  printer.setFullPage(true);
}

void Canvas::print( QPrinter &printer )
{
    QPainter paint;
    paint.begin (&printer);
    paint.setClipping (false);
    document->activePage()->drawContents (paint);
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

#include <iostream.h>
void Canvas::drawGrid (QPainter& p)
{
   float h, v;
   float hd = hGridDistance * zoomFactor;
   float vd = vGridDistance * zoomFactor;
   QPen pen1 (mGridColor, 0);

   p.save ();
   p.setPen (pen1);
   //correct grid, aleXXX
   int tmp=m_visibleArea.left()/(int)hd;
   if (m_visibleArea.left()>0) tmp++;
   h=tmp*int(vd)-m_visibleArea.left();

   //cerr<<"grid x: ";
   for (; h < width(); h += hd)
   {
      int hi = qRound (h);
      p.drawLine (hi, 0, hi, height());
      //cerr<<h<<" ";
   }
   //cerr<<endl;
   /* example:   vd = 20
    top = 49
    -> v=11 = 60 -49

    top=-49
    -> v=9 = -40- (-49)  */
  
   tmp=m_visibleArea.top()/(int)vd;
   if (m_visibleArea.top()>0) tmp++;
   v=tmp*int(vd)-m_visibleArea.top();

   for (; v < height() ; v += vd)
   {
      int vi = qRound (v);
      p.drawLine (0, vi, width(), vi);
   }

   p.restore ();
}

void Canvas::readGridProperties ()
{
   //kdDebug()<<"Canvas::readGridProps()"<<endl;
   KConfig* config = kapp->config ();

   config->setGroup ("Grid");

   vGridDistance = (float) config->readDoubleNumEntry ("vGridDistance", 50.0);
   hGridDistance = (float) config->readDoubleNumEntry ("hGridDistance", 50.0);
   gridIsOn = config->readBoolEntry ("showGrid", false);
   gridSnapIsOn = config->readBoolEntry ("snapToGrid", false);
   mGridColor = config->readColorEntry ("GridColor", &mGridColor);
   document->setGrid(vGridDistance,hGridDistance,gridSnapIsOn);
   //kdDebug()<<"gridSnapIsOn: "<<int(gridSnapIsOn)<<endl;
   //kdDebug()<<"vGridDistance: "<<vGridDistance<<endl;
   //kdDebug()<<"hGridDistance: "<<hGridDistance<<endl;

   config->setGroup ("Helplines");
   helplinesAreOn = config->readBoolEntry ("showHelplines");
   helplinesSnapIsOn = config->readBoolEntry ("snapToHelplines");
   document->activePage()->layerForHelplines ()->setVisible (helplinesAreOn);


}

void Canvas::saveGridProperties ()
{
   //kdDebug()<<"Canvas::saveGridProps()"<<endl;
  KConfig* config = kapp->config ();

  config->setGroup ("Grid");

  config->writeEntry ("vGridDistance", (double) vGridDistance);
  config->writeEntry ("hGridDistance", (double) hGridDistance);
  config->writeEntry ("showGrid", gridIsOn);
  config->writeEntry ("snapToGrid", gridSnapIsOn);
  config->writeEntry ("GridColor", mGridColor);
  //kdDebug()<<"gridSnapIsOn: "<<int(gridSnapIsOn)<<endl;
  //kdDebug()<<"vGridDistance: "<<vGridDistance<<endl;
  //kdDebug()<<"hGridDistance: "<<hGridDistance<<endl;
  
  config->setGroup ("Helplines");
  config->writeEntry ("showHelplines", helplinesAreOn);
  config->writeEntry ("snapToHelplines", helplinesSnapIsOn);

  config->sync ();
}

void Canvas::updateGridInfos ()
{
   //kdDebug()<<"gridsnapison: "<<int(gridSnapIsOn)<<endl;
   document->getGrid (hGridDistance, vGridDistance, gridSnapIsOn);
   //kdDebug()<<"after getGrid() gridsnapison: "<<int(gridSnapIsOn)<<endl;
   document->getHelplines (horizHelplines, vertHelplines, helplinesSnapIsOn);
   if (helplinesAreOn != document->activePage()->layerForHelplines ()->isVisible ())
      showHelplines (document->activePage()->layerForHelplines ()->isVisible ());
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
    int hi = qRound (*i * zoomFactor) + m_relativePaperArea.top();
    p.drawLine (0, hi, width(), hi);
  }
  for (i = vertHelplines.begin(); i!=vertHelplines.end(); ++i) {
    int vi = qRound (*i * zoomFactor) + m_relativePaperArea.left();
    p.drawLine (vi, 0, vi, height());
  }

  if (tmpHorizHelpline != -1) {
    int hi = qRound (tmpHorizHelpline * zoomFactor) + m_relativePaperArea.top();
    p.drawLine (0, hi, width(), hi);
  }

  if (tmpVertHelpline != -1) {
    int vi = qRound (tmpVertHelpline * zoomFactor) + m_relativePaperArea.left();
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
    pos = float(y - m_relativePaperArea.top()) / zoomFactor;
    tmpHorizHelpline = pos;
   }
  else
   {
    pos = float(x - m_relativePaperArea.left()) / zoomFactor;
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
    pos = float(y - m_relativePaperArea.top()) / zoomFactor;
    addHorizHelpline (pos);
   }
  else
   {
    pos = float(x - m_relativePaperArea.left()) / zoomFactor;
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
      document->activePage()->layerForHelplines ()->setVisible (helplinesAreOn);
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
