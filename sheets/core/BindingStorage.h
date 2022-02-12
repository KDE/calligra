/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_BINDING_STORAGE
#define KSPREAD_BINDING_STORAGE

#include "Binding.h"
#include "engine/RectStorage.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \class BindingStorage
 * \ingroup Storage
 * Stores cell ranges acting as data sources.
 */
class BindingStorage : public QObject, public RectStorage<Binding>
{
    Q_OBJECT
public:
    explicit BindingStorage(MapBase* map) : QObject(), RectStorage<Binding>(map) {}
    BindingStorage(const BindingStorage& other) : QObject(other.parent()), RectStorage<Binding>(other) {}

protected Q_SLOTS:
    void triggerGarbageCollection() override {
        QTimer::singleShot(g_garbageCollectionTimeOut, this, SLOT(garbageCollection()));
    }
    void garbageCollection() override {
        RectStorage<Binding>::garbageCollection();
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_BINDING_STORAGE
