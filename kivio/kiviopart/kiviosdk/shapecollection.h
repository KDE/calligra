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
#ifndef KIVIOSHAPECOLLECTION_H
#define KIVIOSHAPECOLLECTION_H

#include <qstring.h>
#include <qvaluelist.h>

namespace Kivio {

class Object;

/**
 * Class the contains a collection of premade shapes
 */
class ShapeCollection
{
  public:
    ShapeCollection();
    ~ShapeCollection();

    void setId(const QString& newId) { m_id = newId; }
    QString id() const { return m_id; }
    void setName(const QString& newName) { m_name = newName; }
    QString name() const { return m_name; }

    void addShape(Object* shape);

  private:
    QString m_id;
    QString m_name;

    QValueList<Object*> m_shapeList;
};

}

#endif
