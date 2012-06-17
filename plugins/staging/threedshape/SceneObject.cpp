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
#include "SceneObject.h"

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
#include "SceneObject.h"


SceneObject::SceneObject(bool topLevel)
    : Object3D()
    , m_topLevel(topLevel)
    , m_threeDParams(0)
{
    m_elementName = static_cast<char*>("dr3d:scene");
}

SceneObject::~SceneObject()
{
    delete m_threeDParams;
}

void SceneObject::paint(QPainter &painter, const KoViewConverter &converter,
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


void SceneObject::saveOdf(KoShapeSavingContext &context) const
{
    Object3D::saveOdf(context);

    // Note that Object3D::saveOdf() already has started the element.
    KoXmlWriter &writer = context.xmlWriter();
    if (m_topLevel && m_threeDParams)
        m_threeDParams->saveOdfAttributes(writer);

    // Writes the attributes and children of the dr3d:scene element.
    //m_scene.saveOdf(context);
    // Write scene attributes
    if (m_threeDParams)
        m_threeDParams->saveOdfAttributes(writer);

    // 2.1 Light sources
    if (m_topLevel && m_threeDParams)
        m_threeDParams->saveOdfChildren(writer);

    // 2.2 Objects in the scene
    foreach (const Object3D *object, m_objects) {
        object->saveOdf(context);
    }

    writer.endElement(); // dr3d:scene
}

bool SceneObject::loadOdf(const KoXmlElement &sceneElement, KoShapeLoadingContext &context)
{
    // Load style information.
    Object3D::loadOdf(sceneElement, context);

    // Load the view parameters.
    if (m_topLevel) {
        m_threeDParams = load3dScene(sceneElement);
    }

    // Load the child elements, i.e the scene itself.

    // From the ODF 1.1 spec section 9.4.1:
    //
    // The elements that may be contained in the <dr3d:scene> element are:
    //  * Title (short accessible name) – see section 9.2.20.
    //  * Long description (in support of accessibility) – see section 9.2.20.
    //  * Light – see section 9.4.2. (handled by Ko3DScene)
    //
    //  * Scene – see section 9.4.1.     [All of these can be 0 or more.]
    //  * Extrude – see section 9.4.5.
    //  * Sphere – see section 9.4.4.
    //  * Rotate – see section 9.4.6.
    //  * Cube – see section 9.4.3.
    //
    // The lights are skipped here, they are taken care of by the call
    // to load3dScene() above.
    KoXmlElement  elem;
    forEachElement(elem, sceneElement) {

        if (elem.localName() == "scene" && elem.namespaceURI() == KoXmlNS::dr3d) {
            // FIXME: Recursive!  How does this work?
        }
        else if (elem.localName() == "sphere" && elem.namespaceURI() == KoXmlNS::dr3d) {
            // Attributes:
            // dr3d:center
            // dr3d:size
            // + a number of other standard attributes
            Sphere  *sphere = new Sphere();
            sphere->loadOdf(elem, context);
            m_objects.append(sphere);
        }
        else if (elem.localName() == "cube" && elem.namespaceURI() == KoXmlNS::dr3d) {
            // Attributes:
            // dr3d:min-edge
            // dr3d:max-edge
            // + a number of other standard attributes
            Cube  *cube = new Cube();
            cube->loadOdf(elem, context);
            m_objects.append(cube);
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

    kDebug(31000) << "Objects:" << m_objects.size();

    return true;
}


void SceneObject::waitUntilReady(const KoViewConverter &converter, bool asynchronous) const
{
}


#include <SceneObject.moc>
