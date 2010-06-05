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

#ifndef KSPREAD_CONDITIONS_STORAGE
#define KSPREAD_CONDITIONS_STORAGE

#include "Condition.h"
#include "RectStorage.h"

namespace KSpread
{

/**
 * \class ConditionsStorage
 * \ingroup Storage
 * Stores conditional cell styles.
 */
class ConditionsStorage : public QObject, public RectStorage<Conditions>
{
    Q_OBJECT
public:
    explicit ConditionsStorage(Map* map) : QObject(map), RectStorage<Conditions>(map) {}
    ConditionsStorage(const ConditionsStorage& other) : QObject(other.parent()), RectStorage<Conditions>(other) {}

protected Q_SLOTS:
    virtual void triggerGarbageCollection() {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
    }
    virtual void garbageCollection() {
        RectStorage<Conditions>::garbageCollection();
    }
};

} // namespace KSpread

#endif // KSPREAD_CONDITIONS_STORAGE
