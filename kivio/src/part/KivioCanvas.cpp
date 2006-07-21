/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KivioCanvas.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QBrush>
#include <QMouseEvent>
#include <QKeyEvent>

#include <KoZoomHandler.h>
#include <KoPageLayout.h>
#include <KoShapeManager.h>
#include <KoTool.h>
#include <KoPointerEvent.h>

#include "KivioView.h"
#include "KivioDocument.h"
#include "KivioAbstractPage.h"

KivioCanvas::KivioCanvas(KivioView* parent)
  : QWidget(parent), m_view(parent)
{
  m_shapeManager = new KoShapeManager(this);

  setMouseTracking(true);

  QPalette newPalette = palette();
  newPalette.setBrush(QPalette::Base, Qt::white); // TODO Make background a page property
  setPalette(newPalette);
  setBackgroundRole(QPalette::Base);

  updateSize();
}

KivioCanvas::~KivioCanvas()
{
}

void KivioCanvas::gridSize(double *horizontal, double *vertical) const
{
  // TODO Implement this
  *horizontal = 0;
  *vertical = 0;
}

bool KivioCanvas::snapToGrid() const
{
  // TODO Implement this
  return false;
}

void KivioCanvas::addCommand(KCommand* command, bool execute)
{
  m_view->document()->addCommand(command, execute);
}

KoShapeManager* KivioCanvas::shapeManager() const
{
  return m_shapeManager;
}

void KivioCanvas::updateCanvas(const QRectF& rc)
{
  QRect clipRect(viewConverter()->documentToView(rc).toRect());
  clipRect.adjust(-2, -2, 2, 2); // Resize to fit anti-aliasing
  update(clipRect);
}

KoViewConverter* KivioCanvas::viewConverter()
{
  return m_view->zoomHandler();
}

QWidget* KivioCanvas::canvasWidget()
{
  return this;
}

void KivioCanvas::updateSize()
{
  int width = 0;
  int height = 0;

  if(m_view->activePage()) {
    KoPageLayout pageLayout = m_view->activePage()->pageLayout();
    width = qRound(m_view->zoomHandler()->zoomItX(pageLayout.ptWidth));
    height = qRound(m_view->zoomHandler()->zoomItX(pageLayout.ptHeight));
  }

  setMinimumSize(width, height);
}

void KivioCanvas::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setClipRect(event->rect());

  shapeManager()->paint(painter, *(viewConverter()), false);
  m_tool->paint(painter, *(viewConverter()));

  painter.end();
}

void KivioCanvas::mouseMoveEvent(QMouseEvent* event)
{
  KoPointerEvent pointerEvent(event, viewConverter()->viewToDocument(event->pos()));
  m_tool->mouseMoveEvent(&pointerEvent);
}

void KivioCanvas::mousePressEvent(QMouseEvent* event)
{
  KoPointerEvent pointerEvent(event, viewConverter()->viewToDocument(event->pos()));
  m_tool->mousePressEvent(&pointerEvent);
}

void KivioCanvas::mouseReleaseEvent(QMouseEvent* event)
{
  KoPointerEvent pointerEvent(event, viewConverter()->viewToDocument(event->pos()));
  m_tool->mouseReleaseEvent(&pointerEvent);
}

void KivioCanvas::keyPressEvent(QKeyEvent* event)
{
  m_tool->keyPressEvent(event);
}

void KivioCanvas::keyReleaseEvent(QKeyEvent* event)
{
  m_tool->keyReleaseEvent(event);
}

#include "KivioCanvas.moc"
