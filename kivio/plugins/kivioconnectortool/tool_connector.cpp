/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 * Copyright (C) 2003-2005 Peter Simonsson <psn@linux.se>
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
#include "tool_connector.h"

#include <qcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <koPoint.h>
#include <kozoomhandler.h>
#include <klocale.h>

#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_page.h"
#include "kivio_doc.h"
#include "kivio_factory.h"

#include "kivio_stencil_spawner_set.h"
#include "kivio_stencil_spawner.h"
#include "kivio_custom_drag_data.h"
#include "kivio_layer.h"
#include "kivio_point.h"
#include "kivio_stencil.h"
#include "straight_connector.h"
#include "kivio_pluginmanager.h"
#include "kivio_1d_stencil.h"
#include "kiviopolylineconnector.h"
#include "polylineconnectorspawner.h"
#include "mousetoolaction.h"
#include "connector.h"

ConnectorTool::ConnectorTool( KivioView* parent ) : Kivio::MouseTool(parent, "Connector Mouse Tool")
{
  m_polyLineAction = new Kivio::MouseToolAction(i18n("Connector"), "kivio_connector", 0,
    actionCollection(), "polyLineConnector");
  connect(m_polyLineAction, SIGNAL(toggled(bool)), this, SLOT(setActivated(bool)));
  connect(m_polyLineAction, SIGNAL(activated()), this, SLOT(activatePolyline()));
  connect(m_polyLineAction, SIGNAL(doubleClicked()), this, SLOT(makePermanent()));
  m_polyLineAction->setExclusiveGroup("ConnectorTool");

  m_permanent = false;

  m_mode = stmNone;

  m_pConnectorCursor1 = new QCursor(BarIcon("kivio_connector_cursor1",KivioFactory::global()),2,2);
  m_pConnectorCursor2 = new QCursor(BarIcon("kivio_connector_cursor2",KivioFactory::global()),2,2);
}

ConnectorTool::~ConnectorTool()
{
  delete m_pConnectorCursor1;
  delete m_pConnectorCursor2;
}


/**
 * Event delegation
 *
 * @param e The event to be identified and processed
 *
 */
bool ConnectorTool::processEvent(QEvent* e)
{
  switch (e->type())
  {
  case QEvent::MouseButtonPress:
    mousePress( static_cast<QMouseEvent*>(e) );
    return true;
    break;

  case QEvent::MouseMove:
    mouseMove( static_cast<QMouseEvent*>(e) );
    return true;
    break;

  default:
    break;
  }

  return false;
}

void ConnectorTool::setActivated(bool a)
{
  if(a) {
    view()->canvasWidget()->setCursor(*m_pConnectorCursor1);
    m_mode = stmNone;
    m_connector = 0;
    emit activated(this);
  } else {
    m_connector = 0;
    m_polyLineAction->setChecked(false);
    m_permanent = false;
    view()->setStatusBarInfo("");
  }
}

void ConnectorTool::mousePress( QMouseEvent *e )
{
  if(e->button() == LeftButton) {
    bool ok = true;

    if(!m_connector) {
      ok = startRubberBanding(e);
    } else {
      KivioCanvas* canvas = view()->canvasWidget();
      KivioPage* pPage = canvas->activePage();
      bool hit = false;
      KoPoint point = pPage->snapToTarget(canvas->mapFromScreen(e->pos()), 8.0, hit);

      if(!hit) {
        point = canvas->snapToGrid(point);
      }

      if((m_mode == stmDrawRubber) && hit) {
        endRubberBanding(e);
      } else {
        m_connector->addPoint(point);
      }
    }

    if(ok) {
      m_mode = stmDrawRubber;
    } else {
      m_mode = stmNone;
    }
  } else if(e->button() == RightButton) {
    if(m_mode == stmDrawRubber) {
      endRubberBanding(e);
    }

    view()->canvasWidget()->setCursor(*m_pConnectorCursor1);
    m_mode = stmNone;
  }
}


/**
 * Tests if we should start rubber banding (always returns true).
 */
bool ConnectorTool::startRubberBanding( QMouseEvent *e )
{
  //FIXME Port to Object code
  KivioCanvas* canvas = view()->canvasWidget();
  KivioDoc* doc = view()->doc();
  KivioPage* pPage = canvas->activePage();

  // Create the connector
  m_connector = new Kivio::Connector();

  bool hit = false;
  KoPoint startPoint = pPage->snapToTarget(canvas->mapFromScreen(e->pos()), 8.0, hit);

  if(!hit) {
    startPoint = canvas->snapToGrid(startPoint);
  }

  m_connector->addPoint(startPoint);
  m_connector->addPoint(startPoint);

  // Unselect everything, add the stencil to the page, and select it
  pPage->unselectAllStencils();
  pPage->addStencil(m_connector);
  pPage->selectStencil(m_connector);

  canvas->repaint();
  canvas->setCursor(*m_pConnectorCursor2);

  return true;
}

void ConnectorTool::mouseMove( QMouseEvent * e )
{
  switch( m_mode )
  {
    case stmDrawRubber:
      continueRubberBanding(e);
      break;

    default:
      break;
  }
}

void ConnectorTool::continueRubberBanding( QMouseEvent *e )
{
  KivioCanvas* canvas = view()->canvasWidget();
  KivioPage* pPage = view()->activePage();
  bool hit = false;
  KoPoint endPoint = pPage->snapToTarget(canvas->mapFromScreen(e->pos()), 8.0, hit);

  if(!hit) {
    endPoint = canvas->snapToGrid(endPoint);
  }

  m_connector->changePoint(m_connector->pointVector().count() - 1, endPoint);
  canvas->repaint();
}

void ConnectorTool::endRubberBanding(QMouseEvent *)
{
  m_connector = 0;

  if(!m_permanent) {
    view()->pluginManager()->activateDefaultTool();
  }
}

void ConnectorTool::activatePolyline()
{
  m_polyLineAction->setChecked(true);
  view()->setStatusBarInfo(i18n("Left mouse button to start drawing, right to end drawing."));
}

void ConnectorTool::makePermanent()
{
  m_permanent = true;
}

#include "tool_connector.moc"
