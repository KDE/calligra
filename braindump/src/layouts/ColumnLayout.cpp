/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ColumnLayout.h"

#include <QRectF>

#include <kdebug.h>
#include <klocale.h>
#include <Utils.h>
#include <KoShape.h>

ColumnLayout::ColumnLayout() : Layout("columnlayout"), m_isUpdating(false)
{
}

ColumnLayout::~ColumnLayout() {
}

QRectF ColumnLayout::boundingBox() const {
  QRectF rect = Layout::boundingBox();
  rect.adjust(-10,0,10,0);
  return rect;
}

void ColumnLayout::shapeAdded(KoShape* _shape) {
  int idx = findIndex(_shape);
  m_shapes.insert(idx, _shape);
  updateShapesPosition();
}

void ColumnLayout::shapeRemoved(KoShape* _shape) {
  m_shapes.removeAll(_shape);
  updateShapesPosition();
}

void ColumnLayout::shapeGeometryChanged(KoShape* _shape) {
  Q_UNUSED(_shape);
  Q_ASSERT(m_shapes.contains(_shape));
  if(m_isUpdating) return;
  m_shapes.move(m_shapes.indexOf(_shape), findIndex(_shape));
  updateShapesPosition();
}

void ColumnLayout::updateShapesPosition() {
  Q_ASSERT(not m_isUpdating);
  m_isUpdating = true;
  double y = 0;
  foreach(KoShape* shape, m_shapes) {
    shape->update();
    QRectF b;
    Utils::containerBoundRec(shape, b);
    QPointF transfo = QPointF(0.0, y) - b.topLeft();
    shape->setAbsolutePosition( transfo + shape->absolutePosition(KoFlake::TopLeftCorner), KoFlake::TopLeftCorner);
    y += b.height();
    shape->update();
  }
  emit(boundingBoxChanged(boundingBox()));
  m_isUpdating = false;
}

int ColumnLayout::findIndex(KoShape* _shape) {
  qreal y = _shape->position().y();
  for(int i = 0; i < m_shapes.count(); ++i) {
    if(m_shapes[i] == _shape ) {
      if( i < m_shapes.count() - 1 and m_shapes[i+1]->position().y() < y ) {
        return i + 1;
      } else {
        return i;
      }
    } else if(m_shapes[i]->position().y() > y ) {
      return i;
    }
  }
  kDebug() << "No one";
  return m_shapes.count() - 1;
}


ColumnLayoutFactory::ColumnLayoutFactory() : LayoutFactory("columnlayout", i18n("Column")) {
}

ColumnLayoutFactory::~ColumnLayoutFactory() {
}

Layout* ColumnLayoutFactory::createLayout() const {
  return new ColumnLayout;
}
