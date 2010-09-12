/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
#ifndef ROWREPEATSTORAGE_H
#define ROWREPEATSTORAGE_H

#include <QMap>
#include <QRect>

#include "kspread_export.h"

namespace KSpread
{

/**
  * Class to store row-repeat values, so on saving of a file we won't have to recalculate these entirely
  * but can just use the values as we know them from loading.
  * Current implementation doesn't have very optimal insertRows/removeRows functions, but the rest of
  * the methods are quite okay.
  */
class KSPREAD_TEST_EXPORT RowRepeatStorage
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

} // namespace KSpread

#endif // ROWREPEATSTORAGE_H
