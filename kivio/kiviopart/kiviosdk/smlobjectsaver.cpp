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
#include "smlobjectsaver.h"

#include <qdom.h>
#include <qbrush.h>

#include <koRect.h>

#include "object.h"
#include "rectangleobject.h"
#include "roundedrectangleobject.h"
#include "bezierobject.h"
#include "ellipseobject.h"
#include "polygonobject.h"
#include "polylineobject.h"
#include "linearrayobject.h"
#include "kivio_common.h"
#include "pen.h"
#include "groupobject.h"

namespace Kivio {

SmlObjectSaver::SmlObjectSaver()
{
  m_shapeTypeMap.insert(kstArc, "Arc");
  m_shapeTypeMap.insert(kstPie, "Pie");
  m_shapeTypeMap.insert(kstLineArray, "LineArray");
  m_shapeTypeMap.insert(kstPolyline, "Polyline");
  m_shapeTypeMap.insert(kstPolygon, "Polygon");
  m_shapeTypeMap.insert(kstBezier, "Bezier");
  m_shapeTypeMap.insert(kstRectangle, "Rectangle");
  m_shapeTypeMap.insert(kstRoundedRectangle, "RoundRectangle");
  m_shapeTypeMap.insert(kstEllipse, "Ellipse");
  m_shapeTypeMap.insert(kstOpenPath, "OpenPath");
  m_shapeTypeMap.insert(kstClosedPath, "ClosedPath");
  m_shapeTypeMap.insert(kstTextBox, "TextBox");
}

SmlObjectSaver::~SmlObjectSaver()
{
}

QDomElement SmlObjectSaver::saveObject(Object* object, QDomDocument& doc)
{
  QDomElement element;

  if(!object) {
    return element;
  }

  switch(object->type()) {
    case kstNone:
      break;

    case kstArc:
      break;

    case kstPie:
      break;

    case kstLineArray:
      break;

    case kstPolyline:
      break;

    case kstPolygon:
      break;

    case kstBezier:
      break;

    case kstRectangle:
      element = saveRectangleObject(static_cast<RectangleObject*>(object), doc);
      break;

    case kstRoundedRectangle:
      break;

    case kstEllipse:
      break;

    case kstOpenPath:
      break;

    case kstClosedPath:
      break;

    case kstTextBox:
      break;

    case kstGroup:
      element = saveGroupObject(static_cast<GroupObject*>(object), doc);
      break;

    case kstConnector:
      break;

    default:
      break;
  }

  return element;
}

QDomElement SmlObjectSaver::saveRectangleObject(RectangleObject* object, QDomDocument& doc)
{
  QDomElement objE = doc.createElement("KivioShape");
  XmlWriteString(objE, "type", "Rectangle");
  XmlWriteString(objE, "name", object->name());

  KoRect rect = object->boundingBox();
  XmlWriteFloat(objE, "x", rect.x());
  XmlWriteFloat(objE, "y", rect.y());
  XmlWriteFloat(objE, "w", rect.width());
  XmlWriteFloat(objE, "h", rect.height());

  objE.appendChild(saveBrush(object->brush(), doc));
  objE.appendChild(savePen(object->pen(), doc));

  return objE;
}

QDomElement SmlObjectSaver::saveGroupObject(GroupObject* object, QDomDocument& doc)
{
  QDomElement objE = doc.createElement("KivioShape");
  XmlWriteString(objE, "type", "Group");
  XmlWriteString(objE, "name", object->name());

  KoRect rect = object->boundingBox();
  XmlWriteFloat(objE, "x", rect.x());
  XmlWriteFloat(objE, "y", rect.y());
  XmlWriteFloat(objE, "w", rect.width());
  XmlWriteFloat(objE, "h", rect.height());

  QValueList<Object*> objectList = object->objectList();
  QValueList<Object*>::iterator itEnd = objectList.end();

  for(QValueList<Object*>::iterator it = objectList.begin(); it != itEnd; ++it) {
    objE.appendChild(saveObject(*it, doc));
  }

  return objE;
}

QDomElement SmlObjectSaver::saveBrush(const QBrush& brush, QDomDocument& doc)
{
  QDomElement brushE = doc.createElement("KivioFillStyle");

  XmlWriteColor(brushE, "color", brush.color());
  XmlWriteInt(brushE, "colorStyle", brush.style());

  return brushE;
}

QDomElement SmlObjectSaver::savePen(const Pen& pen, QDomDocument& doc)
{
  QDomElement penE = doc.createElement("KivioLineStyle");

  XmlWriteColor(penE, "color", pen.color());
  XmlWriteFloat(penE, "width", pen.pointWidth());
  XmlWriteInt(penE, "capStyle", pen.capStyle());
  XmlWriteInt(penE, "joinStyle", pen.joinStyle());
  XmlWriteInt(penE, "pattern", pen.style());

  return penE;
}

}
