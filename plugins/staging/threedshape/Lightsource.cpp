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
#include "Lightsource.h"

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


Lightsource::Lightsource()
{
}

Lightsource::~Lightsource()
{
}


bool Lightsource::loadOdf(const KoXmlElement &lightElement)
{
    m_diffuseColor = QColor(lightElement.attributeNS(KoXmlNS::dr3d, "diffuse-color", "#ffffff"));
    QString direction = lightElement.attributeNS(KoXmlNS::dr3d, "direction", "");
    m_direction    = odfToVector3D(direction);
    m_enabled      = (lightElement.attributeNS(KoXmlNS::dr3d, "enabled", "") == "true");
    m_specular     = (lightElement.attributeNS(KoXmlNS::dr3d, "specular", "") == "true");

    return true;
}

void Lightsource::saveOdf(KoXmlWriter &writer) const
{
    writer.startElement("dr3d:light");

    writer.addAttribute("dr3d:diffuse-color", m_diffuseColor.name());
    writer.addAttribute("dr3d:direction", QString("(%1 %2 %3)").arg(m_direction.x())
                        .arg(m_direction.y()).arg(m_direction.z()));
    writer.addAttribute("dr3d:enabled", m_enabled);
    writer.addAttribute("dr3d:specular", m_specular);

    writer.endElement(); // dr3d:light
}
