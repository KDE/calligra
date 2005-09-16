/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 * Copyright (C) 2005 Peter Simonsson <psn@linux.se>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kivio_birdeye_panel.h"

#include "kivio_screen_painter.h"

#include "kivio_view.h"
#include "kivio_doc.h"
#include "kivio_page.h"
#include "kivio_canvas.h"
#include "kivio_common.h"
#include "kivio_factory.h"

#include <ktoolbar.h>
#include <kaction.h>
#include <klocale.h>
#include <koGlobal.h>
#include <kozoomhandler.h>
#include <kdeversion.h>
#include <kiconloader.h>

#include <qpixmap.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qframe.h>
#include <qcursor.h>

#define TOGGLE_ACTION(X) ((KToggleAction*)child(X))

KivioBirdEyePanel::KivioBirdEyePanel(KivioView* view, QWidget* parent, const char* name)
: KivioBirdEyePanelBase(parent, name), m_pView(view), m_pCanvas(view->canvasWidget()), m_pDoc(view->doc())
{
  handlePress = false;
  m_buffer = new QPixmap();
  canvas->installEventFilter(this);
  m_zoomHandler = new KoZoomHandler;
  bar->setIconSize(16);

  connect( m_pDoc, SIGNAL( sig_updateView(KivioPage*)), SLOT(slotUpdateView(KivioPage*)) );
  connect( m_pView, SIGNAL(zoomChanged(int)), SLOT(canvasZoomChanged()));
  connect( m_pCanvas, SIGNAL(visibleAreaChanged()), SLOT(updateVisibleArea()));

  zoomIn = new KAction( i18n("Zoom In"), "kivio_zoom_plus", 0, this, SLOT(zoomPlus()), this, "zoomIn" );
  zoomOut = new KAction( i18n("Zoom Out"), "kivio_zoom_minus", 0, this, SLOT(zoomMinus()), this, "zoomOut" );

  zoomIn->plug(bar);
  zoomOut->plug(bar);

  canvasZoomChanged();
}

KivioBirdEyePanel::~KivioBirdEyePanel()
{
  delete m_buffer;
  delete m_zoomHandler;
}

void KivioBirdEyePanel::zoomChanged(int z)
{
//  debug(QString("zoomChanged %1 %2").arg(z).arg((double)(z/100.0)));
  m_pView->viewZoom(z);
}

void KivioBirdEyePanel::zoomMinus()
{
  m_pCanvas->zoomOut(QPoint(m_pCanvas->width()/2, m_pCanvas->height()/2));
}

void KivioBirdEyePanel::zoomPlus()
{
  m_pCanvas->zoomIn(QPoint(m_pCanvas->width()/2, m_pCanvas->height()/2));
}

void KivioBirdEyePanel::canvasZoomChanged()
{
  int iz = m_pView->zoomHandler()->zoom();
  slider->blockSignals(true);
  zoomBox->blockSignals(true);

  zoomBox->setValue(iz);
  slider->setMaxValue(QMAX(iz,500));
  slider->setValue(iz);

  zoomBox->blockSignals(false);
  slider->blockSignals(false);

  slotUpdateView(m_pView->activePage());
}

void KivioBirdEyePanel::slotUpdateView(KivioPage* page)
{
  if (!isVisible() || !page || m_pView->activePage() != page)
    return;

  updateView();
}

bool KivioBirdEyePanel::eventFilter(QObject* o, QEvent* ev)
{
  if (o == canvas && ev->type() == QEvent::Show) {
    updateView();
  }

  if (o == canvas && ev->type() == QEvent::Resize) {
    m_buffer->resize(canvas->size());
    slotUpdateView(m_pView->activePage());
  }

  if (o == canvas && ev->type() == QEvent::Paint) {
    updateVisibleArea();
    return true;
  }

  if (o == canvas && ev->type() == QEvent::MouseMove) {
    QMouseEvent* me = (QMouseEvent*)ev;
    if (me->state() == LeftButton)
      handleMouseMoveAction(me->pos());
    else
      handleMouseMove(me->pos());

    lastPos = me->pos();
    return true;
  }

  if (o == canvas && ev->type() == QEvent::MouseButtonPress) {
    QMouseEvent* me = (QMouseEvent*)ev;
    if (me->button() == LeftButton)
      handleMousePress(me->pos());

    return true;
  }

  return KivioBirdEyePanelBase::eventFilter(o, ev);
}

void KivioBirdEyePanel::updateView()
{
  // FIXME: This whole function needs fixing!
  QSize s1 = canvas->size();
  QSize s2;
 
  if (!m_pView || !m_pView->activePage()) return;

  KoPageLayout pl = m_pView->activePage()->paperLayout();

  int pw = m_pView->zoomHandler()->zoomItX(pl.ptWidth);
  int ph = m_pView->zoomHandler()->zoomItY(pl.ptHeight);
  s2 = QSize(pw,ph);

  double zx = (double)s1.width()/(double)s2.width();
  double zy = (double)s1.height()/(double)s2.height();
  double zxy = QMIN(zx,zy);

  m_zoomHandler->setZoomAndResolution(qRound(zxy * 100), KoGlobal::dpiX(),
    KoGlobal::dpiY());

  pw = m_zoomHandler->zoomItX(pl.ptWidth);
  ph = m_zoomHandler->zoomItY(pl.ptHeight);
  int px0 = (s1.width()-pw)/2;
  int py0 = (s1.height()-ph)/2;

  QPoint p0 = QPoint(px0,py0);

  QRect rect(QPoint(0,0),s1);

  QPainter painter(m_buffer);
  painter.fillRect(rect, QColor(120, 120, 120));

  painter.fillRect(px0, py0, pw, ph, white);

  painter.translate(px0, py0);
  m_pDoc->paintContent(painter, rect, false, m_pView->activePage(), p0, m_zoomHandler, false);
  painter.end();

  updateVisibleArea();
}

void KivioBirdEyePanel::updateVisibleArea()
{
  bitBlt(canvas,0,0,m_buffer);

  KoRect vr = m_pCanvas->visibleArea();
  QSize s1 = canvas->size();
  KoPageLayout pl = m_pView->activePage()->paperLayout();
  int pw = m_zoomHandler->zoomItX(pl.ptWidth);
  int ph = m_zoomHandler->zoomItY(pl.ptHeight);
  int px0 = (s1.width()-pw)/2;
  int py0 = (s1.height()-ph)/2;

  int x = m_zoomHandler->zoomItX(vr.x()) + px0;
  int y = m_zoomHandler->zoomItY(vr.y()) + py0;
  int w = m_zoomHandler->zoomItX(vr.width());
  int h = m_zoomHandler->zoomItX(vr.height());

  QPainter painter(canvas,canvas);
  painter.setPen(red);
  painter.drawRect(x, y, w, h);
  painter.setPen(red.light());
  painter.drawRect(x-1, y-1, w+2, h+2);
  painter.end();

  varea.setRect(x,y,w,h);
}

void KivioBirdEyePanel::handleMouseMove(QPoint p)
{
  handlePress = true;

  QRect r1 = QRect(varea.x()-1, varea.y()-1, 3, varea.height()+2);
  if (r1.contains(p)) {
    canvas->setCursor(sizeHorCursor);
    apos = AlignLeft;
    return;
  }

  r1.moveBy(varea.width(),0);
  if (r1.contains(p)) {
    canvas->setCursor(sizeHorCursor);
    apos = AlignRight;
    return;
  }

  QRect r2 = QRect(varea.x()-1, varea.y()-1, varea.width()+2, 3);
  if (r2.contains(p)) {
    canvas->setCursor(sizeVerCursor);
    apos = AlignTop;
    return;
  }

  r2.moveBy(0, varea.height());
  if (r2.contains(p)) {
    canvas->setCursor(sizeVerCursor);
    apos = AlignBottom;
    return;
  }

  if (varea.contains(p)) {
    canvas->setCursor(sizeAllCursor);
    apos = AlignCenter;
    return;
  }

  canvas->setCursor(arrowCursor);
  handlePress = false;
}

void KivioBirdEyePanel::handleMouseMoveAction(QPoint p)
{
  if (!handlePress)
    return;

  p -= lastPos;

  if (apos == AlignCenter) {
    double zy = m_pView->zoomHandler()->zoomedResolutionY() / m_zoomHandler->zoomedResolutionY();
    double zx = m_pView->zoomHandler()->zoomedResolutionX() / m_zoomHandler->zoomedResolutionX();
    m_pCanvas->setUpdatesEnabled(false);
    m_pCanvas->scrollDx(-(int)(p.x()*zx));
    m_pCanvas->scrollDy(-(int)(p.y()*zy));
    m_pCanvas->setUpdatesEnabled(true);
    return;
  }

  double dx = m_zoomHandler->unzoomItX(p.x());
  double dy = m_zoomHandler->unzoomItY(p.y());

  KoRect vr = m_pCanvas->visibleArea();
  if (apos == AlignRight) {
    vr.setWidth(QMAX(10.0, vr.width() + dx));
    m_pCanvas->setVisibleAreaByWidth(vr);
  }
  else if (apos == AlignLeft) {
    vr.setX(vr.x() + dx);
    vr.setWidth(QMAX(10.0, vr.width() - dx));
    m_pCanvas->setVisibleAreaByWidth(vr);
  }
  else if (apos == AlignTop) {
    vr.setY(vr.y() + dy);
    vr.setHeight(QMAX(10.0 ,vr.height() - dy));
    m_pCanvas->setVisibleAreaByHeight(vr);
  }
  else if (apos == AlignBottom) {
    vr.setHeight(QMAX(10.0 ,vr.height() + dy));
    m_pCanvas->setVisibleAreaByHeight(vr);
  }
}

void KivioBirdEyePanel::handleMousePress(QPoint p)
{
  if (handlePress)
    return;

  QSize s1 = canvas->size();
  KoPageLayout pl = m_pView->activePage()->paperLayout();
  int pw = m_zoomHandler->zoomItX(pl.ptWidth);
  int ph = m_zoomHandler->zoomItY(pl.ptHeight);
  int px0 = (s1.width()-pw)/2;
  int py0 = (s1.height()-ph)/2;

  double x = m_zoomHandler->unzoomItX(p.x() - px0);
  double y = m_zoomHandler->unzoomItY(p.y() - py0);

  m_pCanvas->setViewCenterPoint(KoPoint(x,y));
}

#include "kivio_birdeye_panel.moc"
