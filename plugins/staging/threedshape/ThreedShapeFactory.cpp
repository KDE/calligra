/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "ThreedShapeFactory.h"

// Qt
#include <QBuffer>
#include <QByteArray>
#include <QImage>

// KF5
#include <KLocalizedString>

// Calligra
#include <KoIcon.h>
#include <KoXmlNS.h>
// #include "KoShapeBasedDocumentBase.h"
#include <KoDocumentResourceManager.h>
#include <KoOdfLoadingContext.h>
#include <KoProperties.h>
#include <KoShapeLoadingContext.h>

// 3D Shape
#include "SceneObject.h"
// #include "ThreedShapeConfigWidget.h"
#include "ThreedDebug.h"

ThreedShapeFactory::ThreedShapeFactory()
    : KoShapeFactoryBase(THREEDSHAPEID, i18n("3D Scene"))
{
    setToolTip(i18n("Shape that displays a simple 3D scene."));
    // KIconLoader::global()->addAppDir("kchart");
    setIconName(koIconName("x-shape-3d"));
    setLoadingPriority(1);

    // Tell the shape loader which tag we can store
    QList<QPair<QString, QStringList>> elementNamesList;
    elementNamesList.append(qMakePair(QString(KoXmlNS::dr3d), QStringList("scene")));
    setXmlElements(elementNamesList);
}

bool ThreedShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);

    if (e.localName() == "scene" && e.namespaceURI() == KoXmlNS::dr3d) {
        return true;
    }

    return false;
}

KoShape *ThreedShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    Q_UNUSED(documentResources);

    debugThreed << "Creating a 3d shape";

    SceneObject *defaultShape = new SceneObject(nullptr, true);
    defaultShape->setShapeId(THREEDSHAPEID);

    return defaultShape;
}

KoShape *ThreedShapeFactory::createShape(const KoProperties *params, KoDocumentResourceManager *documentResources) const
{
    Q_UNUSED(params);

    SceneObject *shape = static_cast<SceneObject *>(createDefaultShape(documentResources));

    return shape;
}

QList<KoShapeConfigWidgetBase *> ThreedShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase *> result;

    return result;
}
