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

#include <cmath>

#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qscrollbar.h>
#include <qrect.h>

#include <kdebug.h>

#include <koColor.h>
#include <koPainter.h>

#include "kontour_global.h"
#include "kontour_view.h"
#include "kontour_doc.h"
#include "GPage.h"
#include "ToolController.h"

Canvas::Canvas(GDocument *aGDoc, KontourView *aView, QScrollBar *hb, QScrollBar *vb, QWidget *parent, const char *name):
QWidget(parent, name)
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

  mWidthH = width() >> 1;
  mHeightH = height() >> 1;

  mOutlineMode = false;
  mWithBasePoints = false;

  kdDebug(38000) << "Double canvas width=" << 2*mGDoc->xCanvas() << endl;
  kdDebug(38000) << "Double canvas height=" << 2*mGDoc->yCanvas() << endl;
  hBar->setRange(-mGDoc->xCanvas(), mGDoc->xCanvas());
  vBar->setRange(-mGDoc->yCanvas(), mGDoc->yCanvas());

  connect(hBar, SIGNAL(valueChanged(int)), SLOT(scrollX(int)));
  connect(vBar, SIGNAL(valueChanged(int)), SLOT(scrollY(int)));

  connect(mGDoc, SIGNAL(changed(const KoRect &, bool)), this, SLOT(updateRegion(const KoRect &, bool)));

//  connect(mGDoc, SIGNAL (sizeChanged ()), this, SLOT (docSizeChanged()));
  connect(mGDoc, SIGNAL(pageChanged()), this, SLOT(changePage()));
  connect(mGDoc, SIGNAL(zoomFactorChanged(double)), this, SLOT(changeZoomFactor(double)));

//  connect(mGDoc, SIGNAL (handleChanged ()), this, SLOT (repaint ()));
//  connect (document, SIGNAL (gridChanged ()), this, SLOT (updateGridInfos ()));

  painter = new KoPainter(this, width(), height());

  setBackgroundMode(NoBackground);

  QPaintDeviceMetrics pdm(this);
  kdDebug(38000) << "DPI X =" << pdm.logicalDpiX() << endl;
  kdDebug(38000) << "DPI Y =" << pdm.logicalDpiY() << endl;
}

Canvas::~Canvas()
{
  if(painter)
    delete painter;
}

void Canvas::outlineMode(bool flag)
{
  if(mOutlineMode != flag)
  {
    mOutlineMode = flag;
    repaint();
  }
}

void Canvas::withBasePoints(bool flag)
{
  if(mWithBasePoints != flag)
  {
    mWithBasePoints = flag;
  }
}

KoPoint Canvas::point(int x, int y)
{
  double xx = static_cast<double>(x - mXOffset) / zoomFactor();
  double yy = static_cast<double>(y - mYOffset) / zoomFactor();
  return KoPoint(xx, yy);
}

QRect Canvas::onCanvas(const KoRect &rr)
{
  QRect rect;
  int l = static_cast<int>(rr.left() * zoomFactor()) + mXOffset;
  int r = static_cast<int>(rr.right() * zoomFactor()) + mXOffset;
  int t = static_cast<int>(rr.top() * zoomFactor()) + mYOffset;
  int b = static_cast<int>(rr.bottom() * zoomFactor()) + mYOffset;
  rect.setLeft(l);
  rect.setRight(r);
  rect.setTop(t);
  rect.setBottom(b);
  return rect;
}

void Canvas::updateBuf()
{
  updateBuf(rect());
}

void Canvas::updateBuf(const QRect &rect)
{
  int left = rect.left();
  int right = rect.right();
  int top = rect.top();
  int bottom = rect.bottom();
  //int width = rect.width();
  //int height = rect.height();

  // TODO Optimize that
  int w = mGDoc->xCanvas();
  int h = mGDoc->yCanvas();

  bool hll = mXOffset >= left;
  //bool hlr = mXOffset <= right;
  //bool hrl = mXOffset + w >= left;
  bool hrr = mXOffset + w <= right;
  bool vtt = mYOffset >= top;
  //bool vtb = mYOffset <= bottom;
  //bool vbt = mYOffset + h >= top;
  bool vbb = mYOffset + h <= bottom;

/*  if(vtt)
  {
    if(!vtb)
    {
      painter->fillAreaRGB(rect, KoColor::white());
    }
  }*/
  painter->fillAreaRGB(rect, KoColor::white());
  if(hll && hrr && vtt && vbb)
  {
    painter->fillAreaRGB(QRect(mXOffset, mYOffset, w, h) ,KoColor::red());
  }
/*  QPainter p;
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
  p.setPen(Qt::black);*/

  if(mGDoc->showGrid())
    drawGrid(rect);

  QWMatrix m;
  m.translate(mXOffset, mYOffset);

  document()->activePage()->drawContents(painter, m, mWithBasePoints, mOutlineMode);

  if(!document()->activePage()->selectionIsEmpty())
    document()->activePage()->handle().draw(painter, mXOffset, mYOffset, zoomFactor());

  if(mGDoc->showHelplines())
    drawHelplines(rect);
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

void Canvas::zoomToPoint(double /*scale*/, int /*x*/, int /*y*/)
{

}

void Canvas::snapPositionToGrid(double &x, double &y)
{
  x = snapXPositionToGrid(x);
  y = snapXPositionToGrid(y);
}

KoRect Canvas::snapTranslatedBoxToGrid(const KoRect &r)
{
  double x1, x2, y1, y2;
  if(mGDoc->snapToHelplines() || mGDoc->snapToGrid())
  {
    x1 = snapXPositionToGrid(r.left());
    x2 = snapXPositionToGrid(r.right());
    y1 = snapYPositionToGrid(r.top());
    y2 = snapYPositionToGrid(r.bottom());
    double x;
    double y;
    if(fabs(r.left() - x1) < fabs(r.right() - x2))
      x = x1;
    else
      x = x2 - r.width();
    if(fabs(r.top() - y1) < fabs(r.bottom() - y2))
      y = y1;
    else
      y = y2 - r.height();
    return KoRect(x, y, r.width(), r.height());
  }
  else
    return r;
}

KoRect Canvas::snapScaledBoxToGrid(const KoRect &r, int hmask)
{
  double x1, x2, y1, y2;

  if(mGDoc->snapToHelplines() || mGDoc->snapToGrid())
  {
    x1 = snapXPositionToGrid(r.left());
    x2 = snapXPositionToGrid(r.right());
    y1 = snapYPositionToGrid(r.top());
    y2 = snapYPositionToGrid(r.bottom());

    KoRect retval(r);
    if(hmask & Kontour::HPosLeft)
      retval.setLeft(x1);
    if(hmask & Kontour::HPosTop)
      retval.setTop(y1);
    if(hmask & Kontour::HPosRight)
      retval.setRight(x2);
    if(hmask & Kontour::HPosBottom)
      retval.setBottom(y2);
    kdDebug() << "SCALED BOX:" << endl;
    return retval;
  }
  else
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
  double pos;
  if(horizH)
  {
    pos = static_cast<double>(y - mYOffset) / zoomFactor();
    mGDoc->addHorizHelpline(pos);
    updateBuf(QRect(0, y, width(), 1));
    repaint(0, y, width(), 1);
    mTmpHorizHelpline = -1;
  }
  else
  {
    pos = static_cast<double>(x - mXOffset) / zoomFactor();
    mGDoc->addVertHelpline(pos);
    updateBuf(QRect(x, 0, 1, height()));
    repaint(x, 0, 1, height());
    mTmpVertHelpline = -1;
  }
}

bool Canvas::eventFilter(QObject *o, QEvent *e)
{
  if(e->type() == QEvent::KeyPress)
  {
    QKeyEvent *ke = (QKeyEvent *)e;
//    if(ke->key() == Key_Tab)
    keyPressEvent(ke);
    return true;
  }
  return QWidget::eventFilter(o, e);
}

void Canvas::resizeEvent(QResizeEvent *)
{
  painter->resize(size());

  kdDebug(38000) << "Width=" << width() << endl;
  kdDebug(38000) << "Height=" << height() << endl;

  hBar->setPageStep(width());
  vBar->setPageStep(height());

  mXOffset = (width() / 2) - mWidthH + mXOffset;
  mYOffset = (height() / 2) - mHeightH + mYOffset;
  emit offsetXChanged(mXOffset);
  emit offsetYChanged(mYOffset);

  mXCenter = (width() - mGDoc->xCanvas()) / 2;
  mYCenter = (height() - mGDoc->yCanvas()) / 2;
  hBar->setValue(mXCenter - mXOffset);
  vBar->setValue(mYCenter - mYOffset);
  mWidthH = width() / 2;
  mHeightH = height() / 2;
  updateBuf();
  repaint();
}

void Canvas::paintEvent(QPaintEvent *e)
{
  const QRect &rect = e->rect();
  bitBlt((QPaintDevice *)this, rect.x(), rect.y(), painter->image(), rect.x(), rect.y(), rect.width(), rect.height());
//  bitBlt(this, rect.x(), rect.y(), buffer, rect.x(), rect.y(), rect.width(), rect.height());
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
  updateBuf();
  repaint();
}

void Canvas::changeZoomFactor(double scale)
{
  kdDebug(38000) << "Double canvas width=" << 2*mGDoc->xCanvas() << endl;
  kdDebug(38000) << "Double canvas height=" << 2*mGDoc->yCanvas() << endl;
  kdDebug(38000) << "mXOffset(old)=" << mXOffset << endl;
  kdDebug(38000) << "mYOffset(old)=" << mYOffset << endl;
  mXOffset = mWidthH - static_cast<int>(scale * (mWidthH - mXOffset));
  mYOffset = mHeightH - static_cast<int>(scale * (mHeightH - mYOffset));
  kdDebug(38000) << "mXOffset(new)=" << mXOffset << endl;
  kdDebug(38000) << "mYOffset(new)=" << mYOffset << endl;
  hBar->setRange(-mGDoc->xCanvas(), mGDoc->xCanvas());
  vBar->setRange(-mGDoc->yCanvas(), mGDoc->yCanvas());
  mXCenter = (width() - mGDoc->xCanvas()) / 2;
  mYCenter = (height() - mGDoc->yCanvas()) / 2;
  hBar->setValue(mXCenter - mXOffset);
  vBar->setValue(mYCenter - mYOffset);
  updateBuf();
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
  //TODO BitBlt
  updateBuf();
  repaint();
}

void Canvas::scrollY(int v)
{
  mYOffset = mYCenter - v;
  emit offsetYChanged(mYOffset);
  //TODO BitBlt
  updateBuf();
  repaint();
}

void Canvas::updateRegion(const KoRect &r, bool handle)
{
  if(r.isEmpty())
    return;
  // TODO : small handle size bug
  int x = static_cast<int>(r.x() * zoomFactor()) + mXOffset;
  int y = static_cast<int>(r.y() * zoomFactor()) + mYOffset;
  int w = static_cast<int>(r.width() * zoomFactor());
  int h = static_cast<int>(r.height() * zoomFactor());
  if(handle)
  {
    x -= 7;
    y -= 7;
    w += 36;
    h += 36;
  }
//  kdDebug(38000) << "update: x=" << x << " y=" << y << " w=" << w << " h=" << h <<endl;
  QRect rr(x, y, w, h);
  updateBuf(rr);
  repaint(rr);
}

void Canvas::propagateMouseEvent(QMouseEvent *e)
{
  emit mousePositionChanged(e->x(), e->y());
  //TODO Zoom
  emit coordsChanged(static_cast<double>(e->x() - mXOffset) / zoomFactor(), static_cast<double>(e->y() - mYOffset) / zoomFactor());

  if(e->button() == RightButton && e->type() == QEvent::MouseButtonPress)
  {
    if(mGDoc->activePage()->selectionIsEmpty())
    {
/*      GObject *obj = mGDoc->activePage()->findContainingObject(new_pos.x(), new_pos.y());
      if(obj)
        emit rightButtonAtObjectClicked(e->x(), e->y(), obj);
      else */
//        emit rightButtonClicked(e->x(), e->y());
    }
    else
    {
      /* pop up menu for the current selection */
      emit rmbAtSelection(e->x(), e->y());
    }
    return;
  }
  if(mView->toolController())
    /* the tool controller processes the event */
    mView->toolController()->delegateEvent(e);
}

void Canvas::drawGrid(const QRect &rect)
{
  if(!mGDoc->document()->isReadWrite())
    return;
  double hd = mGDoc->xGridZ();
  double h = mXOffset + hd * static_cast<int>((rect.x() - mXOffset) / hd);
  if(h < 0.0)
    h += hd;
  for(; h <= rect.right(); h += hd)
  {
    int hi = qRound(h);
    painter->drawVertLineRGB(hi, rect.top(), rect.bottom(), mGDoc->gridColor());
  }

  double vd = mGDoc->yGridZ();
  double v = mYOffset + vd * static_cast<int>((rect.y() - mYOffset) / vd);
  if(v < 0.0)
    v += vd;
  for(; v <= rect.bottom(); v += vd)
  {
    int vi = qRound(v);
    painter->drawHorizLineRGB(rect.left(), rect.right(), vi, mGDoc->gridColor());
  }
}

void Canvas::drawHelplines(const QRect &rect)
{
  if(!mGDoc->document()->isReadWrite())
    return;
  QValueList<double>::Iterator i;
  for(i = mGDoc->horizHelplines().begin(); i != mGDoc->horizHelplines().end(); ++i)
  {
    int vi = qRound(*i * zoomFactor()) + mYOffset;
    if(vi >= rect.top() && vi <= rect.bottom())
      painter->drawHorizLineRGB(rect.left(), rect.right(), vi, Qt::blue);
  }

  for(i = mGDoc->vertHelplines().begin(); i != mGDoc->vertHelplines().end(); ++i)
  {
    int hi = qRound(*i * zoomFactor()) + mXOffset;
    if(hi >= rect.left() && hi <= rect.right())
      painter->drawVertLineRGB(hi, rect.top(), rect.bottom(), Qt::blue);
  }
}

double Canvas::snapXPositionToGrid(double pos)
{
  if(mGDoc->snapToHelplines())
  {
    // try to snap to helplines
    QValueList<double>::Iterator i;
    // TODO Snap distance isnt constant
    for(i = mGDoc->vertHelplines().begin(); i != mGDoc->vertHelplines().end(); ++i)
      if(fabs(*i - pos) <= Kontour::snapDistance)
        return *i;
  }
  if(mGDoc->snapToGrid())
  {
    int n = static_cast<int>(pos / mGDoc->xGrid());
    double r = fmod(pos, mGDoc->xGrid());
    if(r > mGDoc->xGrid() / 2.0)
      n++;
    return mGDoc->xGrid() * n;
  }
  return pos;
}

double Canvas::snapYPositionToGrid(double pos)
{
  if(mGDoc->snapToHelplines())
  {
    // try to snap to helplines
    QValueList<double>::Iterator i;
    for(i = mGDoc->horizHelplines().begin(); i != mGDoc->horizHelplines().end(); ++i)
      if(fabs(*i - pos) <= Kontour::snapDistance)
        return *i;
  }
  if(mGDoc->snapToGrid())
  {
    int n = static_cast<int>(pos / mGDoc->yGrid());
    double r = fmod(pos, mGDoc->yGrid());
    if(r > mGDoc->yGrid() / 2.0)
      n++;
    return mGDoc->yGrid() * n;
  }
  return pos;
}

#include "Canvas.moc"
