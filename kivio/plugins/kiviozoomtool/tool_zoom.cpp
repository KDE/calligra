/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
#include "kivio_view.h"
#include "kivio_page.h"
#include "kivio_canvas.h"

#include "toolbarseparator.h"

#include <kaction.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <ktoolbar.h>
#include <kdebug.h>

#include <qapplication.h>
#include <qcursor.h>

ZoomTool::ZoomTool(KivioView* view)
:Tool(view,"Zoom")
{
  setSortNum(1);
  m_pToolBar = 0L;

  new ToolBarSeparator(actionCollection(),"---");
  m_z1 = new KRadioAction( i18n("Zoom"), "kivio_zoom", Key_F2, actionCollection(), "zoom" );
  m_z2 = new KRadioAction( i18n("Hand"), "kivio_zoom_hand", Key_F3, actionCollection(), "zoomHand" );
  m_z1->setExclusiveGroup("zoomAction");
  m_z2->setExclusiveGroup("zoomAction");
  connect(m_z1,SIGNAL(activated()),SLOT(zoomActivated()));
  connect(m_z2,SIGNAL(activated()),SLOT(handActivated()));

  m_pPlus = new KAction( i18n("Zoom plus"), "kivio_zoom_plus", SHIFT+Key_F2, actionCollection(), "zoomPlus" );
  connect(m_pPlus,SIGNAL(activated()),SLOT(zoomPlus()));

  m_pMinus = new KAction( i18n("Zoom minus"), "kivio_zoom_minus", SHIFT+Key_F3, actionCollection(), "zoomMinus" );
  connect(m_pMinus,SIGNAL(activated()),SLOT(zoomMinus()));

  m_pZoomWidth = new KAction( i18n("Zoom width"), "kivio_zoom_width", SHIFT+Key_F4, actionCollection(), "zoomWidth" );
  connect(m_pZoomWidth,SIGNAL(activated()),SLOT(zoomWidth()));

  m_pZoomHeight = new KAction( i18n("Zoom height"), "kivio_zoom_height", SHIFT+Key_F5, actionCollection(), "zoomHeight" );
  connect(m_pZoomHeight,SIGNAL(activated()),SLOT(zoomHeight()));

  m_pZoomPage = new KAction( i18n("Zoom page"), "kivio_zoom_page", SHIFT+Key_F6, actionCollection(), "zoomPage" );
  connect(m_pZoomPage,SIGNAL(activated()),SLOT(zoomPage()));

  m_pZoomSelected = new KAction( i18n("Zoom selected"), "kivio_zoom_selected", CTRL+Key_Y, actionCollection(), "zoomSelected" );
  connect(m_pZoomSelected,SIGNAL(activated()),SLOT(zoomSelected()));

  m_pZoomAllObjects = new KAction( i18n("Zoom all objects"), "kivio_zoom_allobject", 0, actionCollection(), "zoomAllObjects" );
  connect(m_pZoomAllObjects,SIGNAL(activated()),SLOT(zoomAllobjects()));

  QPixmap pix;

  pix = BarIcon("kivio_zoom_plus");
  m_pPlusCursor = new QCursor(pix,pix.width()/2,pix.height()/2);

  pix = BarIcon("kivio_zoom_minus");
  m_pMinusCursor = new QCursor(pix,pix.width()/2,pix.height()/2);

  pix = BarIcon("kivio_zoom_hand");
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
        if ( m_pCurrent == m_pMinus )
          m_pCurrent->activate();
        else {
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
  	    m_pCanvas->endRectDraw();
        m_bDrawRubber = false;
        m_bLockKeyboard = false;
        zoomRect(m_pCanvas->rect());
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
        m_pCanvas->setUpdatesEnabled(false);
        mousePos = ((QMouseEvent*)e)->pos();
        break;
      case QEvent::MouseButtonRelease:
        isHandMousePressed = false;
        m_pCanvas->setUpdatesEnabled(true);
        break;
      case QEvent::MouseMove:
        if (isHandMousePressed) {
          QPoint newPos = ((QMouseEvent*)e)->pos();
          mousePos -= newPos;
          m_pCanvas->scrollDx(-mousePos.x());
          m_pCanvas->scrollDy(-mousePos.y());
          mousePos = newPos;
        }
        break;
      default:
        break;
    }
}

void ZoomTool::activateGUI(KXMLGUIFactory* factory)
{
  m_pToolBar = (KToolBar*)factory->container("ZoomToolBar",this);
  if (m_pToolBar)
    m_pToolBar->hide();
}

void ZoomTool::deactivateGUI( KXMLGUIFactory* )
{
}

void ZoomTool::activate()
{
  m_pToolBar = (KToolBar*)m_pView->factory()->container("ZoomToolBar",this);
  if (m_pToolBar)
    m_pToolBar->show();

  m_pCurrent = m_pPlus;
}

void ZoomTool::deactivate()
{
   kdDebug() << "ZoomTool DeActivate" << endl;
  if (m_pToolBar)
   m_pToolBar->hide();

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
   if(m_pCanvas->zoom()>=10000)
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
   if(m_pCanvas->zoom()<=0.1f)
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
   
   int cw = QMAX(10,m_pCanvas->width()-20);
   TKPageLayout pl = m_pCanvas->activePage()->paperLayout();
   float w = pl.ptWidth();
   float z = cw/w;
   
   m_pCanvas->setUpdatesEnabled(false);
   m_pCanvas->centerPage();
   m_pCanvas->setZoom(z);
   m_pCanvas->setUpdatesEnabled(true);

   removeOverride();
}

void ZoomTool::zoomHeight()
{
  setOverride();

  int ch = QMAX(10,m_pCanvas->height()-20);
  TKPageLayout pl = m_pCanvas->activePage()->paperLayout();
  float h = pl.ptHeight();
  float zh = ch/h;

  m_pCanvas->setUpdatesEnabled(false);
  m_pCanvas->setZoom(zh);
  m_pCanvas->centerPage();
  m_pCanvas->setUpdatesEnabled(true);

  removeOverride();
}

void ZoomTool::zoomPage()
{
  setOverride();

  int cw = QMAX(10,m_pCanvas->width()-20);
  int ch = QMAX(10,m_pCanvas->height()-20);

  TKPageLayout pl = m_pCanvas->activePage()->paperLayout();
  float w = pl.ptWidth();
  float h = pl.ptHeight();

  float z = QMIN(cw/w,ch/h);

  m_pCanvas->setUpdatesEnabled(false);
  m_pCanvas->centerPage();
  m_pCanvas->setZoom(z);
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
  m_pCanvas->setVisibleArea(r);

  removeOverride();
}

void ZoomTool::zoomAllobjects()
{
  setOverride();

  KivioRect r = m_pCanvas->activePage()->getRectForAllStencils();
  m_pCanvas->setVisibleArea(r);

  removeOverride();
}

void ZoomTool::zoomRect(QRect r)
{
  if (r.isEmpty()) {
    m_pCanvas->zoomIn(r.topLeft());
    return;
  }

  TKPoint p0 = m_pCanvas->mapFromScreen(r.topLeft());
  m_pCanvas->setVisibleArea(KivioRect(p0.x, p0.y, r.width()/m_pCanvas->zoom(), r.height()/m_pCanvas->zoom()));
}
#include "tool_zoom.moc"
