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

#include "Canvas.h"

#include <qpainter.h>
#include <qscrollbar.h>
#include <qrect.h>

#include <kdebug.h>

#include "kontour_global.h"
#include "kontour_view.h"
#include "kontour_doc.h"
#include "GPage.h"
#include "ToolController.h"

Canvas::Canvas(GDocument *aGDoc, KontourView *aView, QScrollBar *hb, QScrollBar *vb, QWidget *parent, const char *name)
:QWidget(parent, name)
{
  mGDoc = aGDoc;
  mView = aView;
  hBar = hb;
  vBar = vb;

  installEventFilter(this);
  setFocusPolicy(StrongFocus);
  setMouseTracking(true);
  setBackgroundMode(NoBackground);

  mTmpHorizHelpline = -1;
  mTmpVertHelpline = -1;

  vBar->setLineStep(30);
  hBar->setLineStep(30);

  mXOffset = (width() - mGDoc->xCanvas()) / 2;
  mYOffset = (height() - mGDoc->yCanvas()) / 2;

  mWidthH = width() / 2;
  mHeightH = height() / 2;

  mOutlineMode = false;

  hBar->setRange(-mGDoc->xCanvas(), mGDoc->xCanvas());
  vBar->setRange(-mGDoc->yCanvas(), mGDoc->yCanvas());

  connect(hBar, SIGNAL(valueChanged(int)), SLOT(scrollX(int)));
  connect(vBar, SIGNAL(valueChanged(int)), SLOT(scrollY(int)));

  connect(mGDoc, SIGNAL(changed(const KoRect &)), this, SLOT(updateRegion(const KoRect &)));

//  connect(mGDoc, SIGNAL (sizeChanged ()), this, SLOT (docSizeChanged()));
  connect(mGDoc, SIGNAL(pageChanged()), this, SLOT(changePage()));
  connect(mGDoc, SIGNAL(zoomFactorChanged(double)), this, SLOT(changeZoomFactor(double)));

//  connect(mGDoc, SIGNAL (handleChanged ()), this, SLOT (repaint ()));
//  connect (document, SIGNAL (gridChanged ()), this, SLOT (updateGridInfos ()));

  buffer = new QPixmap();
}

Canvas::~Canvas()
{
  if(buffer)
    delete buffer;
}

void Canvas::outlineMode(bool flag)
{
  if(mOutlineMode != flag)
  {
    mOutlineMode = flag;
    repaint();
  }
}

void Canvas::updateBuf()
{
  updateBuf(geometry());
}

void Canvas::updateBuf(const QRect &rect)
{
  // TODO Kill that
  int w = mGDoc->xCanvas();
  int h = mGDoc->yCanvas();

  QPainter p;
  p.begin(buffer);
  p.setClipRect(rect);
  
  p.setBackgroundColor(mView->workSpaceColor());
  p.eraseRect(rect);
  
  p.save();
  p.translate(mXOffset, mYOffset);
  p.fillRect(1, 1, w - 2, h - 2, QBrush(mGDoc->activePage()->bgColor()));
  p.restore();
  
  if(mGDoc->showGrid())
    drawGrid(p, rect);
  
  p.save();
  p.translate(mXOffset, mYOffset);
  p.setPen(Qt::black);
  p.drawRect(0, 0, w, h);
  p.setPen(QPen(Qt::darkGray, 2));
  p.moveTo(w+1, 1);
  p.lineTo(w+1, h+1);
  p.moveTo(w, h+1);
  p.lineTo(1, h+1);
  p.setPen(Qt::black);

  p.save();
  p.scale(document()->zoomFactor(), document()->zoomFactor());
  document()->activePage()->drawContents(p, false, mOutlineMode);
  p.restore();

  if(!document()->activePage()->selectionIsEmpty())
    document()->activePage()->handle().draw(p);

  p.restore();

  p.end();
}

void Canvas::setXimPosition(int x, int y, int w, int h)
{
  QWidget::setMicroFocusHint(x, y, w, h);
}

void Canvas::center(int x, int y)
{
  hBar->setValue(x);
  vBar->setValue(y);
}

void Canvas::snapPositionToGrid(double &x, double &y)
{
/*  bool snap = false;

  if (mGDoc->snapToHelplines())
  {
    // try to snap to help lines
    QValueList<double>::Iterator i;
    for (i = mGDoc->horizHelplines().begin (); i != mGDoc->horizHelplines().end (); ++i)
    {
      if (fabs (*i - y) <= 10.0)
      {
        y = *i;
        snap = true;
        break;
      }
    }
    for (i = mGDoc->vertHelplines().begin (); i != mGDoc->vertHelplines().end (); ++i)
    {
      if (fabs (*i - x) <= 10.0)
      {
        x = *i;
        snap = true;
        break;
      }
    }
  }
  if (mGDoc->snapToGrid() && ! snap)
  {
    int n = (int) (x / mGDoc->xGrid());
    double r = fmod (x, mGDoc->xGrid());
    if (r > (mGDoc->xGrid() / 2.0))
      n++;
    x = mGDoc->xGrid() * n;

    n = (int) (y / mGDoc->yGrid());
    r = fmod (y, mGDoc->yGrid());
    if (r > (mGDoc->yGrid() / 2.0))
      n++;
    y = mGDoc->yGrid() * n;
  }*/
}

KoRect Canvas::snapTranslatedBoxToGrid(const KoRect &r)
{
/*  double x1, x2, y1, y2;

  if (mGDoc->snapToHelplines() || mGDoc->snapToGrid())
  {
    x1 = snapXPositionToGrid (r.left ());
    x2 = snapXPositionToGrid (r.right ());
    y1 = snapYPositionToGrid (r.top ());
    y2 = snapYPositionToGrid (r.bottom ());

    double x = 0, y = 0;
    if (fabs (r.left () - x1) < fabs (r.right () - x2))
      x = x1;
    else
      x = x2 - r.width ();

    if (fabs (r.top () - y1) < fabs (r.bottom () - y2))
      y = y1;
    else
      y = y2 - r.height ();
    return KoRect (x, y, r.width (), r.height ());
  }
  else*/
  return r;
}

KoRect Canvas::snapScaledBoxToGrid(const KoRect &r, int hmask)
{
/*  double x1, x2, y1, y2;

  if (mGDoc->snapToHelplines() || mGDoc->snapToGrid())
   {
    x1 = snapXPositionToGrid (r.left ());
    x2 = snapXPositionToGrid (r.right ());
    y1 = snapYPositionToGrid (r.top ());
    y2 = snapYPositionToGrid (r.bottom ());

    KoRect retval (r);
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
  else*/
  return r;
}

void Canvas::drawTmpHelpline(int x, int y, bool horizH)
{
  if(horizH)
  {
    if(mTmpHorizHelpline >= 0)
      repaint(0, mTmpHorizHelpline, width(), 1);
    if(y >= 0)
    {
      QPainter p(this);
      QPen pen(red);
      p.setPen(pen);
      p.drawLine(0, y, width() - 1, y);
      p.end();
    }
    mTmpHorizHelpline = y;
  }
  else
  {
    if(mTmpVertHelpline >= 0)
      repaint(mTmpVertHelpline, 0, 1, height());
    if(x >= 0)
    {
      QPainter p(this);
      QPen pen(red);
      p.setPen(pen);
      p.drawLine(x, 0, x, height() - 1);
      p.end();
    }
    mTmpVertHelpline = x;
  }
}

void Canvas::addHelpline(int x, int y, bool horizH)
{
/*  double pos = -1;
  tmpHorizHelpline = tmpVertHelpline = -1;
  // convert into mGDoc coordinates
  // and add helpline
  if(horizH)
  {
    pos = double(y - m_relativePaperArea.top()) / zoomFactor();
    mGDoc->addHorizHelpline (pos);
  }
  else
  {
    pos = double(x - m_relativePaperArea.left()) / zoomFactor();
    mGDoc->addVertHelpline (pos);
  }*/
}

bool Canvas::eventFilter(QObject *o, QEvent *e)
{
  if(e->type() == QEvent::KeyPress)
  {
    QKeyEvent *ke = (QKeyEvent *)e;
    if(ke->key() == Key_Tab)
/*      {
         if (toolController->getActiveTool ()->id()==Tool::ToolSelect)
            ((SelectionTool *)
             toolController->getActiveTool ())->processTabKeyEvent (mGDoc,this);
      }
      else if(toolController->getActiveTool ()->id()!=Tool::ToolText
              && ke->key()== Key_Backspace)
          emit backSpaceCalled();
      else if(toolController->getActiveTool()->id()!=Tool::ToolText
              && ke->key()== Key_Delete)
          emit backSpaceCalled();
      else*/
    keyPressEvent(ke);
    return true;
  }
  return QWidget::eventFilter(o, e);
}

void Canvas::resizeEvent(QResizeEvent *)
{
  buffer->resize(size());
  hBar->setPageStep(width());
  vBar->setPageStep(height());
  
  mXOffset = width() / 2 - mWidthH + mXOffset;
  mYOffset = height() / 2 - mHeightH + mYOffset;
  
  mXCenter = (width() - mGDoc->xCanvas()) / 2;
  mYCenter = (height() - mGDoc->yCanvas()) / 2;
  hBar->setValue(mXCenter - mXOffset);
  vBar->setValue(mYCenter - mYOffset);
  mWidthH = width() / 2;
  mHeightH = height() / 2;
}

void Canvas::paintEvent(QPaintEvent *e)
{
  const QRect &rect = e->rect();
  updateBuf(rect);
  bitBlt(this, rect.x(), rect.y(), buffer, rect.x(), rect.y(), rect.width(), rect.height());
}

void Canvas::wheelEvent(QWheelEvent *e)
{
  e->accept();
  if(e->delta() < 0)
    vBar->addLine();
  else
    vBar->subtractLine();
}

void Canvas::mousePressEvent(QMouseEvent *e)
{
  propagateMouseEvent(e);
}

void Canvas::mouseReleaseEvent(QMouseEvent *e)
{
  propagateMouseEvent(e);
}

void Canvas::mouseMoveEvent(QMouseEvent *e)
{
  propagateMouseEvent(e);
}

void Canvas::keyPressEvent(QKeyEvent* e)
{
  if(mView->toolController())
    mView->toolController()->delegateEvent(e);
}

void Canvas::changePage()
{
  updateScrollBars();
  mXCenter = (width() - mGDoc->xCanvas()) / 2;
  mYCenter = (height() - mGDoc->yCanvas()) / 2;
  center();
  repaint();
}

void Canvas::changeZoomFactor(double scale)
{
  mXOffset = mWidthH - static_cast<int>(scale * (mWidthH - mXOffset));
  mYOffset = mHeightH - static_cast<int>(scale * (mHeightH - mYOffset));
  repaint();
}

void Canvas::updateScrollBars()
{
  hBar->setRange(-mGDoc->xCanvas(), mGDoc->xCanvas());
  vBar->setRange(-mGDoc->yCanvas(), mGDoc->yCanvas());
  hBar->setPageStep(width());
  vBar->setPageStep(height());
}

void Canvas::scrollX(int v)
{
  mXOffset = mXCenter - v;
  emit offsetXChanged(mXOffset);
  repaint();
}

void Canvas::scrollY(int v)
{
  mYOffset = mYCenter - v;
  emit offsetYChanged(mYOffset);
  repaint();
}

void Canvas::updateRegion(const KoRect &r)
{
  int x = static_cast<int>(r.x() + mXOffset);
  int y = static_cast<int>(r.y() + mYOffset);
  int w = static_cast<int>(r.width() * zoomFactor());
  int h = static_cast<int>(r.height() * zoomFactor());
  kdDebug(38000) << "update: x=" << x << " y=" << y << " w=" << w << " h=" << h <<endl;
  QRect rr(x, y, w, h);
  updateBuf(rr);
  repaint(rr);
}

void Canvas::propagateMouseEvent(QMouseEvent *e)
{
  emit mousePositionChanged(e->x(), e->y());

  if(e->button() == RightButton && e->type() == QEvent::MouseButtonPress)
  {
    if(mGDoc->activePage()->selectionIsEmpty())
    {
/*      GObject *obj = mGDoc->activePage()->findContainingObject(new_pos.x(), new_pos.y());
      if(obj)
        emit rightButtonAtObjectClicked(e->x(), e->y(), obj);
      else */
        emit rightButtonClicked(e->x(), e->y());
    }
    else
    {
      /* pop up menu for the current selection */
      emit rightButtonAtSelectionClicked(e->x(), e->y());
    }
    return;
  }
  if(mView->toolController())
    /* the tool controller processes the event */
    mView->toolController()->delegateEvent(e);
}

void Canvas::drawGrid(QPainter &p, const QRect &rect)
{
  if(!mGDoc->document()->isReadWrite())
    return;

  p.save();
  QPen pen(mGDoc->gridColor());
  p.setPen(pen);

  double hd = mGDoc->xGridZ();
  double h = mXOffset + hd * static_cast<int>((rect.x() - mXOffset) / hd);
  for(; h <= rect.right(); h += hd)
  {
    int hi = qRound(h);
    p.drawLine(hi, rect.top(), hi, rect.bottom());
  }

  double vd = mGDoc->yGridZ();
  double v = mYOffset + vd * static_cast<int>((rect.y() - mYOffset) / vd);
  for(; v <= rect.bottom(); v += vd)
  {
    int vi = qRound(v);
    p.drawLine(rect.left(), vi, rect.right(), vi);
  }

  p.restore();
}

void Canvas::drawHelplines(QPainter &p, const QRect &rect)
{
  if(!mGDoc->document()->isReadWrite())
    return;

  p.save();
  QPen pen(blue);
  p.setPen(pen);

  QValueList<double>::Iterator i;

  for(i = mGDoc->horizHelplines().begin(); i != mGDoc->horizHelplines().end(); ++i)
  {
    int hi = qRound(*i * zoomFactor()) + mYOffset;
    p.drawLine(rect.left(), hi, rect.right(), hi);
  }

  for(i = mGDoc->vertHelplines().begin(); i != mGDoc->vertHelplines().end(); ++i)
  {
    int vi = qRound(*i * zoomFactor()) + mXOffset;
    p.drawLine(vi, rect.top(), vi, rect.bottom());
  }

  p.restore ();
}

double Canvas::snapXPositionToGrid(double pos)
{
/*  bool snap = false;

  if (mGDoc->snapToHelplines())
  {
    // try to snap to help lines
    QValueList<double>::Iterator i;
    for (i = mGDoc->vertHelplines().begin (); i != mGDoc->vertHelplines().end (); ++i)
    {
      if (fabs (*i - pos) <= 10.0) {
        pos = *i;
        snap = true;
        break;
      }
    }
  }
  if (mGDoc->snapToGrid() && ! snap) {
    int n = (int) (pos / mGDoc->xGrid());
    double r = fmod (pos, mGDoc->xGrid());
    if (r > (mGDoc->xGrid() / 2.0))
      n++;
    pos = mGDoc->xGrid() * n;
  }*/
  return pos;
}

double Canvas::snapYPositionToGrid(double pos)
{
/*  bool snap = false;

  if (mGDoc->snapToHelplines())
  {
    // try to snap to help lines
    QValueList<double>::Iterator i;
    for (i = mGDoc->horizHelplines().begin (); i != mGDoc->horizHelplines().end (); ++i) {
      if (fabs (*i - pos) <= 10.0) {
        pos = *i;
        snap = true;
        break;
      }
    }
  }
  if (mGDoc->snapToGrid() && ! snap)
  {
    int n = (int) (pos / mGDoc->yGrid());
    double r = fmod (pos, mGDoc->yGrid());
    if (r > (mGDoc->yGrid() / 2.0))
      n++;
    pos = mGDoc->yGrid() * n;
  }*/
  return pos;
}

#include "Canvas.moc"
