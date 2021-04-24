/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_DATABASE_STORAGE
#define KSPREAD_DATABASE_STORAGE

#include "Database.h"
#include "RectStorage.h"

namespace Calligra
{
namespace Sheets
{

/**
 * DatabaseStorage
 */
class DatabaseStorage : public QObject, public RectStorage<Database>
{
    Q_OBJECT
public:
    explicit DatabaseStorage(Map* map) : QObject(map), RectStorage<Database>(map) {}
    DatabaseStorage(const DatabaseStorage& other) : QObject(other.parent()), RectStorage<Database>(other) {}

protected Q_SLOTS:
    void triggerGarbageCollection() override {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
    }
    void garbageCollection() override {
        RectStorage<Database>::garbageCollection();
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_DATABASE_STORAGE
