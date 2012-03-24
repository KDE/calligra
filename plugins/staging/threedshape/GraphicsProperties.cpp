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
#include "GraphicsProperties.h"

// Qt
#include <QString>

// KDE
#include <KDebug>

// Calligra
#include <KoXmlReader.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>
#include <KoShape.h>
#include <KoShapeSavingContext.h>

// Shape


GraphicsProperties::GraphicsProperties()
    : m_attributes(0)
    , m_fill(None)
    , m_fillColor(QColor())
    , m_secondaryFillColor(QColor())
{
}

GraphicsProperties::~GraphicsProperties()
{
}


bool GraphicsProperties::loadOdf(const KoXmlElement &properties)
{
    QString dummy;
    QColor  dummyColor;

    m_attributes = 0x0;

    // draw:fill
    dummy = properties.attributeNS(KoXmlNS::draw, "fill", "");
    if (!dummy.isEmpty()) {
        kDebug(31000) << "draw:fill" << dummy;
        m_attributes |= AttributeFill;
        if (dummy == "solid") {
            m_fill = Solid;
        }
        // FIXME: More fill modes here
    }

    // draw:fill-color
    dummy = properties.attributeNS(KoXmlNS::draw, "fill-color", "");
    if (!dummy.isEmpty()) {
        kDebug(31000) << "draw:fill-color" << dummy;
        dummyColor = QColor(dummy);
        if (dummyColor.isValid()) {
            m_fillColor = dummyColor;
            m_attributes |= AttributeFillColor;
        }
        else
            return false;
    }

    // draw:secondary-fill-color
    dummy = properties.attributeNS(KoXmlNS::draw, "secondary-fill-color", "");
    if (!dummy.isEmpty()) {
        kDebug(31000) << "draw:secondary-fill-color" << dummy;
        dummyColor = QColor(dummy);
        if (dummyColor.isValid()) {
            m_secondaryFillColor = dummyColor;
            m_attributes |= AttributeSecondaryFillColor;
        }
    }

    kDebug(31000) << "attributes:" << hex << m_attributes << dec;
    return true;
}

void GraphicsProperties::saveOdf(KoXmlWriter &writer) const
{
}
