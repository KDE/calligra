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

#ifndef SCENE_H
#define SCENE_H

// Qt
#include <QColor>
#include <QVector3D>
#include <QString>
#include <QList>

// Calligra
#include <Ko3dScene.h>
#include <KoShape.h>

// Shape
#include "GraphicsProperties.h"
//#include "Lightsource.h"
#include "Objects.h"

class KoXmlWriter;
class KoShapeLoadingContext;
class KoShapeSavingContext;


class Scene
{
public:
    enum Projection {
        Parallel,
        Perspective
    };

    enum Shademode {
        Flat,
        Gouraud,
        Phong,
        Draft                   // Wireframe
    };

    Scene();
    ~Scene();

    bool loadOdf(const KoXmlElement &sceneElement, KoShapeLoadingContext &context);
    void saveOdf(KoShapeSavingContext &context) const;

    Ko3dScene *threeDParams() const;

private:
    // Scene style
    // FIXME: Use loading in KoShape instead.
    GraphicsProperties  m_sceneProperties;

    // Scene attributes
    Ko3dScene *m_threeDParams;    // Camera and rendering parameters plus lightsources.

    // The objects in the scene.
    QList<Object3D*>  m_objects; // value list. If necessary, we can make object3d shared.
};


#endif
