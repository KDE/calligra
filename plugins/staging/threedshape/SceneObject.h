/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef THREEDSHAPE_H
#define THREEDSHAPE_H

// Qt
#include <QList>
#include <QObject>

// Calligra
#include <Ko3dScene.h>
#include <KoShapeContainer.h>

// Shape
#include "Object3D.h"

#define THREEDSHAPEID "ThreedShape"

class SceneObject : public Object3D //, public QObject
#if IMPLEMENT_AS_SHAPECONTAINER
    ,
                    public KoShapeContainer
#else
    ,
                    public KoShape
#endif
{
public:
    // Constructor

    // @param topLevel true if this is the top level scene
    // element. The top level element is the only one that should read
    // view parameters from the element.
    explicit SceneObject(Object3D *parent, bool topLevel = false);
    ~SceneObject() override;

    /// reimplemented from KoShapeContainer
    void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;

    // reimplemented from KoShape
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context) override;
    // reimplemented from KoShape
    void saveOdf(KoShapeSavingContext &context) const override;
    // reimplemented from KoShape
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    // reimplemented from KoShape
    void waitUntilReady(const KoViewConverter &converter, bool asynchronous) const override;

    // Really save the object.  See the explanation in Object3D.h.
    void saveObjectOdf(KoShapeSavingContext &context) const override;

    // getters
    Ko3dScene *threeDParams() const;

private:
    // View attributes
    bool m_topLevel;
    Ko3dScene *m_threeDParams; // Camera and rendering parameters plus lightsources.

    QList<Object3D *> m_objects;
};

#endif
