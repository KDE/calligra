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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "groupobject.h"

#include <qpainter.h>
#include <qrect.h>

#include <koRect.h>
#include <koSize.h>
#include <kozoomhandler.h>

namespace Kivio {

GroupObject::GroupObject()
 : RectangleObject()
{
}

GroupObject::~GroupObject()
{
  QValueList<Object*>::iterator it = m_objectList.begin();
  QValueList<Object*>::iterator itEnd = m_objectList.end();
  Object* obj = 0;

  while(it != itEnd) {
    obj = *it;
    it = m_objectList.remove(it);
    delete obj;
  }
}

Object* GroupObject::duplicate()
{
  GroupObject* object = new GroupObject(*this);

  return object;
}

ShapeType GroupObject::type()
{
  return kstGroup;
}

void GroupObject::resize(double xOffset, double yOffset)
{
  KoSize newSize(xOffset, yOffset);
  KoSize oldSize = size();
  newSize += oldSize;
  double percentIncreaseW = newSize.width() / oldSize.width();
  double percentIncreaseH = newSize.height() / oldSize.height();
  setSize(newSize);

  QValueList<Kivio::Object*>::iterator it = m_objectList.begin();
  QValueList<Kivio::Object*>::iterator itEnd = m_objectList.end();

  for(;it != itEnd; ++it) {
    if((xOffset > 1e-13) || (yOffset > 1e-13)) {
      (*it)->resizeInPercent(percentIncreaseW, percentIncreaseH);
    }
  }
}

void GroupObject::resizeInPercent(double percentWidth, double percentHeight)
{
  RectangleObject::resizeInPercent(percentWidth, percentHeight);

  QValueList<Kivio::Object*>::iterator it = m_objectList.begin();
  QValueList<Kivio::Object*>::iterator itEnd = m_objectList.end();

  for(;it != itEnd; ++it) {
    (*it)->resizeInPercent(percentWidth, percentHeight);
  }
}

QBrush GroupObject::brush() const
{
  return m_objectList.first()->brush();
}

void GroupObject::setBrush(const QBrush& newBrush)
{
  QValueList<Kivio::Object*>::iterator it = m_objectList.begin();
  QValueList<Kivio::Object*>::iterator itEnd = m_objectList.end();

  for(;it != itEnd; ++it) {
    (*it)->setBrush(newBrush);
  }
}

Pen GroupObject::pen() const
{
  return m_objectList.first()->pen();
}

void GroupObject::setPen(const Pen& newPen)
{
  QValueList<Kivio::Object*>::iterator it = m_objectList.begin();
  QValueList<Kivio::Object*>::iterator itEnd = m_objectList.end();

  for(;it != itEnd; ++it) {
    (*it)->setPen(newPen);
  }
}

void GroupObject::paint(QPainter& painter, KoZoomHandler* zoomHandler)
{
  painter.save();
  painter.setWindow(zoomHandler->zoomRect(KoRect(position(), size())));
  QValueList<Kivio::Object*>::iterator it = m_objectList.begin();
  QValueList<Kivio::Object*>::iterator itEnd = m_objectList.end();

  for(;it != itEnd; ++it) {
    (*it)->paint(painter, zoomHandler);
  }

  painter.restore();
}

void GroupObject::addObject(Object* newObject)
{
  m_objectList.append(newObject);
}

}
