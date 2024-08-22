/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "Object3D.h"

// Qt

// Calligra
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

// Shape
#include "ThreedDebug.h"
#include "utils.h"

// #define OdfObjectAttributes (OdfAllAttributes & ~(OdfGeometry | OdfTransformation))
#define OdfObjectAttributes (OdfAdditionalAttributes | OdfMandatories)

// ================================================================
//                             Object3D

Object3D::Object3D(Object3D *parent)
    : m_parent(parent)
{
}

Object3D::~Object3D() = default;

QString Object3D::transform()
{
    return m_transform3D;
}

Object3D *Object3D::parent()
{
    return m_parent;
}

bool Object3D::loadOdf(const KoXmlElement &objectElement, KoShapeLoadingContext &context)
{
    Q_UNUSED(context);

    m_transform3D = objectElement.attributeNS(KoXmlNS::dr3d, "transform", "");

    return true;
}

void Object3D::saveOdf(KoShapeSavingContext &context) const
{
    Q_UNUSED(context);
}
void Object3D::saveObjectOdf(KoShapeSavingContext &context) const
{
    if (!m_transform3D.isEmpty()) {
        KoXmlWriter &writer = context.xmlWriter();
        writer.addAttribute("dr3d:transform", m_transform3D);
    }
}
