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

#include <kpluginfactory.h>
#include <KLocalizedString>

#include <KoIcon.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeRegistry.h>
#include <KoToolRegistry.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoXmlNS.h>
#include <KoStyleStack.h>
#include <KoDocumentBase.h>

#include <Map.h>
#include <TableShape.h>
#include <TableToolFactory.h>

using namespace Calligra::Sheets;

K_PLUGIN_FACTORY_WITH_JSON(TableShapePluginFactory, "calligra_shape_spreadsheet.json",
                           registerPlugin<TableShapePlugin>();)

TableShapePlugin::TableShapePlugin(QObject * parent, const QVariantList&)
{
    Q_UNUSED(parent);
    KoShapeRegistry::instance()->add(new TableShapeFactory());
}


TableShapeFactory::TableShapeFactory()
    : KoShapeFactoryBase(TableShapeId, i18n("Spreadsheet"))
{
    setToolTip(i18n("Spreadsheet Shape"));
    setIconName(koIconName("spreadsheetshape"));
    setXmlElementNames("urn:oasis:names:tc:opendocument:xmlns:drawing:1.0", QStringList("object"));

    KoToolRegistry::instance()->add(new TableToolFactory());
}

TableShapeFactory::~TableShapeFactory()
{
}

KoShape *TableShapeFactory::createDefaultShape(KoDocumentResourceManager *documentResources) const
{
    TableShape *shape = new TableShape(documentResources, documentResources->odfDocument());
    shape->setShapeId(TableShapeId);
    if (documentResources && documentResources->boolResource(ClearMapId)) {
        shape->clear();
    }
    return shape;
}

KoShape *TableShapeFactory::createShapeFromOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    KoDocumentResourceManager *resource = context.documentResourceManager();
    resource->setResource(ClearMapId, true);
    KoShape *shape = createDefaultShape(resource);
    resource->clearResource(ClearMapId);

    context.odfLoadingContext().styleStack().save();
    bool loaded = shape->loadOdf(element, context);
    context.odfLoadingContext().styleStack().restore();

    if (!loaded) {
        delete shape;
        return nullptr;
    }
    return shape;
}

bool TableShapeFactory::supports(const KoXmlElement &element, KoShapeLoadingContext &context) const
{
    if (element.namespaceURI() == KoXmlNS::office && element.localName() == "spreadsheet") {
        return true;
    }
    if (element.namespaceURI() == KoXmlNS::draw && element.tagName() == "object") {
        QString href = element.attribute("href");
        if (!href.isEmpty()) {
            // check the mimetype
            if (href.startsWith(QLatin1String("./"))) {
                href.remove(0, 2);
            }
            const QString mimetype = context.odfLoadingContext().mimeTypeForPath(href);
            return mimetype.isEmpty() || mimetype == "application/vnd.oasis.opendocument.spreadsheet";
        }
    }
    return false;
}

#include "TableShapeFactory.moc"
