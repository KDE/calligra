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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "tool_connector.h"

#include <qcursor.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <KoPoint.h>
#include <KoZoomHandler.h>
#include <kactionclasses.h>
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
#include "sml_connector.h"

SMLConnector::SMLConnector( KivioView* view )
 : Kivio::MouseTool(view, "SMLConnector")
{
  m_connectorAction = new KToggleAction(i18n("Polyline Connector"), "", 0,
    actionCollection(), "sml_connector");
  connect(m_connectorAction, SIGNAL(toggled(bool)), this, SLOT(setActivated(bool)));

  m_mode = stmNone;

  m_pConnectorCursor1 = new QCursor(BarIcon("kivio_connector_cursor1",KivioFactory::global()),2,2);
  m_pConnectorCursor2 = new QCursor(BarIcon("kivio_connector_cursor2",KivioFactory::global()),2,2);
}

SMLConnector::~SMLConnector()
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
bool SMLConnector::processEvent( QEvent* e )
{
    switch (e->type())
    {
    case QEvent::MouseButtonPress:
        mousePress( (QMouseEvent*)e );
        return true;
        break;

    case QEvent::MouseButtonRelease:
        mouseRelease( (QMouseEvent*)e );
        return true;
        break;

    case QEvent::MouseMove:
        mouseMove( (QMouseEvent*)e );
        return true;
        break;

    default:
      break;
    }

    return false;
}

void SMLConnector::setActivated(bool a)
{
  m_connectorAction->setChecked(a);
  
  if(a) {
    kDebug(43000) << "SMLConnector activate" << endl;
    view()->canvasWidget()->setCursor(*m_pConnectorCursor1);
    m_mode = stmNone;
    m_pStencil = 0;
    m_pDragData = 0;
    emit activated(this);
  } else {
    m_pStencil = 0;
    delete m_pDragData;
    m_pDragData = 0;
  }
}

void SMLConnector::connector(QRect)
{
    if (!m_pStencil)
      return;

    delete m_pDragData;
    m_pDragData = 0;

    KivioDoc* doc = view()->doc();
    KivioPage* page = view()->activePage();

    if (m_pStencil->w() < 3.0 && m_pStencil->h() < 3.0) {
        page->unselectAllStencils();
        page->selectStencil(m_pStencil);
        page->deleteSelectedStencils();
        m_pStencil = 0;
        doc->updateView(page);
        return;
    }

    m_pStencil->searchForConnections(page, view()->zoomHandler()->unzoomItY(4));
    doc->updateView(page);
}

void SMLConnector::mousePress( QMouseEvent *e )
{
  if(e->button() == Qt::RightButton)
  {
    return;
  }
  if( startRubberBanding( e ) )
  {
    m_mode = stmDrawRubber;
  }
}


/**
 * Tests if we should start rubber banding (always returns true).
 */
bool SMLConnector::startRubberBanding( QMouseEvent *e )
{
  KivioDoc* doc = view()->doc();
  KivioPage* pPage = view()->activePage();
  KivioCanvas* canvas = view()->canvasWidget();

  startPoint = canvas->snapToGrid(canvas->mapFromScreen( e->pos() ));

  // Create the stencil
    KivioStencilSpawner* ss = doc->findInternalStencilSpawner("SML Connector");

  if (!ss) {
    kDebug(43000) << "SMLTool: Failed to find StencilSpawner!" << endl;
    return false;
  }

  startPoint = canvas->snapToGrid(canvas->mapFromScreen( e->pos() ));

  // Create the stencil
  m_pStencil = (KivioSMLConnector*)ss->newStencil("basic_line");
  m_pStencil->setTextFont(doc->defaultFont());

  // Unselect everything, add the stencil to the page, and select it
  pPage->unselectAllStencils();
  pPage->addStencil(m_pStencil);
  pPage->selectStencil(m_pStencil);

  // Get drag info ready
  m_pDragData = new KivioCustomDragData();
  m_pDragData->page = pPage;
  m_pDragData->x = startPoint.x();
  m_pDragData->y = startPoint.y();
  m_pDragData->id = kctCustom + 2;

  m_pStencil->setStartPoint(startPoint.x() + 10.0f, startPoint.y() + 10.0f);
  m_pStencil->setEndPoint(startPoint.x(), startPoint.y());
  m_pStencil->customDrag(m_pDragData);


  canvas->repaint();
  canvas->setCursor(*m_pConnectorCursor2);
  return true;
}

void SMLConnector::mouseMove( QMouseEvent * e )
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

void SMLConnector::continueRubberBanding( QMouseEvent *e )
{
  KivioCanvas* canvas = view()->canvasWidget();
  KoPoint endPoint = canvas->mapFromScreen( e->pos() );
  endPoint = canvas->snapToGrid(endPoint);

  m_pStencil->setStartPoint(endPoint.x(), endPoint.y());


  m_pDragData->x = endPoint.x();
  m_pDragData->y = endPoint.y();
  m_pDragData->id = kctCustom + 1;
  m_pStencil->customDrag(m_pDragData);

  m_pStencil->updateGeometry();
  canvas->repaint();
}

void SMLConnector::mouseRelease( QMouseEvent *e )
{
  switch( m_mode )
  {
    case stmDrawRubber:
      endRubberBanding(e);
      break;
  }

  view()->canvasWidget()->setCursor(*m_pConnectorCursor1);
  m_mode = stmNone;
}

void SMLConnector::endRubberBanding(QMouseEvent *)
{
  connector(view()->canvasWidget()->rect());
  m_pStencil = 0;
}

#include "tool_connector.moc"
