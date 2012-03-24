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
#include "ThreedShape.h"

// Qt
#include <QPainter>
#include <QTimer>
#include <QPixmapCache>
#include <QtCore/QBuffer>

// KDE
#include <KDebug>

// Calligra
#include <KoViewConverter.h>
#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoStoreDevice.h>
#include <KoUnit.h>
#include <KoGenStyle.h>
#include <KoFilterEffectStack.h>

// 3D shape
#include "Scene.h"
#include "Lightsource.h"


ThreedShape::ThreedShape()
    : KoShape()
    , m_scene()
{
}

ThreedShape::~ThreedShape()
{
}

void ThreedShape::paint(QPainter &painter, const KoViewConverter &converter,
                        KoShapePaintingContext &context)
{
    //painter.setPen(QPen(QColor(172, 196, 206)));
    painter.setPen(QPen(QColor(0, 0, 0)));

#if 1
    painter.drawRect(converter.documentToView(QRectF(QPoint(0, 0), size())));
#else
    QRectF rect = converter.documentToView(boundingRect());
    QRectF rect = converter.documentToView(QRectF(QPoint(0, 0), size()));
    painter.drawRect(rect);
#endif
    kDebug(31000) << "boundingRect: " << boundingRect();
    kDebug(31000) << "outlineRect: " << outlineRect();

#if 0 // Taken from the vector shape
    QRectF  rect(QPointF(0,0), m_size);
    painter.save();

    // Draw a simple cross in a rectangle just to indicate that there is something here.
    painter.setPen(QPen(QColor(172, 196, 206)));
    painter.drawRect(rect);
    painter.drawLine(rect.topLeft(), rect.bottomRight());
    painter.drawLine(rect.bottomLeft(), rect.topRight());

    painter.restore();
#endif
}


void ThreedShape::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter &writer = context.xmlWriter();

    writer.startElement("dr3d:scene");
    saveOdfAttributes(context, OdfAllAttributes);

    // Writes the attributes and children of the dr3d:scene element.
    m_scene.saveOdf(writer);

    writer.endElement(); // dr3d:scene
}

bool ThreedShape::loadOdf(const KoXmlElement &sceneElement, KoShapeLoadingContext &context)
{
    kDebug(31000) << "========================================================== Starting 3D Scene";
    kDebug(31000) <<"Loading ODF element: " << sceneElement.tagName();

    loadOdfAttributes(sceneElement, context, OdfAllAttributes);

    bool result = m_scene.loadOdf(sceneElement, context);
    kDebug(31000) << "Camera:" << m_scene.vrp() << m_scene.vpn() << m_scene.vup()
                  << m_scene.distance() << m_scene.projection() << m_scene.focalLength()
                  << "\nRendering:" << m_scene.shadowSlant()
                  << m_scene.ambientColor()
                  << m_scene.shadeMode() << m_scene.lightingMode() << m_scene.transform();

    return result;
}


void ThreedShape::waitUntilReady(const KoViewConverter &converter, bool asynchronous) const
{
}


#include <ThreedShape.moc>
