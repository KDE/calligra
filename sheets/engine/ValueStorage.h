/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus stefan.nikolaus @kdemail.net

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_VALUE_STORAGE
#define KSPREAD_VALUE_STORAGE

#include "PointStorage.h"
#include "Value.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \class ValueStorage
 * \ingroup Storage
 * \ingroup Value
 * Stores cell values.
 */
class ValueStorage : public PointStorage<Value>
{
public:
    ValueStorage()
        : PointStorage<Value>()
    {
    }

    ValueStorage(const PointStorage<Value> &o) // krazy:exclude=explicit
        : PointStorage<Value>(o)
    {
    }

    ValueStorage &operator=(const PointStorage<Value> &o)
    {
        PointStorage<Value>::operator=(o);
        return *this;
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_VALUE_STORAGE
