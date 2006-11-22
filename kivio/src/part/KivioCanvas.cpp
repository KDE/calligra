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
#include <KoToolManager.h>
#include <KoPointerEvent.h>
#include <KoToolProxy.h>

#include "KivioView.h"
#include "KivioDocument.h"
#include "KivioAbstractPage.h"

KivioCanvas::KivioCanvas(KivioView* parent)
  : QWidget(parent), KoCanvasBase(parent), m_view(parent)
{
  m_shapeManager = new KoShapeManager(this);
  m_toolProxy = KoToolManager::instance()->toolProxy();

  setMouseTracking(true);

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
  painter.setClipRect(event->rect());
  QColor pageColor = QColor(Qt::white);
  painter.fillRect(event->rect(), QBrush(pageColor));
  painter.setRenderHint(QPainter::Antialiasing);

  shapeManager()->paint(painter, *(viewConverter()), false);
  m_toolProxy->paint(painter, *(viewConverter()));

  painter.end();
}

void KivioCanvas::mouseMoveEvent(QMouseEvent* event)
{
  m_toolProxy->mouseMoveEvent(event, viewConverter()->viewToDocument(event->pos()));
  m_view->updateMousePosition(event->pos());
}

void KivioCanvas::mousePressEvent(QMouseEvent* event)
{
  m_toolProxy->mousePressEvent(event, viewConverter()->viewToDocument(event->pos()));
}

void KivioCanvas::mouseReleaseEvent(QMouseEvent* event)
{
  m_toolProxy->mouseReleaseEvent(event, viewConverter()->viewToDocument(event->pos()));
}

void KivioCanvas::keyPressEvent(QKeyEvent* event)
{
  m_toolProxy->keyPressEvent(event);
}

void KivioCanvas::keyReleaseEvent(QKeyEvent* event)
{
  m_toolProxy->keyReleaseEvent(event);
}

#include "KivioCanvas.moc"
