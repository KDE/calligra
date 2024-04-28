/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_CONDITIONS_STORAGE
#define KSPREAD_CONDITIONS_STORAGE

#include "Condition.h"
#include "sheets_core_export.h"

#include "engine/RectStorage.h"

namespace Calligra
{
namespace Sheets
{
class MapBase;

/**
 * \class ConditionsStorage
 * \ingroup Storage
 * Stores conditional cell styles.
 */
class CALLIGRA_SHEETS_CORE_EXPORT ConditionsStorage : public QObject, public RectStorage<Conditions>
{
    Q_OBJECT
public:
    explicit ConditionsStorage(MapBase *map)
        : QObject(map)
        , RectStorage<Conditions>(map)
    {
    }
    ConditionsStorage(const ConditionsStorage &other)
        : QObject(other.parent())
        , RectStorage<Conditions>(other)
    {
    }

protected Q_SLOTS:
    void triggerGarbageCollection() override
    {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, &ConditionsStorage::garbageCollection);
    }
    void garbageCollection() override
    {
        RectStorage<Conditions>::garbageCollection();
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_CONDITIONS_STORAGE
