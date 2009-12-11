/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus stefan.nikolaus@kdemail.net

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

#ifndef KSPREAD_DATABASE_STORAGE
#define KSPREAD_DATABASE_STORAGE

#include "Database.h"
#include "RectStorage.h"

namespace KSpread
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
    virtual void triggerGarbageCollection() {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
    }
    virtual void garbageCollection() {
        RectStorage<Database>::garbageCollection();
    }
};

} // namespace KSpread

#endif // KSPREAD_DATABASE_STORAGE
