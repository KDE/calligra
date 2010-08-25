/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "TableShapeFactory.h"

#include <QStringList>
#include <QSharedPointer>

#include <kgenericfactory.h>
#include <klocale.h>

#include <KoResourceManager.h>
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>
#include <KoXmlNS.h>

#include <Map.h>

#include "TableShape.h"
#include "TableToolFactory.h"

using namespace KSpread;

K_EXPORT_COMPONENT_FACTORY(spreadsheetshape, KGenericFactory<TableShapePlugin>("TableShape"))

TableShapePlugin::TableShapePlugin(QObject * parent,  const QStringList&)
{
    KoShapeRegistry::instance()->add(new TableShapeFactory(parent));
    KoToolRegistry::instance()->add(new TableToolFactory(parent));
}


TableShapeFactory::TableShapeFactory(QObject* parent)
        : KoShapeFactoryBase(parent, TableShapeId, i18n("Table"))
{
    setToolTip(i18n("Table Shape"));
    setIcon("spreadsheetshape");
    setOdfElementNames(KoXmlNS::table, QStringList() << "table");
}

TableShapeFactory::~TableShapeFactory()
{
}

bool TableShapeFactory::supports(const KoXmlElement &element) const
{
    return (element.namespaceURI() == KoXmlNS::table && element.localName() == "table");
}

KoShape *TableShapeFactory::createDefaultShape(KoResourceManager *documentResources) const
{
    TableShape *shape = new TableShape();
    shape->setShapeId(TableShapeId);
    if (documentResources) {
        Q_ASSERT(documentResources->hasResource(MapResourceId));
        Map *map = static_cast<Map*>(documentResources->resource(MapResourceId).value<void*>());
        shape->setMap(map);
    }
    return shape;
}

void TableShapeFactory::newDocumentResourceManager(KoResourceManager *manager)
{
    if (manager->hasResource(MapResourceId)) return;
    // One spreadsheet map for all inserted tables to allow referencing cells among them.
    QVariant variant;
    Map* map = new Map();
    // Make the KoResourceManager manage this Map, since we cannot delete it ourselves
    map->setParent(manager);
    connect(manager, SIGNAL(destroyed()), map, SLOT(deleteLater()));
    variant.setValue<void*>(map);
    manager->setResource(MapResourceId, variant);
}

#include "TableShapeFactory.moc"
