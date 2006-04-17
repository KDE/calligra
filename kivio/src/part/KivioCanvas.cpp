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

#include <KoZoomHandler.h>
#include <KoPageLayout.h>

#include "KivioView.h"

KivioCanvas::KivioCanvas(KivioView* parent)
  : QWidget(parent)
{
  m_offsetX = m_offsetY = 0;

  setAttribute(Qt::WA_OpaquePaintEvent, true);
  setMouseTracking(true);
}

KivioCanvas::~KivioCanvas()
{
}

void KivioCanvas::setOffsetX(int offset)
{
  m_offsetX = offset;
  update();
}

void KivioCanvas::setOffsetY(int offset)
{
  m_offsetY = offset;
  update();
}

int KivioCanvas::offsetX() const
{
  return m_offsetX;
}

int KivioCanvas::offsetY() const
{
  return m_offsetY;
}

int KivioCanvas::pageWidth() const
{
  // TODO Implement this correctly
  KoZoomHandler zoomHandler;
  KoPageLayout pageLayout = KoPageLayout::standardLayout();
  return zoomHandler.zoomItX(pageLayout.ptWidth);
}

int KivioCanvas::pageHeight() const
{
  // TODO Implement this correctly
  KoZoomHandler zoomHandler;
  KoPageLayout pageLayout = KoPageLayout::standardLayout();
  return zoomHandler.zoomItY(pageLayout.ptHeight);
}

void KivioCanvas::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event);
  KoPageLayout pageLayout = KoPageLayout::standardLayout();

  QPainter painter(this);
  painter.translate(-offsetX(), -offsetY());

  KoZoomHandler zoomHandler;
  int pageWidth = zoomHandler.zoomItX(pageLayout.ptWidth);
  int pageHeight = zoomHandler.zoomItY(pageLayout.ptHeight);
  qreal pageX = 0.0;
  qreal pageY = 0.0;

  if(pageWidth < width()) {
    pageX = (width() - pageWidth) / 2;
  }

  if(pageHeight < height()) {
    pageY = (height() - pageHeight) / 2;
  }

  if((pageX > 0.0) || (pageY > 0.0)) {
    painter.fillRect(offsetX(), offsetY(), width(), height(), Qt::darkGray);
  }

  QRectF pageRect(pageX, pageY, pageWidth, pageHeight);
  painter.setPen(Qt::black);
  painter.setBrush(Qt::white);
  painter.drawRect(pageRect);
  painter.setPen(QPen(QColor(5, 5, 5, 127), 3));
  painter.drawLine(QPointF(pageX + pageWidth + 1, pageY + 3), QPointF(pageX + pageWidth + 1, pageY + pageHeight + 1));
  painter.drawLine(QPointF(pageX + 3, pageY + pageHeight + 1), QPointF(pageX + pageWidth - 2, pageY + pageHeight + 1));
}

#include "KivioCanvas.moc"
