/*
 *  canvasview.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "canvasview.h"

CanvasView::CanvasView(QWidget *parent) : QWidget(parent)
{
  setMouseTracking(true);
}
CanvasView::~CanvasView() {}

void CanvasView::paintEvent(QPaintEvent *e)
{
  emit sigPaint(e);
}

void CanvasView::mousePressEvent(QMouseEvent *e)
{
  emit sigMousePress(e);
}

void CanvasView::mouseMoveEvent(QMouseEvent *e)
{
  emit sigMouseMove(e);
}

void CanvasView::mouseReleaseEvent(QMouseEvent *e)
{
  emit sigMouseRelease(e);
}

#include "canvasview.moc"
