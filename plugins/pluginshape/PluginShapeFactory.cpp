/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Vidhyapria  Arunkumar <vidhyapria.arunkumar@nokia.com>
 * Contact: Amit Aggarwal <amit.5.aggarwal@nokia.com>
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>

 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "PluginShapeFactory.h"

#include "PluginShape.h"

#include "KoShapeBasedDocumentBase.h"
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

#include <KLocalizedString>

PluginShapeFactory::PluginShapeFactory()
    : KoShapeFactoryBase(PLUGINSHAPEID, i18n("Plugin Placeholder"))
{
    setToolTip(i18n("Plugin Placeholder, embedded or fullscreen"));
    // setIcon("video-x-generic");
    setXmlElementNames(KoXmlNS::draw, QStringList("plugin"));
    setLoadingPriority(1);
    setHidden(true);
}

KoShape *PluginShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    Q_UNUSED(documentResources);
    PluginShape *defaultShape = new PluginShape();
    defaultShape->setShapeId(PLUGINSHAPEID);
    return defaultShape;
}

bool PluginShapeFactory::supports(const KoXmlElement &e, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    return e.localName() == "plugin" && e.namespaceURI() == KoXmlNS::draw;
}
