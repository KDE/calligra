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
#include "tool_connector.h"

#include <qcursor.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <kdebug.h>

#include "kivio_view.h"
#include "kivio_canvas.h"
#include "kivio_page.h"
#include "kivio_doc.h"

#include "kivio_stencil_spawner_set.h"
#include "kivio_stencil_spawner.h"
#include "kivio_custom_drag_data.h"
#include "kivio_layer.h"
#include "kivio_point.h"
#include "kivio_stencil.h"
#include "kivio_custom_drag_data.h"
#include "straight_connector.h"

ConnectorTool::ConnectorTool( KivioView* view )
:Tool(view,"Connector")
{
  setSortNum(3);

  ToolSelectAction* connector = new ToolSelectAction( actionCollection(), "ToolAction" );
  KAction* m_z1 = new KAction( i18n("Edit Stencil Connector"), "kivio_connector", 0, actionCollection(), "connector" );
  connector->insert(m_z1);

  m_mode = stmNone;

  m_pConnectorCursor1 = new QCursor(BarIcon("kivio_connector_cursor1"),2,2);
  m_pConnectorCursor2 = new QCursor(BarIcon("kivio_connector_cursor2"),2,2);
}

ConnectorTool::~ConnectorTool()
{
}


/**
 * Event delegation
 *
 * @param e The event to be identified and processed
 *
 */
void ConnectorTool::processEvent( QEvent* e )
{
    switch (e->type())
    {
    case QEvent::MouseButtonPress:
        mousePress( (QMouseEvent*)e );
        break;

    case QEvent::MouseButtonRelease:
        mouseRelease( (QMouseEvent*)e );
        break;

    case QEvent::MouseMove:
        mouseMove( (QMouseEvent*)e );
        break;

    default:
      break;
    }
}

void ConnectorTool::activate()
{
    kdDebug() << "ConnectorTool activate" << endl;
    m_pCanvas->setCursor(*m_pConnectorCursor1);
    m_mode = stmNone;
    m_pStencil = 0;
    m_pDragData = 0;
}

void ConnectorTool::deactivate()
{
    m_pStencil = 0;
    if (m_pDragData)
      delete m_pDragData;

    m_pDragData = 0;
}

void ConnectorTool::configure()
{
}

void ConnectorTool::connector(QRect /*r*/)
{
    if (!m_pStencil)
      return;

    delete m_pDragData;
    m_pDragData = 0;

    KivioDoc* doc = m_pView->doc();
    KivioPage* page = m_pCanvas->activePage();

    if (m_pStencil->w() < 3.0 && m_pStencil->h() < 3.0) {
        page->unselectAllStencils();
        page->selectStencil(m_pStencil);
        page->deleteSelectedStencils();
        m_pStencil = 0;
        doc->updateView(page,false);
        return;
    }

    m_pStencil->searchForConnections(page);
    doc->updateView(page);
}

void ConnectorTool::mousePress( QMouseEvent *e )
{
    if( startRubberBanding( e ) )
    {
        m_mode = stmDrawRubber;
    }
}


/**
 * Tests if we should start rubber banding (always returns true).
 */
bool ConnectorTool::startRubberBanding( QMouseEvent *e )
{
    KivioDoc* doc = m_pView->doc();
    KivioPage* pPage = m_pCanvas->activePage();
    KivioStencilSpawner* ss = doc->findInternalStencilSpawner("Straight Connector");
    if (!ss)
        return false;

    startPoint = m_pCanvas->snapToGrid(m_pCanvas->mapFromScreen( e->pos() ));

    m_pStencil = (KivioStraightConnector*)ss->newStencil();
    m_pDragData = new KivioCustomDragData();
    m_pDragData->page = pPage;
    m_pDragData->x = startPoint.x;
    m_pDragData->y = startPoint.y;
    m_pDragData->id = kctCustom + 1;

    m_pStencil->setStartPoint(startPoint.x+10.0f,startPoint.y+10.0f);
    m_pStencil->setEndPoint(startPoint.x,startPoint.y);
    m_pStencil->customDrag(m_pDragData);

    pPage->unselectAllStencils();
    pPage->addStencil(m_pStencil);
    pPage->selectStencil(m_pStencil);

    m_pStencil->searchForConnections(m_pCanvas->activePage());

    m_pCanvas->repaint();
    m_pCanvas->setCursor(*m_pConnectorCursor2);
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
    TKPoint endPoint = m_pCanvas->mapFromScreen( e->pos() );
    endPoint = m_pCanvas->snapToGrid(endPoint);

    m_pStencil->setStartPoint(endPoint.x,endPoint.y);


    m_pDragData->x = endPoint.x;
    m_pDragData->y = endPoint.y;
    m_pDragData->id = kctCustom + 1;
    m_pStencil->customDrag(m_pDragData);

    m_pStencil->updateGeometry();
    m_pCanvas->repaint();
}

void ConnectorTool::mouseRelease( QMouseEvent *e )
{
    switch( m_mode )
    {
        case stmDrawRubber:
            endRubberBanding(e);
            break;
    }

    m_pCanvas->setCursor(*m_pConnectorCursor1);
        m_mode = stmNone;
}

void ConnectorTool::endRubberBanding(QMouseEvent *)
{
    connector(m_pCanvas->rect());
    m_pStencil = 0;
}
#include "tool_connector.moc"
