/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_VALIDITY_STORAGE
#define KSPREAD_VALIDITY_STORAGE

#include "RectStorage.h"
#include "Validity.h"

namespace Calligra
{
namespace Sheets
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
    explicit ValidityStorage(MapBase *map)
        : QObject(map)
        , RectStorage<Validity>(map)
    {
    }
    ValidityStorage(const ValidityStorage &other)
        : QObject(other.parent())
        , RectStorage<Validity>(other)
    {
    }

protected Q_SLOTS:
    void triggerGarbageCollection() override
    {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, &ValidityStorage::garbageCollection);
    }
    void garbageCollection() override
    {
        RectStorage<Validity>::garbageCollection();
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_VALIDITY_STORAGE
