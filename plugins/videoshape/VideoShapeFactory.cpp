/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VideoShapeFactory.h"

#include "VideoCollection.h"
#include "VideoDebug.h"
#include "VideoShape.h"
#include "VideoShapeConfigWidget.h"

#include "KoShapeBasedDocumentBase.h"
#include <KoDocumentResourceManager.h>
#include <KoIcon.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

#include <KLocalizedString>

VideoShapeFactory::VideoShapeFactory()
    : KoShapeFactoryBase(VIDEOSHAPEID, i18n("Video"))
{
    setToolTip(i18n("Video, embedded or fullscreen"));
    setIconName(koIconName("video-x-generic"));
    setXmlElementNames(KoXmlNS::draw, QStringList("plugin"));
    setLoadingPriority(2);
}

KoShape *VideoShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    VideoShape *defaultShape = new VideoShape();
    defaultShape->setShapeId(VIDEOSHAPEID);
    if (documentResources) {
        Q_ASSERT(documentResources->hasResource(VideoCollection::ResourceId));
        QVariant vc = documentResources->resource(VideoCollection::ResourceId);
        defaultShape->setVideoCollection(static_cast<VideoCollection *>(vc.value<void *>()));
    }
    return defaultShape;
}

bool VideoShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    if (e.localName() != "plugin" || e.namespaceURI() != KoXmlNS::draw) {
        return false;
    }
    return e.attribute("mime-type") == "application/vnd.sun.star.media";
}

void VideoShapeFactory::newDocumentResourceManager(KoDocumentResourceManager *manager) const
{
    QVariant variant;
    variant.setValue<void *>(new VideoCollection(manager));
    manager->setResource(VideoCollection::ResourceId, variant);
}

QList<KoShapeConfigWidgetBase *> VideoShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase *> panels;
    panels.append(new VideoShapeConfigWidget());
    return panels;
}
