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

#ifndef KSPREAD_VALIDITY_STORAGE
#define KSPREAD_VALIDITY_STORAGE

#include "RectStorage.h"
#include "Validity.h"

namespace KSpread
{

/**
 * \class ValidityStorage
 * \ingroup Storage
 * \ingroup Value
 * Stores cell validations.
 */
class ValidityStorage : public QObject, public RectStorage<Validity>
{
    Q_OBJECT
public:
    explicit ValidityStorage(Map* map) : QObject(map), RectStorage<Validity>(map) {}
    ValidityStorage(const ValidityStorage& other) : QObject(other.parent()), RectStorage<Validity>(other) {}

protected Q_SLOTS:
    virtual void triggerGarbageCollection() {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
    }
    virtual void garbageCollection() {
        RectStorage<Validity>::garbageCollection();
    }
};

} // namespace KSpread

#endif // KSPREAD_VALIDITY_STORAGE
