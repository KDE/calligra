/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PictureShapeFactory.h"

#include "PictureDebug.h"
#include "PictureShape.h"
#include "PictureShapeConfigWidget.h"

#include <QBuffer>
#include <QByteArray>
#include <QImage>

#include "KoShapeBasedDocumentBase.h"
#include <KLocalizedString>
#include <KoDocumentResourceManager.h>
#include <KoIcon.h>
#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoOdfLoadingContext.h>
#include <KoProperties.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

PictureShapeFactory::PictureShapeFactory()
    : KoShapeFactoryBase(PICTURESHAPEID, i18n("Image"))
{
    setToolTip(i18n("Image shape that can display jpg, png etc."));
    setIconName(koIconName("x-shape-image"));
    setLoadingPriority(1);

    QList<QPair<QString, QStringList>> elementNamesList;
    elementNamesList.append(qMakePair(QString(KoXmlNS::draw), QStringList("image")));
    elementNamesList.append(qMakePair(QString(KoXmlNS::svg), QStringList("image")));
    setXmlElements(elementNamesList);
}

KoShape *PictureShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    PictureShape *defaultShape = new PictureShape();
    defaultShape->setShapeId(PICTURESHAPEID);
    if (documentResources) {
        defaultShape->setImageCollection(documentResources->imageCollection());
    }
    return defaultShape;
}

KoShape *PictureShapeFactory::createShape(const KoProperties *params, KoDocumentResourceManager *documentResources) const
{
    PictureShape *shape = static_cast<PictureShape *>(createDefaultShape(documentResources));
    if (params->contains("qimage")) {
        QImage image = params->property("qimage").value<QImage>();
        Q_ASSERT(!image.isNull());

        if (shape->imageCollection()) {
            KoImageData *data = shape->imageCollection()->createImageData(image);
            shape->setUserData(data);
            shape->setSize(data->imageSize());
            shape->update();
        }
    }
    return shape;
}

bool PictureShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    if (e.localName() == "image" && e.namespaceURI() == KoXmlNS::draw) {
        QString href = e.attribute("href");
        if (!href.isEmpty()) {
            // check the mimetype
            if (href.startsWith(QLatin1String("./"))) {
                href.remove(0, 2);
            }
            QString mimetype = context.odfLoadingContext().mimeTypeForPath(href);
            if (!mimetype.isEmpty()) {
                return mimetype.startsWith("image");
            } else {
                return (href.endsWith("bmp") || href.endsWith("jpg") || href.endsWith("gif") || href.endsWith("eps") || href.endsWith("png")
                        || href.endsWith("tif") || href.endsWith("tiff"));
            }
        } else {
            return !KoXml::namedItemNS(e, KoXmlNS::office, "binary-data").isNull();
        }
    }
    return false;
}

QList<KoShapeConfigWidgetBase *> PictureShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase *> panels;
    panels.append(new PictureShapeConfigWidget());
    return panels;
}

void PictureShapeFactory::newDocumentResourceManager(KoDocumentResourceManager *manager) const
{
    if (!manager->imageCollection())
        manager->setImageCollection(new KoImageCollection(manager));
}
