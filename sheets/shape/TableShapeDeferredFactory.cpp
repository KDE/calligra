/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "TableShapeDeferredFactory.h"

// #include <QStringList>

// #include <KPluginFactory>

// #include <KoDocumentResourceManager.h>
// #include <KoToolRegistry.h>
// #include <KoShapeLoadingContext.h>
// #include <KoXmlNS.h>

// #include <Map.h>

// #include "TableShape.h"
// #include "TableToolFactory.h"
// #include "TableShapeFactory.h"

using namespace Calligra::Sheets;

K_PLUGIN_FACTORY_WITH_JSON(TableShapePluginFactory, "calligra_shape_spreadsheet-deferred.json", registerPlugin<TableDeferredShapeFactory>();)

TableDeferredShapeFactory::TableDeferredShapeFactory(QObject *parent, const QVariantList &)
    : KoDeferredShapeFactoryBase(parent)
{
    // only create the tool when this plugin gets loaded.
    KoToolRegistry::instance()->addDeferred(new TableToolFactory());

    m_stubFactory = qobject_cast<KoShapeFactoryBase *>(parent);
}

TableDeferredShapeFactory::~TableDeferredShapeFactory()
{
}

KoShape *TableDeferredShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    QList<KoDocumentResourceManager *> resourceManagers = m_stubFactory->documentResourceManagers();
    foreach (KoDocumentResourceManager *documentResources, resourceManagers) {
        if (!documentResources->hasResource(MapResourceId)) {
            // One spreadsheet map for all inserted tables to allow referencing cells among them.
            QVariant variant;
            Map *map = new Map();
            // Make the KoDocumentResourceManager manage this Map, since we cannot delete it ourselves
            map->setParent(documentResources);
            QObject::connect(documentResources, SIGNAL(destroyed()), map, SLOT(deleteLater()));
            variant.setValue<void *>(map);
            documentResources->setResource(MapResourceId, variant);
        }
    }

    TableShape *shape = new TableShape();
    shape->setShapeId(TableShapeId);
    if (documentResources) {
        Q_ASSERT(documentResources->hasResource(MapResourceId));
        Map *map = static_cast<Map *>(documentResources->resource(MapResourceId).value<void *>());
        shape->setMap(map);
    }
    return shape;
}

#include "TableShapeDeferredFactory.moc"
