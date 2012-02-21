/* This file is part of the KDE project
 *
 * Copyright (C) 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

// Qt
#include <QColor>
#include <QVector3D>

// Calligra
#include <KoShape.h>


class KoXmlWriter;


class Lightsource
{
public:
    Lightsource(KoXmlElement &lightElem);
    ~Lightsource();

    void saveOdf(KoXmlWriter &writer);

    // getters
    QColor    diffuseColor() { return m_diffuseColor; }
    QVector3D direction()    { return m_direction;    }
    bool      enabled()      { return m_enabled;      }
    bool      specular()     { return m_specular;     }

private:
    QColor     m_diffuseColor;
    QVector3D  m_direction;
    bool       m_enabled;
    bool       m_specular;
};


#endif
