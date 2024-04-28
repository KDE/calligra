/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_TABLE_SHAPE_FACTORY
#define CALLIGRA_SHEETS_TABLE_SHAPE_FACTORY

#include <QVariantList>

#include <KoShapeFactoryBase.h>

class KoShape;

#define ClearMapId 65227212 // FIXME use number range allocated in KoDocumentResourceManager::DocumentResource

class TableShapePlugin : public QObject
{
    Q_OBJECT
public:
    TableShapePlugin(QObject *parent, const QVariantList &);
    ~TableShapePlugin()
    {
    }
};

class TableShapeFactory : public KoShapeFactoryBase
{
public:
    TableShapeFactory();
    ~TableShapeFactory();

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources) const override;
    KoShape *createShapeFromOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    bool supports(const KoXmlElement &element, KoShapeLoadingContext &context) const override;
};

#endif // CALLIGRA_SHEETS_TABLE_SHAPE_FACTORY
