/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_TABLE_SHAPE_DEFERRED_FACTORY
#define CALLIGRA_SHEETS_TABLE_SHAPE_DEFERRED_FACTORY

#include <KoDeferredShapeFactoryBase.h>
#include <QVariantList>

class KoShapeFactoryBase;

class TableDeferredShapeFactory : public KoDeferredShapeFactoryBase
{
    Q_OBJECT
public:
    TableDeferredShapeFactory(QObject *parent, const QVariantList &);
    ~TableDeferredShapeFactory();

    virtual QString deferredPluginName()
    {
        return "spreadsheetshape-deferred";
    }

    KoShape *createDefaultShape(KoDocumentResourceManager *documentResources = 0) const;

private:
    KoShapeFactoryBase *m_stubFactory;
};

#endif // CALLIGRA_SHEETS_TABLE_SHAPE_DEFERRED_FACTORY
