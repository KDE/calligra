/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef ROWREPEATSTORAGE_H
#define ROWREPEATSTORAGE_H

#include <QMap>
#include <QRect>

#include "sheets_odf_export.h"

namespace Calligra
{
namespace Sheets
{

/**
  * Class to store row-repeat values, so on saving of a file we won't have to recalculate these entirely
  * but can just use the values as we know them from loading.
  * Current implementation doesn't have very optimal insertRows/removeRows functions, but the rest of
  * the methods are quite okay.
  */
class CALLIGRA_SHEETS_ODF_TEST_EXPORT RowRepeatStorage
{
public:
    RowRepeatStorage();
    void setRowRepeat(int firstRow, int rowRepeat);
    int rowRepeat(int row) const;
    int firstIdenticalRow(int row) const;
    void insertRows(int row, int count);
    void removeRows(int row, int count);
    void insertShiftDown(const QRect& rect);
    void insertShiftRight(const QRect& rect);
    void removeShiftUp(const QRect& rect);
    void removeShiftLeft(const QRect& rect);
    void splitRowRepeat(int row);
    void dump() const;
private:
    // m_data[i] = j means that rows [i-j+1 , i] are the same
    QMap<int, int> m_data;
};

} // namespace Sheets
} // namespace Calligra

#endif // ROWREPEATSTORAGE_H
