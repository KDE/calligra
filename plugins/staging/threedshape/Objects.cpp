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

// Own
#include "Objects.h"

// Qt
#include <QString>

// KDE
#include <KDebug>

// Calligra
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

// Shape
#include "utils.h"


// ================================================================
//                             Object3D


Object3D::Object3D()
{
}

Object3D::~Object3D()
{
}


bool Object3D::loadOdf(const KoXmlElement &objectElement)
{
    // FIXME: Load style, layer, Z-index, id.

    return true;
}

void Object3D::saveOdf(KoXmlWriter &writer) const
{
    // FIXME: Save style, layer, Z-index, id.
}


// ================================================================
//                             Sphere


Sphere::Sphere()
{
}

Sphere::~Sphere()
{
}


bool Sphere::loadOdf(const KoXmlElement &objectElement)
{
    Object3D::loadOdf(objectElement);

    QString dummy;
    dummy = objectElement.attributeNS(KoXmlNS::dr3d, "center", "(0 0 0)");
    m_center = odfToVector3D(dummy);
    dummy = objectElement.attributeNS(KoXmlNS::dr3d, "size", "(1 1 1)");
    m_size = odfToVector3D(dummy);

    kDebug(31000) << "sphere" << m_center << m_size;
    return true;
}

void Sphere::saveOdf(KoXmlWriter &writer) const
{
    writer.startElement("dr3d:sphere");

    Object3D::saveOdf(writer);

    writer.addAttribute("dr3d:center", QString("(%1 %2 %3)").arg(m_center.x())
                        .arg(m_center.y()).arg(m_center.z()));
    writer.addAttribute("dr3d:size", QString("(%1 %2 %3)").arg(m_size.x())
                        .arg(m_size.y()).arg(m_size.z()));

    writer.endElement(); // dr3d:sphere
}


// ================================================================
//                             Cube


Cube::Cube()
{
}

Cube::~Cube()
{
}


bool Cube::loadOdf(const KoXmlElement &objectElement)
{
    Object3D::loadOdf(objectElement);

    QString dummy;
    dummy = objectElement.attributeNS(KoXmlNS::dr3d, "min-edge", "(0 0 0)");
    m_minEdge = odfToVector3D(dummy);
    dummy = objectElement.attributeNS(KoXmlNS::dr3d, "max-edge", "(1 1 1)");
    m_maxEdge = odfToVector3D(dummy);

    kDebug(31000) << "cube" << m_minEdge << m_maxEdge;
    return true;
}

void Cube::saveOdf(KoXmlWriter &writer) const
{
    writer.startElement("dr3d:cube");

    Object3D::saveOdf(writer);

    writer.addAttribute("dr3d:min-edge", QString("(%1 %2 %3)").arg(m_minEdge.x())
                        .arg(m_minEdge.y()).arg(m_minEdge.z()));
    writer.addAttribute("dr3d:max-edge", QString("(%1 %2 %3)").arg(m_maxEdge.x())
                        .arg(m_maxEdge.y()).arg(m_maxEdge.z()));

    writer.endElement(); // dr3d:cube
}


