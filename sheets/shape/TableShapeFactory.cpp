/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "TableShapeFactory.h"
#include "TableToolFactory.h"
#include <TableShape.h>

#include <KLocalizedString>
#include <KPluginFactory>

#include <KoDocumentResourceManager.h>
#include <KoIcon.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeRegistry.h>
#include <KoStyleStack.h>
#include <KoToolRegistry.h>
#include <KoXmlNS.h>

using namespace Calligra::Sheets;

K_PLUGIN_FACTORY_WITH_JSON(TableShapePluginFactory, "calligra_shape_spreadsheet.json", registerPlugin<TableShapePlugin>();)

TableShapePlugin::TableShapePlugin(QObject *parent, const QVariantList &)
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
