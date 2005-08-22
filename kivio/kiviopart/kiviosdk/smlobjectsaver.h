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
#ifndef KIVIOSMLOBJECTSAVER_H
#define KIVIOSMLOBJECTSAVER_H

#include <qmap.h>

#include "object.h"

class QDomDocument;
class QDomElement;
class QBrush;

namespace Kivio {

class RectangleObject;
class GroupObject;
class Pen;

/**
 * Class to save objects in SML format
 */
class SmlObjectSaver{
  public:
    SmlObjectSaver();
    ~SmlObjectSaver();

    QDomElement saveObject(Object* object, QDomDocument& doc);

  protected:
    QDomElement saveRectangleObject(RectangleObject* object, QDomDocument& doc);
    QDomElement saveGroupObject(GroupObject* object, QDomDocument& doc);

    QDomElement saveBrush(const QBrush& brush, QDomDocument& doc);
    QDomElement savePen(const Pen& pen, QDomDocument& doc);

  private:
    QMap<ShapeType, QString> m_shapeTypeMap;
};

}

#endif
