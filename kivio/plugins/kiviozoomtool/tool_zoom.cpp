/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2003 theKompany.com & Dave Marotti,
 *                         Peter Simonsson
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "tool_zoom.h"
#include "kivio_view.h"
#include "kivio_page.h"
#include "kivio_canvas.h"
#include "kivio_factory.h"

#include <kaction.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <ktoolbar.h>
#include <KoMainWindow.h>
#include <kdebug.h>
#include <KoZoomHandler.h>
#include <KoPoint.h>
#include <kstdaction.h>
#include <KoZoomAction.h>

#include <qapplication.h>
#include <qcursor.h>

ZoomTool::ZoomTool(KivioView* parent) : Kivio::MouseTool(parent, "Zoom Mouse Tool")
{
  m_zoomAction = new KRadioAction(i18n("&Zoom"), "viewmag", CTRL + ALT + Qt::Key_Z, actionCollection(), "zoom");
  m_zoomAction->setWhatsThis(i18n("By pressing this button you can zoom in on a specific area."));
  m_panAction = new KRadioAction(i18n("&Pan Document"), "kivio_zoom_hand", CTRL + ALT + Qt::Key_H, actionCollection(), "pan");
  m_panAction->setWhatsThis(i18n("You can drag the document by using the mouse."));
  m_zoomAction->setExclusiveGroup("zoomAction");
  m_panAction->setExclusiveGroup("zoomAction");
  connect(m_zoomAction,SIGNAL(activated()),SLOT(zoomActivated()));
  connect(m_panAction,SIGNAL(activated()),SLOT(handActivated()));
  connect(m_zoomAction, SIGNAL(toggled(bool)), this, SLOT(setActivated(bool)));
  connect(m_panAction, SIGNAL(toggled(bool)), this, SLOT(setActivated(bool)));

  KoZoomAction* viewZoom = new KoZoomAction(i18n("Zoom &Level"), "viewmag", 0, actionCollection(), "viewZoom" );
  viewZoom->setWhatsThis(i18n("This allows you to zoom in or out of a document. You can either choose one of the predefined zoomfactors or enter a new zoomfactor (in percent)."));
  connect(viewZoom, SIGNAL(zoomChanged(const QString&)), parent, SLOT(viewZoom(const QString&)));
  connect(parent, SIGNAL(zoomChanged(int)), viewZoom, SLOT(setZoom(int)));
  
  m_pPlus = KStdAction::zoomIn(this, SLOT(zoomPlus()), actionCollection(), "zoomPlus");
  m_pPlus->setWhatsThis(i18n("You can zoom in on the document by pressing this button."));

  m_pMinus = KStdAction::zoomOut(this, SLOT(zoomMinus()), actionCollection(), "zoomMinus");
  m_pMinus->setWhatsThis(i18n("By pressing this button you can zoom out of the document."));

  m_pZoomWidth = new KAction( i18n("Zoom Width"), "kivio_zoom_width", Qt::SHIFT+Qt::Key_F4, actionCollection(), "zoomWidth" );
  m_pZoomWidth->setWhatsThis(i18n("You can zoom the document that it fits into the window width."));
  connect(m_pZoomWidth,SIGNAL(activated()),SLOT(zoomWidth()));

  m_pZoomHeight = new KAction( i18n("Zoom Height"), "kivio_zoom_height", Qt::SHIFT+Qt::Key_F5, actionCollection(), "zoomHeight" );
  m_pZoomHeight->setWhatsThis(i18n("You can zoom the document that it fits into the window height."));
  connect(m_pZoomHeight,SIGNAL(activated()),SLOT(zoomHeight()));

  m_pZoomPage = new KAction( i18n("Zoom Page"), "kivio_zoom_page", Qt::SHIFT+Qt::Key_F6, actionCollection(), "zoomPage" );
  m_pZoomPage->setWhatsThis(i18n("The Zoom Page button shows the entire page."));
  connect(m_pZoomPage,SIGNAL(activated()),SLOT(zoomPage()));

  m_pZoomSelected = new KAction( i18n("Zoom Selected"), "kivio_zoom_selected", CTRL+Qt::Key_Y, actionCollection(), "zoomSelected" );
  m_pZoomSelected->setWhatsThis(i18n("By pressing this button you zoom in on the document, so that all <b>selected</b> objects are visible."));
  connect(m_pZoomSelected,SIGNAL(activated()),SLOT(zoomSelected()));

  m_pZoomAllObjects = new KAction( i18n("Zoom All Objects"), "kivio_zoom_allobject", 0, actionCollection(), "zoomAllObjects" );
  m_pZoomAllObjects->setWhatsThis(i18n("You are able to zoom in on the document, so that all objects are visible by pressing this button."));
  connect(m_pZoomAllObjects,SIGNAL(activated()),SLOT(zoomAllobjects()));

  QPixmap pix;

  pix = BarIcon("kivio_zoom_plus",KivioFactory::global());
  m_pPlusCursor = new QCursor(pix,pix.width()/2,pix.height()/2);

  pix = BarIcon("kivio_zoom_minus",KivioFactory::global());
  m_pMinusCursor = new QCursor(pix,pix.width()/2,pix.height()/2);

  pix = BarIcon("kivio_zoom_hand",KivioFactory::global());
  m_handCursor = new QCursor(pix,pix.width()/2,pix.height()/2);

  m_pMenu = 0;
}

ZoomTool::~ZoomTool()
{
  delete m_pPlusCursor;
  delete m_pMinusCursor;
  delete m_handCursor;
}

bool ZoomTool::processEvent(QEvent* e)
{
  KivioCanvas* canvas = view()->canvasWidget();

  if(!m_bHandMode) {
    switch(e->type()) {
      case QEvent::KeyPress:
        if (!m_bLockKeyboard && (static_cast<QKeyEvent*>(e)->key() == Qt::Key_Shift)) {
          m_pCurrent = m_pMinus;
          canvas->setCursor(*m_pMinusCursor);
          return true;
        }
        break;
      case QEvent::KeyRelease:
        if (!m_bLockKeyboard && (static_cast<QKeyEvent*>(e)->key() == Qt::Key_Shift)) {
          m_pCurrent = m_pPlus;
          canvas->setCursor(*m_pPlusCursor);
          return true;
        }
        break;
      case QEvent::MouseButtonPress:
      {
        QMouseEvent* me = static_cast<QMouseEvent*>(e);
        
        if(me->button() == LeftButton) {
          if(m_pCurrent == m_pMinus) {
            m_pCurrent->activate();
          } else {
            m_bLockKeyboard = true;
            m_bDrawRubber = true;
            canvas->startRectDraw(me->pos(), KivioCanvas::Rubber);
          }
        } else {
          showPopupMenu(me->globalPos());
        }
        
        return true;
        break;
      }
      case QEvent::MouseButtonRelease:
        if(m_pCurrent == m_pPlus && m_bDrawRubber) {
          canvas->endRectDraw();
          m_bDrawRubber = false;
          m_bLockKeyboard = false;
          zoomRect(canvas->rect());
          return true;
        }
        break;
      case QEvent::MouseMove:
        if (m_bDrawRubber) {
          canvas->continueRectDraw(static_cast<QMouseEvent*>(e)->pos(), KivioCanvas::Rubber);
          return true;
        }
        break;
      default:
        break;
    }
  } else {
    switch(e->type()) {
      case QEvent::MouseButtonPress:
        isHandMousePressed = true;
        mousePos = static_cast<QMouseEvent*>(e)->pos();
        return true;
        break;
      case QEvent::MouseButtonRelease:
        isHandMousePressed = false;
        return true;
        break;
      case QEvent::MouseMove:
        if (isHandMousePressed) {
          canvas->setUpdatesEnabled(false);
          QPoint newPos = static_cast<QMouseEvent*>(e)->pos();
          mousePos -= newPos;
          canvas->scrollDx(-mousePos.x());
          canvas->scrollDy(-mousePos.y());
          mousePos = newPos;
          canvas->setUpdatesEnabled(true);
          return true;
        }
        break;
      default:
        break;
    }
  }

  return false;
}

void ZoomTool::setActivated(bool a)
{
  if(a) {
    m_pCurrent = m_pPlus;
    emit activated(this);
  } else {
    kDebug(43000) << "ZoomTool DeActivate" << endl;
  
    m_pCurrent = 0L;
  
    m_zoomAction->setChecked(false);
    m_panAction->setChecked(false);
    view()->setStatusBarInfo("");
  
    if (!view()->canvasWidget()->isUpdatesEnabled()) {
      view()->canvasWidget()->setUpdatesEnabled(true);
    }
  }
}

void ZoomTool::zoomActivated()
{
  view()->canvasWidget()->setCursor(*m_pPlusCursor);
  m_bHandMode = false;
  m_bDrawRubber = false;
  m_bLockKeyboard = false;
  m_zoomAction->setChecked(true);
  m_panAction->setChecked(false);
  view()->setStatusBarInfo(i18n("Hold Shift to zoom out."));
}

void ZoomTool::handActivated()
{
  view()->canvasWidget()->setCursor(*m_handCursor);
  m_bHandMode = true;
  isHandMousePressed = false;
  m_zoomAction->setChecked(false);
  m_panAction->setChecked(true);
}

void ZoomTool::zoomPlus()
{
  KivioCanvas* canvas = view()->canvasWidget();
  canvas->zoomIn(QPoint(canvas->width()/2, canvas->height()/2));
  if(view()->zoomHandler()->zoom() >= 2000)
  {
    m_pPlus->setEnabled(false);
    m_pMinus->setEnabled(true);
  }
  else
  {
    m_pPlus->setEnabled(true);
    m_pMinus->setEnabled(true);
  }
}

void ZoomTool::zoomMinus()
{
  KivioCanvas* canvas = view()->canvasWidget();
  canvas->zoomOut(QPoint(canvas->width()/2, canvas->height()/2));
  if(view()->zoomHandler()->zoom() <= 25)
  {
    m_pMinus->setEnabled(false);
    m_pPlus->setEnabled(true);
  }
  else
  {
    m_pMinus->setEnabled(true);
    m_pPlus->setEnabled(true);
  }
}

void ZoomTool::zoomWidth()
{
  KivioCanvas* canvas = view()->canvasWidget();
  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, KoGlobal::dpiX(),
    KoGlobal::dpiY());
  int cw = qMax(10,canvas->width()-20);
  KoPageLayout pl = canvas->activePage()->paperLayout();
  float w = zoom.zoomItX(pl.ptWidth);
  float z = cw/w;

  canvas->setUpdatesEnabled(false);
  view()->viewZoom(qRound(z * 100));
  canvas->setUpdatesEnabled(true);
}

void ZoomTool::zoomHeight()
{
  KivioCanvas* canvas = view()->canvasWidget();
  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, KoGlobal::dpiX(),
    KoGlobal::dpiY());
  int ch = qMax(10,canvas->height()-20);
  KoPageLayout pl = canvas->activePage()->paperLayout();
  float h = zoom.zoomItY(pl.ptHeight);
  float zh = ch/h;

  canvas->setUpdatesEnabled(false);
  view()->viewZoom(qRound(zh * 100));
  canvas->setUpdatesEnabled(true);
}

void ZoomTool::zoomPage()
{
  KivioCanvas* canvas = view()->canvasWidget();
  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, KoGlobal::dpiX(),
    KoGlobal::dpiY());
  int cw = qMax(10,canvas->width()-20);
  int ch = qMax(10,canvas->height()-20);

  KoPageLayout pl = canvas->activePage()->paperLayout();
  float w = zoom.zoomItX(pl.ptWidth);
  float h = zoom.zoomItY(pl.ptHeight);

  float z = qMin(cw/w,ch/h);

  canvas->setUpdatesEnabled(false);
  view()->viewZoom(qRound(z * 100));
  canvas->setUpdatesEnabled(true);
}

void ZoomTool::showPopupMenu(const QPoint& p )
{
  if(!m_pMenu) {
    m_pMenu = static_cast<KMenu*>(factory()->container("ZoomPopup", this));
  }
  
  if(m_pMenu) {
    m_pMenu->popup(p);
  } else {
    kDebug(43000) << "What no popup! *ARGH*!" << endl;
  }
}

void ZoomTool::zoomSelected()
{
  KivioCanvas* canvas = view()->canvasWidget();
  KoRect r = canvas->activePage()->getRectForAllSelectedStencils();

  if (!r.isNull() && r.isValid()) {
    canvas->setVisibleArea(r);
  }
}

void ZoomTool::zoomAllobjects()
{
  KivioCanvas* canvas = view()->canvasWidget();
  KoRect r = canvas->activePage()->getRectForAllStencils();

  if (!r.isNull() && r.isValid()) {
    canvas->setVisibleArea(r);
  }
}

void ZoomTool::zoomRect(QRect r)
{
  KivioCanvas* canvas = view()->canvasWidget();
  
  if (r.isEmpty()) {
    canvas->zoomIn(r.topLeft());
    return;
  }

  KoPoint p0 = canvas->mapFromScreen(r.topLeft());
  canvas->setVisibleArea(KoRect(p0.x(), p0.y(), view()->zoomHandler()
    ->unzoomItX(r.width()), view()->zoomHandler()->unzoomItY(r.height())));
}
#include "tool_zoom.moc"
