/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "tool_zoom.h"
#include "kivio_view.h"
#include "kivio_page.h"
#include "kivio_canvas.h"
#include "kivio_factory.h"

#include <kaction.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <koMainWindow.h>
#include <kdebug.h>
#include <kozoomhandler.h>
#include <koPoint.h>

#include <qapplication.h>
#include <qcursor.h>

ZoomTool::ZoomTool(KivioView* view)
:Tool(view,"Zoom")
{
  setSortNum(1);
  m_pToolBar = 0L;

  m_z1 = new KRadioAction( i18n("Zoom"), "kivio_zoom", CTRL + ALT + Key_Z, actionCollection(), "zoom" );
  m_z1->setWhatsThis(i18n("By pressing this button you can zoom in on a specific area."));
  m_z2 = new KRadioAction( i18n("Hand"), "kivio_zoom_hand", CTRL + ALT + Key_H, actionCollection(), "zoomHand" );
  m_z2->setWhatsThis(i18n("You can drag the document by using the mouse."));
  m_z1->setExclusiveGroup("zoomAction");
  m_z2->setExclusiveGroup("zoomAction");
  connect(m_z1,SIGNAL(activated()),SLOT(zoomActivated()));
  connect(m_z2,SIGNAL(activated()),SLOT(handActivated()));

  m_pPlus = new KAction( i18n("Zoom Plus"), "kivio_zoom_plus", SHIFT+Key_F2, actionCollection(), "zoomPlus" );
  m_pPlus->setWhatsThis(i18n("You can zoom in on the document by pressing this button."));
  connect(m_pPlus,SIGNAL(activated()),SLOT(zoomPlus()));

  m_pMinus = new KAction( i18n("Zoom Minus"), "kivio_zoom_minus", SHIFT+Key_F3, actionCollection(), "zoomMinus" );
  m_pMinus->setWhatsThis(i18n("By pressing this button you can zoom out of the document."));
  connect(m_pMinus,SIGNAL(activated()),SLOT(zoomMinus()));

  m_pZoomWidth = new KAction( i18n("Zoom Width"), "kivio_zoom_width", SHIFT+Key_F4, actionCollection(), "zoomWidth" );
  m_pZoomWidth->setWhatsThis(i18n("You can zoom the document that it fits into the window width."));
  connect(m_pZoomWidth,SIGNAL(activated()),SLOT(zoomWidth()));

  m_pZoomHeight = new KAction( i18n("Zoom Height"), "kivio_zoom_height", SHIFT+Key_F5, actionCollection(), "zoomHeight" );
  m_pZoomHeight->setWhatsThis(i18n("You can zoom the document that it fits into the window height."));
  connect(m_pZoomHeight,SIGNAL(activated()),SLOT(zoomHeight()));

  m_pZoomPage = new KAction( i18n("Zoom Page"), "kivio_zoom_page", SHIFT+Key_F6, actionCollection(), "zoomPage" );
  m_pZoomPage->setWhatsThis(i18n("The Zoom Page button shows the entire page."));
  connect(m_pZoomPage,SIGNAL(activated()),SLOT(zoomPage()));

  m_pZoomSelected = new KAction( i18n("Zoom Selected"), "kivio_zoom_selected", CTRL+Key_Y, actionCollection(), "zoomSelected" );
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

  ToolSelectAction* zoom = new ToolSelectAction( actionCollection(), "ToolAction" );
  zoom->insert(m_z1);
  zoom->insert(m_z2);

  m_pMenu = new KActionMenu(i18n("Zoom Menu"), this, "zoomToolMenu");
  buildMenu();
}

ZoomTool::~ZoomTool()
{
  delete m_pPlusCursor;
  delete m_pMinusCursor;
  delete m_handCursor;
}

void ZoomTool::processEvent(QEvent* e)
{
  if ( !m_bHandMode )
    switch (e->type()) {
      case QEvent::KeyPress:
        if (!m_bLockKeyboard) {
          m_pCurrent = m_pMinus;
          m_pCanvas->setCursor(*m_pMinusCursor);
        }
        break;
      case QEvent::KeyRelease:
        if (!m_bLockKeyboard) {
          m_pCurrent = m_pPlus;
          m_pCanvas->setCursor(*m_pPlusCursor);
        }
        break;
      case QEvent::MouseButtonPress:
        if ( m_pCurrent == m_pMinus ) {
          m_pCurrent->activate();
        } else {
          if( ((QMouseEvent*)e)->button()==RightButton )
          {
            showPopupMenu( ((QMouseEvent*)e)->globalPos() );
          }
          else
          {
            m_bLockKeyboard = true;
            m_bDrawRubber = true;
            m_pCanvas->startRectDraw( ((QMouseEvent*)e)->pos(), KivioCanvas::Rubber );
          }
        }
        break;
      case QEvent::MouseButtonRelease:
        if(m_pCurrent != m_pMinus) {
          m_pCanvas->endRectDraw();
          m_bDrawRubber = false;
          m_bLockKeyboard = false;
          zoomRect(m_pCanvas->rect());
        }
        break;
      case QEvent::MouseMove:
        if (m_bDrawRubber)
          m_pCanvas->continueRectDraw( ((QMouseEvent*)e)->pos(), KivioCanvas::Rubber );
        break;
      default:
        break;
    }
  else
    switch (e->type()) {
      case QEvent::MouseButtonPress:
        isHandMousePressed = true;
        mousePos = ((QMouseEvent*)e)->pos();
        break;
      case QEvent::MouseButtonRelease:
        isHandMousePressed = false;
        break;
      case QEvent::MouseMove:
        if (isHandMousePressed) {
          m_pCanvas->setUpdatesEnabled(false);
          QPoint newPos = ((QMouseEvent*)e)->pos();
          mousePos -= newPos;
          m_pCanvas->scrollDx(-mousePos.x());
          m_pCanvas->scrollDy(-mousePos.y());
          mousePos = newPos;
          m_pCanvas->setUpdatesEnabled(true);
        }
        break;
      default:
        break;
    }
}

void ZoomTool::activateGUI(KXMLGUIFactory* /*factory*/)
{
}

void ZoomTool::deactivateGUI( KXMLGUIFactory* )
{
}

void ZoomTool::activate()
{
  m_pCurrent = m_pPlus;
}

void ZoomTool::deactivate()
{
  kdDebug() << "ZoomTool DeActivate" << endl;

  m_pCurrent = 0L;

  m_z1->setChecked(false);
  m_z2->setChecked(false);

  if (!m_pCanvas->isUpdatesEnabled()) {
    m_pCanvas->setUpdatesEnabled(true);
  }
}

void ZoomTool::configure()
{
}

void ZoomTool::zoomActivated()
{
  setOverride();
  m_pCanvas->setCursor(*m_pPlusCursor);
  m_bHandMode = false;
  m_bDrawRubber = false;
  m_bLockKeyboard = false;
}

void ZoomTool::handActivated()
{
  setOverride();
  m_pCanvas->setCursor(*m_handCursor);
  m_bHandMode = true;
  isHandMousePressed = false;
}

void ZoomTool::zoomPlus()
{
   setOverride();
   m_pCanvas->zoomIn(QPoint(m_pCanvas->width()/2, m_pCanvas->height()/2));
   if(m_pView->zoomHandler()->zoom() >= 2000)
   {
      m_pPlus->setEnabled(false);
      m_pMinus->setEnabled(true);
   }
   else
   {
      m_pPlus->setEnabled(true);
      m_pMinus->setEnabled(true);
   }
   removeOverride();
}

void ZoomTool::zoomMinus()
{
   setOverride();
   m_pCanvas->zoomOut(QPoint(m_pCanvas->width()/2, m_pCanvas->height()/2));
   if(m_pView->zoomHandler()->zoom() <= 25)
   {
      m_pMinus->setEnabled(false);
      m_pPlus->setEnabled(true);
   }
   else
   {
      m_pMinus->setEnabled(true);
      m_pPlus->setEnabled(true);
   }
   removeOverride();
}

void ZoomTool::zoomWidth()
{
  setOverride();

  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, QPaintDevice::x11AppDpiX(),
    QPaintDevice::x11AppDpiY());
  int cw = QMAX(10,m_pCanvas->width()-20);
  KoPageLayout pl = m_pCanvas->activePage()->paperLayout();
  float w = zoom.zoomItX(pl.ptWidth);
  float z = cw/w;

  m_pCanvas->setUpdatesEnabled(false);
  m_pCanvas->setZoom(qRound(z * 100));
  m_pCanvas->setUpdatesEnabled(true);

  removeOverride();
}

void ZoomTool::zoomHeight()
{
  setOverride();

  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, QPaintDevice::x11AppDpiX(),
    QPaintDevice::x11AppDpiY());
  int ch = QMAX(10,m_pCanvas->height()-20);
  KoPageLayout pl = m_pCanvas->activePage()->paperLayout();
  float h = zoom.zoomItY(pl.ptHeight);
  float zh = ch/h;

  m_pCanvas->setUpdatesEnabled(false);
  m_pCanvas->setZoom(qRound(zh * 100));
  m_pCanvas->setUpdatesEnabled(true);

  removeOverride();
}

void ZoomTool::zoomPage()
{
  setOverride();

  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, QPaintDevice::x11AppDpiX(),
    QPaintDevice::x11AppDpiY());
  int cw = QMAX(10,m_pCanvas->width()-20);
  int ch = QMAX(10,m_pCanvas->height()-20);

  KoPageLayout pl = m_pCanvas->activePage()->paperLayout();
  float w = zoom.zoomItX(pl.ptWidth);
  float h = zoom.zoomItY(pl.ptHeight);

  float z = QMIN(cw/w,ch/h);

  m_pCanvas->setUpdatesEnabled(false);
  m_pCanvas->setZoom(qRound(z * 100));
  m_pCanvas->setUpdatesEnabled(true);

  removeOverride();
}

void ZoomTool::buildMenu()
{
  m_pMenu->insert( m_pPlus );
  m_pMenu->insert( m_pMinus );
  m_pMenu->popupMenu()->insertSeparator();

  m_pMenu->insert( m_pZoomWidth );
  m_pMenu->insert( m_pZoomHeight );
  m_pMenu->insert( m_pZoomPage );
  m_pMenu->popupMenu()->insertSeparator();

  m_pMenu->insert( m_pZoomSelected );
  m_pMenu->insert( m_pZoomAllObjects );
}

void ZoomTool::showPopupMenu( QPoint p )
{
    m_pMenu->popup(p);
}

void ZoomTool::zoomSelected()
{
  setOverride();
  KivioRect r = m_pCanvas->activePage()->getRectForAllSelectedStencils();

  if (!r.isNull() && r.isValid()) {
    m_pCanvas->setVisibleArea(r);
  }

  removeOverride();
}

void ZoomTool::zoomAllobjects()
{
  setOverride();
  KivioRect r = m_pCanvas->activePage()->getRectForAllStencils();

  if (!r.isNull() && r.isValid()) {
    m_pCanvas->setVisibleArea(r);
  }

  removeOverride();
}

void ZoomTool::zoomRect(QRect r)
{
  if (r.isEmpty()) {
    m_pCanvas->zoomIn(r.topLeft());
    return;
  }

  KoPoint p0 = m_pCanvas->mapFromScreen(r.topLeft());
  m_pCanvas->setVisibleArea(KivioRect(p0.x(), p0.y(), m_pView->zoomHandler()
    ->unzoomItX(r.width()), m_pView->zoomHandler()->unzoomItY(r.height())));
}
#include "tool_zoom.moc"
