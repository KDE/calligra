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
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

#include <Map.h>
#include <TableShape.h>

using namespace Calligra::Sheets;

K_PLUGIN_FACTORY_WITH_JSON(TableShapePluginFactory, "calligra_shape_spreadsheet.json",
                           registerPlugin<TableShapePlugin>();)

TableShapePlugin::TableShapePlugin(QObject * parent, const QVariantList&)
{
    Q_UNUSED(parent);
    KoShapeRegistry::instance()->add(new TableShapeFactory());
}


TableShapeFactory::TableShapeFactory()
    : KoShapeFactoryBase(TableShapeId, i18n("Spreadsheet"), "spreadsheetshape-deferred")
{
    setToolTip(i18n("Spreadsheet Shape"));
    setIconName(koIconName("spreadsheetshape"));
    setXmlElementNames(KoXmlNS::table, QStringList() << "table");
}

TableShapeFactory::~TableShapeFactory()
{
}

bool TableShapeFactory::supports(const KoXmlElement &element, KoShapeLoadingContext &context) const
{
    Q_UNUSED(context);
    return (element.namespaceURI() == KoXmlNS::table && element.localName() == "table");
}

#include "TableShapeFactory.moc"
