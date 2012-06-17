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
#include <KoShapeSavingContext.h>

// Shape
#include "utils.h"


// ================================================================
//                             Sphere


Sphere::Sphere()
    : KoShape()
{
}

Sphere::~Sphere()
{
}


void Sphere::paint(QPainter &painter, const KoViewConverter &converter,
                   KoShapePaintingContext &context)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
    Q_UNUSED(context);
}

bool Sphere::loadOdf(const KoXmlElement &objectElement, KoShapeLoadingContext &context)
{
    // Load style information.
    loadOdfAttributes(objectElement, context, OdfAllAttributes);

    QString dummy;
    dummy = objectElement.attributeNS(KoXmlNS::dr3d, "center", "(0 0 0)");
    m_center = odfToVector3D(dummy);
    dummy = objectElement.attributeNS(KoXmlNS::dr3d, "size", "(1 1 1)");
    m_size = odfToVector3D(dummy);

    kDebug(31000) << "Sphere:" << m_center << m_size;
    return true;
}

void Sphere::saveOdf(KoShapeSavingContext &context) const
{
    kDebug(31000) << "Saving Sphere:" << m_center << m_size;

    KoXmlWriter &writer = context.xmlWriter();
    writer.startElement("dr3d:sphere");

    saveOdfAttributes(context, OdfAllAttributes);

    writer.addAttribute("dr3d:center", QString("(%1 %2 %3)").arg(m_center.x())
                        .arg(m_center.y()).arg(m_center.z()));
    writer.addAttribute("dr3d:size", QString("(%1 %2 %3)").arg(m_size.x())
                        .arg(m_size.y()).arg(m_size.z()));

    writer.endElement(); // dr3d:sphere
}


// ================================================================
//                             Cube


Cube::Cube()
    : KoShape()
{
}

Cube::~Cube()
{
}


void Cube::paint(QPainter &painter, const KoViewConverter &converter,
                 KoShapePaintingContext &context)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
    Q_UNUSED(context);
}

bool Cube::loadOdf(const KoXmlElement &objectElement, KoShapeLoadingContext &context)
{
    // Load style information.
    loadOdfAttributes(objectElement, context, OdfAllAttributes);

    QString dummy;
    dummy = objectElement.attributeNS(KoXmlNS::dr3d, "min-edge", "(0 0 0)");
    m_minEdge = odfToVector3D(dummy);
    dummy = objectElement.attributeNS(KoXmlNS::dr3d, "max-edge", "(1 1 1)");
    m_maxEdge = odfToVector3D(dummy);

    kDebug(31000) << "Cube:" << m_minEdge << m_maxEdge;
    return true;
}

void Cube::saveOdf(KoShapeSavingContext &context) const
{
    kDebug(31000) << "Saving Cube:" << m_minEdge << m_maxEdge;

    KoXmlWriter &writer = context.xmlWriter();
    writer.startElement("dr3d:cube");

    saveOdfAttributes(context, OdfAllAttributes);

    writer.addAttribute("dr3d:min-edge", QString("(%1 %2 %3)").arg(m_minEdge.x())
                        .arg(m_minEdge.y()).arg(m_minEdge.z()));
    writer.addAttribute("dr3d:max-edge", QString("(%1 %2 %3)").arg(m_maxEdge.x())
                        .arg(m_maxEdge.y()).arg(m_maxEdge.z()));

    writer.endElement(); // dr3d:cube
}


