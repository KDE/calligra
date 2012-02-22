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
#include "Scene.h"

// Qt
#include <QString>

// KDE
#include <KDebug>

// Calligra
#include <KoXmlReader.h>
#include <KoXmlNS.h>
//#include <KoXmlWriter.h>

// Shape
#include "utils.h"


Scene::Scene(const KoXmlElement &sceneElement)
{
    QString dummy;

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

    //m_diffuseColor = QColor(lightElem.attributeNS(KoXmlNS::dr3d, "diffuse-color", "#ffffff"));
    //QString direction = lightElem.attributeNS(KoXmlNS::dr3d, "direction", "");
    //m_direction    = stringToVector(direction);
    //m_enabled      = (lightElem.attributeNS(KoXmlNS::dr3d, "enabled", "") == "true");
    //m_specular     = (lightElem.attributeNS(KoXmlNS::dr3d, "specular", "") == "true");
}

Scene::~Scene()
{
}


void Scene::saveOdf(KoXmlWriter &writer)
{
}
