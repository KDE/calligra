/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2005 Peter Simonsson
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
#include "rectangleobject.h"

#include <qpainter.h>
#include <qrect.h>

#include <koRect.h>
#include <kozoomhandler.h>

namespace Kivio {

RectangleObject::RectangleObject()
 : Object()
{
}

RectangleObject::~RectangleObject()
{
}

Object* RectangleObject::duplicate()
{
  RectangleObject* object = new RectangleObject(*this);

  return object;
}

ShapeType RectangleObject::type()
{
  return kstRectangle;
}

KoPoint RectangleObject::position() const
{
  return m_position;
}

void RectangleObject::setPosition(const KoPoint& newPosition)
{
  m_position.setX(QMAX(0, newPosition.x()));
  m_position.setY(QMAX(0, newPosition.y()));
}

void RectangleObject::move(double xOffset, double yOffset)
{
  KoPoint offset(xOffset, yOffset);
  setPosition(position() + offset);
}

KoSize RectangleObject::size() const
{
  return m_size;
}

void RectangleObject::setSize(const KoSize& newSize)
{
  if(newSize.width() > 0) m_size.setWidth(newSize.width());
  if(newSize.height() > 0) m_size.setHeight(newSize.height());
}

void RectangleObject::resize(double xOffset, double yOffset)
{
  KoSize newSize(xOffset, yOffset);
  newSize += m_size;

  setSize(newSize);
}

void RectangleObject::resizeInPercent(double percentWidth, double percentHeight)
{
  KoPoint newPosition;
  newPosition.setX(m_position.x() * percentWidth);
  newPosition.setY(m_position.y() * percentHeight);
  setPosition(newPosition);

  KoSize newSize;
  newSize.setWidth(m_size.width() * percentWidth);
  newSize.setHeight(m_size.height() * percentHeight);
  setSize(newSize);
}

KoRect RectangleObject::boundingBox()
{
  return KoRect(position(), size());
}

void RectangleObject::paint(QPainter& painter, KoZoomHandler* zoomHandler, bool paintHandles)
{
  QRect rect = zoomHandler->zoomRect(boundingBox());
  QPen zoomedPen = pen().zoomedPen(zoomHandler);
  painter.setPen(zoomedPen);
  painter.setBrush(brush());

  if(zoomedPen.width() > 1) {
    int lineOffset = (zoomedPen.width() / 2);
    rect.moveBy(lineOffset, lineOffset);
    rect.setSize(QSize(rect.size().width() - zoomedPen.width() + 1, rect.size().height() - zoomedPen.width() + 1));
  }

  painter.drawRect(rect);

  if(selected() && paintHandles) {
    paintSelection(painter, zoomHandler);
  }
}

void RectangleObject::paintSelection(QPainter& painter, KoZoomHandler* zoomHandler)
{
  QRect rect = zoomHandler->zoomRect(boundingBox());
  painter.setPen(QPen(QColor(0, 200, 0), 0, Qt::DashLine));
  painter.setBrush(Qt::NoBrush);
  painter.drawRect(rect);
  paintResizePoint(painter, rect.topLeft());
  paintResizePoint(painter, QPoint(rect.x() + qRound(rect.width() * 0.5), rect.y()));
  paintResizePoint(painter, rect.topRight());
  paintResizePoint(painter, QPoint(rect.x(), rect.y() + qRound(rect.height() * 0.5)));
  paintResizePoint(painter, QPoint(rect.x() + rect.width(), rect.y() + qRound(rect.height() * 0.5)));
  paintResizePoint(painter, rect.bottomLeft());
  paintResizePoint(painter, QPoint(rect.x() + qRound(rect.width() * 0.5), rect.y() + rect.height()));
  paintResizePoint(painter, rect.bottomRight());
}

KoPoint RectangleObject::moveResizePoint(int pointId, const KoPoint& _offset)
{
  KoPoint offset = _offset;
  KoPoint returnValue;

  switch(pointId) {
    case 0: //Top left
      resize(-offset.x(), -offset.y());
      move(offset.x(), offset.y());
      returnValue = boundingBox().topLeft();
      break;
    case 1: //Top center
    {
      resize(0, -offset.y());
      move(0, offset.y());
      KoRect rect = boundingBox();
      returnValue = KoPoint(rect.x() + (rect.width() * 0.5), rect.y());
      break;
    }
    case 2: // Top right
      resize(offset.x(), -offset.y());
      move(0, offset.y());
      returnValue = boundingBox().topRight();
      break;
    case 3: // Middle left
    {
      resize(-offset.x(), 0);
      move(offset.x(), 0);
      KoRect rect = boundingBox();
      returnValue = KoPoint(rect.x(), rect.y() + (rect.height() * 0.5));
      break;
    }
    case 4: // Middle right
    {
      resize(offset.x(), 0);
      KoRect rect = boundingBox();
      returnValue = KoPoint(rect.x() + rect.width(), rect.y() + (rect.height() * 0.5));
      break;
    }
    case 5: // Bottom left
      resize(-offset.x(), offset.y());
      move(offset.x(), 0);
      returnValue = boundingBox().bottomLeft();
      break;
    case 6: // Bottom center
    {
      resize(0, offset.y());
      KoRect rect = boundingBox();
      returnValue = KoPoint(rect.x() + (rect.width() * 0.5), rect.y() + rect.height());
      break;
    }
    case 7: // Bottom right
      resize(offset.x(), offset.y());
      returnValue = boundingBox().bottomRight();
      break;
    default:
      break;
  }

  return returnValue;
}

CollisionFeedback RectangleObject::contains(const KoPoint& point)
{
  KoRect rect = boundingBox();
  CollisionFeedback feedback;
  feedback.type = CTNone;

  if(hitResizePoint(rect.topLeft(), point)) {
    feedback.type = CTResizePoint;
    feedback.resizePointId = 0;
  } else if(hitResizePoint(KoPoint(rect.x() + (rect.width() * 0.5), rect.y()), point)) {
    feedback.type = CTResizePoint;
    feedback.resizePointId = 1;
  } else if(hitResizePoint(rect.topRight(), point)) {
    feedback.type = CTResizePoint;
    feedback.resizePointId = 2;
  } else if(hitResizePoint(KoPoint(rect.x(), rect.y() + (rect.height() * 0.5)), point)) {
    feedback.type = CTResizePoint;
    feedback.resizePointId = 3;
  } else if(hitResizePoint(KoPoint(rect.x() + rect.width(), rect.y() + (rect.height() * 0.5)), point)) {
    feedback.type = CTResizePoint;
    feedback.resizePointId = 4;
  } else if(hitResizePoint(rect.bottomLeft(), point)) {
    feedback.type = CTResizePoint;
    feedback.resizePointId = 5;
  } else if(hitResizePoint(KoPoint(rect.x() + (rect.width() * 0.5), rect.y() + rect.height()), point)) {
    feedback.type = CTResizePoint;
    feedback.resizePointId = 6;
  } else if(hitResizePoint(rect.bottomRight(), point)) {
    feedback.type = CTResizePoint;
    feedback.resizePointId = 7;
  }

  if(rect.contains(point)) {
    feedback.type = CTBody;
  }

  return feedback;
}

}
