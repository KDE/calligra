/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_STORAGE_BASE
#define CALLIGRA_SHEETS_STORAGE_BASE

#include <QRect>

namespace Calligra
{
namespace Sheets
{

// Base class for all the storage classes. Used to simplify the logic in CellStorage

class StorageBase
{
public:
    StorageBase()
    {
        m_storingUndo = false;
    };
    virtual ~StorageBase() = default;

    virtual void insertColumns(int position, int number) = 0;
    virtual void removeColumns(int position, int number) = 0;
    virtual void insertRows(int position, int number) = 0;
    virtual void removeRows(int position, int number) = 0;
    virtual void removeShiftLeft(const QRect &rect) = 0;
    virtual void insertShiftRight(const QRect &rect) = 0;
    virtual void removeShiftUp(const QRect &rect) = 0;
    virtual void insertShiftDown(const QRect &rect) = 0;

    void storeUndo(bool store)
    {
        m_storingUndo = store;
    }

    virtual void resetUndo() = 0;

protected:
    bool m_storingUndo;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_STORAGE_BASE
