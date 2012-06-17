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
#include <KoXmlWriter.h>
#include <KoGenStyle.h>
#include <KoGenStyles.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>

// Shape
#include "utils.h"
#include "GraphicsProperties.h"

Scene::Scene()
    : Object3D()
    , m_sceneProperties()
    , m_threeDParams(0)
    , m_objects()
{
}

Scene::~Scene()
{
    qDeleteAll(m_objects);
    delete m_threeDParams;
}


Ko3dScene *Scene::threeDParams() const
{
    return m_threeDParams;
}


bool Scene::loadOdf(const KoXmlElement &sceneElement, KoShapeLoadingContext &context)
{
    QString dummy;

    // FIXME: Get the style and load it.
    const KoOdfStylesReader &styles    = context.odfLoadingContext().stylesReader();
    const QString            styleName = sceneElement.attributeNS(KoXmlNS::draw, "style-name", QString());
    const KoXmlElement      *styleElement = styles.findStyle(styleName, "graphic");

    kDebug(31000) << styleName << styleElement;

    // Get the 
    m_sceneProperties.clear();
    if (styleElement && !styleElement->isNull()) {
        kDebug(31000) << "loading style" << styleName;

        // Look up graphic-properties in the style.
        const KoXmlElement  properties = KoXml::namedItemNS(*styleElement, KoXmlNS::style, "graphic-properties");
        kDebug(31000) << "isNull:" << properties.isNull();
        if (!properties.isNull())
            m_sceneProperties.loadOdf(properties);
    }
    
    // Load the scene attributes.
    m_threeDParams = load3dScene(sceneElement);

    // Load the child elements, i.e the scene itself.

    // From the ODF 1.1 spec section 9.4.1:
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
    //
    // The lights are skipped here, they are taken care of by the call
    // to loadScene() above.
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
            sphere->loadOdf(elem);
            m_objects.append(sphere);
        }
        else if (elem.localName() == "cube" && elem.namespaceURI() == KoXmlNS::dr3d) {
            // Attributes:
            // dr3d:min-edge
            // dr3d:max-edge
            // + a number of other standard attributes
            Cube  *cube = new Cube();
            cube->loadOdf(elem);
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

void Scene::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter &writer = context.xmlWriter();

    // 0. Scene style
    // FIXME: This doesn't work because the shape itself has its own style with its own stylename.
    KoGenStyle   sceneStyle(KoGenStyle::GraphicAutoStyle, "graphic" /*, m_sceneProperties.parent()*/);
    m_sceneProperties.saveOdf(sceneStyle);
    QString  styleName = context.mainStyles().insert(sceneStyle, "gr");
    writer.addAttribute("draw:style-name", styleName);

    // Write scene attributes
    if (m_threeDParams)
        m_threeDParams->saveOdfAttributes(writer);

    // 2.1 Light sources
    if (m_threeDParams)
        m_threeDParams->saveOdfChildren(writer);

    // 2.2 Objects in the scene
    foreach (const Object3D *object, m_objects) {
        object->saveOdf(writer);
    }
}
