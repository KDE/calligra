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
#include "Ko3dScene.h"

// Qt
#include <QString>

// KDE
#include <KDebug>

// Calligra
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>


static QVector3D odfToVector3D(QString &string);


// ----------------------------------------------------------------
//                         Lightsource


Ko3dScene::Lightsource::Lightsource()
{
}

Ko3dScene::Lightsource::~Lightsource()
{
}


bool Ko3dScene::Lightsource::loadOdf(const KoXmlElement &lightElement)
{
    m_diffuseColor = QColor(lightElement.attributeNS(KoXmlNS::dr3d, "diffuse-color", "#ffffff"));
    QString direction = lightElement.attributeNS(KoXmlNS::dr3d, "direction", "");
    m_direction    = odfToVector3D(direction);
    m_enabled      = (lightElement.attributeNS(KoXmlNS::dr3d, "enabled", "") == "true");
    m_specular     = (lightElement.attributeNS(KoXmlNS::dr3d, "specular", "") == "true");

    return true;
}

void Ko3dScene::Lightsource::saveOdf(KoXmlWriter &writer) const
{
    writer.startElement("dr3d:light");

    writer.addAttribute("dr3d:diffuse-color", m_diffuseColor.name());
    writer.addAttribute("dr3d:direction", QString("(%1 %2 %3)").arg(m_direction.x())
                        .arg(m_direction.y()).arg(m_direction.z()));
    writer.addAttribute("dr3d:enabled", m_enabled);
    writer.addAttribute("dr3d:specular", m_specular);

    writer.endElement(); // dr3d:light
}


// ----------------------------------------------------------------
//                         Ko3dScene


Ko3dScene::Ko3dScene()
{
}

Ko3dScene::~Ko3dScene()
{
}


bool Ko3dScene::loadOdf(const KoXmlElement &sceneElement)
{
    QString dummy;

    // 1. Load the scene attributes.

    // Camera attributes
    dummy = sceneElement.attributeNS(KoXmlNS::dr3d, "vrp", "");
    m_vrp = odfToVector3D(dummy);
    dummy = sceneElement.attributeNS(KoXmlNS::dr3d, "vpn", "");
    m_vpn = odfToVector3D(dummy);
    dummy = sceneElement.attributeNS(KoXmlNS::dr3d, "vup", "(0.0 0.0 1.0)");
    m_vup = odfToVector3D(dummy);

    dummy = sceneElement.attributeNS(KoXmlNS::dr3d, "projection", "perspective");
    if (dummy == "parallel")
        m_projection = Parallel;
    else
        m_projection = Perspective;

    m_distance     = sceneElement.attributeNS(KoXmlNS::dr3d, "distance", "");
    m_focalLength  = sceneElement.attributeNS(KoXmlNS::dr3d, "focal-length", "");
    m_shadowSlant  = sceneElement.attributeNS(KoXmlNS::dr3d, "shadow-slant", "");
    m_ambientColor = QColor(sceneElement.attributeNS(KoXmlNS::dr3d, "ambient-color", "#888888"));

    // Rendering attributes
    dummy = sceneElement.attributeNS(KoXmlNS::dr3d, "shade-mode", "gouraud");
    if (dummy == "flat")
        m_shadeMode = Flat;
    else if (dummy == "phong")
        m_shadeMode = Phong;
    else if (dummy == "draft")
        m_shadeMode = Draft;
    else
        m_shadeMode = Gouraud;

    m_lightingMode = (sceneElement.attributeNS(KoXmlNS::dr3d, "lighting-mode", "") == "true");
    m_transform = sceneElement.attributeNS(KoXmlNS::dr3d, "transform", "");

    // 2. Load the light sources.

    // From the ODF 1.1 spec section 9.4.1:
    KoXmlElement  elem;
    forEachElement(elem, sceneElement) {

        if (elem.localName() == "light" && elem.namespaceURI() == KoXmlNS::dr3d) {
            Lightsource  light;
            light.loadOdf(elem);
            m_lights.append(light);

#if 0
            Lightsource  &l = m_lights.back();
            //kDebug(31000) << "  Light:" << l.diffuseColor() << l.direction()
            //              << l.enabled() << l.specular();
#endif
        }

    }

    //kDebug(31000) << "Lights:" << m_lights.size() << "Objects:" << m_objects.size();

    return true;
}

void Ko3dScene::saveOdfAttributes(KoXmlWriter &writer) const
{
    // 1. Write scene attributes
    // Camera attributes
    writer.addAttribute("dr3d:vrp", QString("(%1 %2 %3)").arg(m_vrp.x())
                        .arg(m_vrp.y()).arg(m_vrp.z()));
    writer.addAttribute("dr3d:vpn", QString("(%1 %2 %3)").arg(m_vpn.x())
                        .arg(m_vpn.y()).arg(m_vpn.z()));
    writer.addAttribute("dr3d:vup", QString("(%1 %2 %3)").arg(m_vup.x())
                        .arg(m_vup.y()).arg(m_vup.z()));

    writer.addAttribute("dr3d:projection", (m_projection == Parallel) ? "parallel" : "perspective");

    writer.addAttribute("dr3d:distance",     m_distance);
    writer.addAttribute("dr3d:focal-length", m_focalLength);
    writer.addAttribute("dr3d:shadow-slant", m_shadowSlant);

    // Rendering attributes
    writer.addAttribute("dr3d:ambient-color", m_ambientColor.name());
    switch (m_shadeMode) {
    case Flat:
        writer.addAttribute("dr3d:shade-mode", "flat");
        break;
    case Phong:
        writer.addAttribute("dr3d:shade-mode", "phong");
        break;
    case Draft:
        writer.addAttribute("dr3d:shade-mode", "draft");
        break;
    case Gouraud:
    default:
        writer.addAttribute("dr3d:shade-mode", "gouraud");
        break;
    }

    writer.addAttribute("dr3d:lighting-mode", m_lightingMode);
    writer.addAttribute("dr3d:transform",     m_transform);
}


void Ko3dScene::saveOdfChildren(KoXmlWriter &writer) const
{
    // Write light sources.
    foreach (const Lightsource &light, m_lights) {
        light.saveOdf(writer);
    }
}


// ----------------------------------------------------------------
//                         Static functions


QVector3D odfToVector3D(QString &string)
{
    // The string comes into this function in the form "(0 3.5 0.3)".
    QStringList elements = string.mid(1, string.size() - 2).split(' ', QString::SkipEmptyParts);
    if (elements.size() != 3)
        return QVector3D(0, 0, 1);
    else
        return QVector3D(elements[0].toDouble(), elements[1].toDouble(), elements[2].toDouble());
}
