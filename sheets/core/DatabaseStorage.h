/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_DATABASE_STORAGE
#define KSPREAD_DATABASE_STORAGE

#include "sheets_core_export.h"

#include "Database.h"
#include "RectStorage.h"

namespace Calligra
{
namespace Sheets
{

/**
 * DatabaseStorage
 */
class CALLIGRA_SHEETS_CORE_EXPORT DatabaseStorage : public QObject, public RectStorage<Database>
{
    Q_OBJECT
public:
    explicit DatabaseStorage(MapBase* map) : QObject(map), RectStorage<Database>(map) {}
    DatabaseStorage(const DatabaseStorage& other) : QObject(other.parent()), RectStorage<Database>(other) {}

protected Q_SLOTS:
    void triggerGarbageCollection() override {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, &DatabaseStorage::garbageCollection);
    }
    void garbageCollection() override {
        RectStorage<Database>::garbageCollection();
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_DATABASE_STORAGE
