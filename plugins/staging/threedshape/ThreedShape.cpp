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
#include "Lightsource.h"


ThreedShape::ThreedShape()
    : KoShape()
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
}

bool ThreedShape::loadOdf(const KoXmlElement &sceneElement, KoShapeLoadingContext &context)
{
    kDebug(31000) << "========================================================== Starting 3D Scene";
    kDebug(31000) <<"Loading ODF element: " << sceneElement.tagName();

    loadOdfAttributes(sceneElement, context, OdfAllAttributes);


    // Extract the attributes of dr3d:scene
    //
    // From the ODF 1.2 spec section 9.4.1:
    // The attributes that may be associated with the <dr3d:scene> element are:
    //  o Position, Size, Style, Layer, Z-Index, ID and Caption ID – see section 9.2.15
    //  o Text anchor, table background, draw end position – see section 9.2.16
    //  * Camera vectors
    //  * Projection
    //  * Distance
    //  * Focal length
    //  * Shadow slant
    //  * Shade mode
    //  * Ambient color
    //  * Lighting mode
    //
    // Example: 
    //    dr3d:vrp="(0 0 16885.7142857143)"
    //    dr3d:vpn="(0 0 14285.7142857143)"
    //    dr3d:projection="perspective"
    //    dr3d:distance="2.6cm"
    //    dr3d:focal-length="10cm"
    //    dr3d:shadow-slant="0"
    //    dr3d:shade-mode="gouraud"
    //    dr3d:ambient-color="#666666"
    //    dr3d:lighting-mode="false"

    // Camera attributes
    QString vrp          = sceneElement.attributeNS(KoXmlNS::dr3d, "vrp", "");
    QString vpn          = sceneElement.attributeNS(KoXmlNS::dr3d, "vpn", "");
    QString vup          = sceneElement.attributeNS(KoXmlNS::dr3d, "vup", "(0.0 0.0 1.0)");
    QString distance     = sceneElement.attributeNS(KoXmlNS::dr3d, "distance", "");
    QString projection   = sceneElement.attributeNS(KoXmlNS::dr3d, "projection", "perspective");
    QString focalLength  = sceneElement.attributeNS(KoXmlNS::dr3d, "focal-length", "");
    QString shadowSlant  = sceneElement.attributeNS(KoXmlNS::dr3d, "shadow-slant", "");
    QString ambientColor = sceneElement.attributeNS(KoXmlNS::dr3d, "ambient-color", "#888888");

    // Rendering attributes
    QString shadeMode    = sceneElement.attributeNS(KoXmlNS::dr3d, "shade-mode", "gouraud");
    QString lightingMode = sceneElement.attributeNS(KoXmlNS::dr3d, "lighting-mode", "");
    QString transform    = sceneElement.attributeNS(KoXmlNS::dr3d, "transform", "");

    kDebug(31000) << vrp << vpn << vup
                  << distance << projection << focalLength
                  << shadowSlant
                  << ambientColor
                  << shadeMode << lightingMode << transform;
    // FIXME

    // Get the scene itself, i.e. the elements in the scene
    // From the ODF 1.2 spec section 9.4.1:
    //
    // The elements that may be contained in the <dr3d:scene> element are:
    //  * Title (short accessible name) – see section 9.2.20.
    //  * Long description (in support of accessibility) – see section 9.2.20.
    //  * Light – see section 9.4.2.
    //  * Scene – see section 9.4.1.
    //  * Extrude – see section 9.4.5.
    //  * Sphere – see section 9.4.4.
    //  * Rotate – see section 9.4.6.
    //  * Cube – see section 9.4.3.
    KoXmlElement  elem;
    forEachElement(elem, sceneElement) {

        if (elem.localName() == "light" && elem.namespaceURI() == KoXmlNS::dr3d) {
            Lightsource  light(elem);
            kDebug(31000) << "  Light:" << light.diffuseColor() << light.direction()
                          << light.enabled() << light.specular();
        }
        else if (elem.localName() == "scene" && elem.namespaceURI() == KoXmlNS::dr3d) {
            // FIXME: Recursive!  How does this work?
        }
        else if (elem.localName() == "sphere" && elem.namespaceURI() == KoXmlNS::dr3d) {
            // Attributes:
            // dr3d:center
            // dr3d:size
            // + a number of other standard attributes
        }
        else if (elem.localName() == "cube" && elem.namespaceURI() == KoXmlNS::dr3d) {
            // Attributes:
            // dr3d:min-edge
            // dr3d:max-edge
            // + a number of other standard attributes
        }
        else if (elem.localName() == "rotate" && elem.namespaceURI() == KoXmlNS::dr3d) {
            // Attributes:
            // dr3d:
        }
        else if (elem.localName() == "extrude" && elem.namespaceURI() == KoXmlNS::dr3d) {
            // Attributes:
            // dr3d:
        }
    }


    return true;
}


void ThreedShape::waitUntilReady(const KoViewConverter &converter, bool asynchronous) const
{
}


#include <ThreedShape.moc>
