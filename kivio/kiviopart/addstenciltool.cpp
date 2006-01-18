/*
 * Kivio - Visual Modelling and Flowcharting
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "addstenciltool.h"

#include <qevent.h>
#include <qcursor.h>

#include "kdebug.h"

#include "kivio_canvas.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil.h"
#include "kivio_page.h"
#include "kivio_doc.h"
#include "kivio_pluginmanager.h"

namespace Kivio {

AddStencilTool::AddStencilTool(KivioView* parent)
 : MouseTool(parent, "Add Stencil Mouse Tool")
{
  m_leftMouseButtonPressed = false;
  m_stencil = 0;
  m_spawner = 0;
}

AddStencilTool::~AddStencilTool()
{
  delete m_stencil;
  m_stencil = 0;
}

bool AddStencilTool::processEvent(QEvent* e)
{
  switch(e->type()) {
    case QEvent::MouseButtonPress:
    {
      QMouseEvent* me = static_cast<QMouseEvent*>(e);

      if(me->button() == LeftButton) {
        m_leftMouseButtonPressed = true;
        createStencil(me->pos());
        return true;
      }

      break;
    }
    case QEvent::MouseButtonRelease:
    {
      QMouseEvent* me = static_cast<QMouseEvent*>(e);

      if(m_leftMouseButtonPressed && (me->button() == LeftButton)) {
        m_leftMouseButtonPressed = false;
        endOperation(me->pos());
        return true;
      }

      break;
    }
    case QEvent::MouseMove:
    {
      QMouseEvent* me = static_cast<QMouseEvent*>(e);

      if(m_leftMouseButtonPressed) {
        resize(me->pos());
        return true;
      }

      break;
    }
    default:
      break;
  }

  return false;
}

void AddStencilTool::setActivated(bool a)
{
  if(a) {
    m_leftMouseButtonPressed = false;
    view()->canvasWidget()->setCursor(Qt::CrossCursor);
    emit activated(this);
  } else {
    view()->canvasWidget()->unsetCursor();
    m_spawner = 0;
    delete m_stencil;
    m_stencil = 0;
  }
}

void AddStencilTool::activateNewStencil(KivioStencilSpawner* spawner)
{
  m_spawner = spawner;
  setActivated(true);
}

void AddStencilTool::createStencil(const QPoint& position)
{
  if(!m_spawner) {
    return;
  }

  m_startPoint = view()->canvasWidget()->mapFromScreen(position);
  m_startPoint = view()->canvasWidget()->snapToGridAndGuides(m_startPoint);

  m_stencil = m_spawner->newStencil();

  if(!m_stencil) {
    return;
  }

  m_stencil->setPosition(m_startPoint.x(), m_startPoint.y());
  m_originalSize.setWidth(m_stencil->w());
  m_originalSize.setHeight(m_stencil->h());
  m_stencil->setW(1);
  m_stencil->setH(1);
  view()->canvasWidget()->beginUnclippedSpawnerPainter();
  view()->canvasWidget()->drawStencilXOR(m_stencil);
}

void AddStencilTool::endOperation(const QPoint& position)
{
  if(!m_stencil) {
    return;
  }

  KoPoint endPoint = view()->canvasWidget()->mapFromScreen(position);
  endPoint = view()->canvasWidget()->snapToGridAndGuides(endPoint);

  if(m_startPoint == endPoint) {
    m_stencil->setW(m_originalSize.width());
    m_stencil->setH(m_originalSize.height());
  }

  KivioPage* page = view()->canvasWidget()->activePage();
  page->unselectAllStencils();
  page->addStencil(m_stencil);
  page->selectStencil(m_stencil);
  m_stencil = 0;

  view()->canvasWidget()->endUnclippedSpawnerPainter();
  view()->doc()->updateView(page);

  view()->pluginManager()->activateDefaultTool();
}

void AddStencilTool::resize(const QPoint& position)
{
  KoPoint endPoint = view()->canvasWidget()->mapFromScreen(position);
  endPoint = view()->canvasWidget()->snapToGridAndGuides(endPoint);

  view()->canvasWidget()->drawStencilXOR(m_stencil);
  m_stencil->setW(endPoint.x() - m_startPoint.x());
  m_stencil->setH(endPoint.y() - m_startPoint.y());
  view()->canvasWidget()->drawStencilXOR(m_stencil);
}

}

#include "addstenciltool.moc"
