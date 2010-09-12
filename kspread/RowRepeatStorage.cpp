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
#include "RowRepeatStorage.h"

#include <QList>
#include <QPair>
#include <qdebug.h>

#include "kspread_limits.h"

using namespace KSpread;

RowRepeatStorage::RowRepeatStorage()
{
}

void RowRepeatStorage::dump() const
{
    for (QMap<int, int>::const_iterator it = m_data.begin(); it != m_data.end(); ++it) {
        qDebug() << "[" << (it.key() - it.value() + 1) << it.key() << "] =" << it.value();
    }
}

void RowRepeatStorage::setRowRepeat(int firstRow, int rowRepeat)
{
   const int lastRow = firstRow + rowRepeat - 1;
   // see if m_data contains a range that includes firstRow
   QMap<int, int>::iterator it = m_data.lowerBound(firstRow);
   typedef QPair<int, int> intPair;
   QList<intPair> newRanges;
   // returns first range that ends at or after firstRow
   if (it != m_data.end()) {
       while (it != m_data.end() && (it.key() - it.value() + 1) <= lastRow) {
           if ((it.key() - it.value() + 1) < firstRow) {
               // starts before the new range, so we want to change the row repeat value of this range
               // but since the key is the end of the range, we can only remove it and re-insert it later
               newRanges.append(qMakePair(firstRow - 1, it.value() - (it.key() - firstRow + 1)));
               if (it.key() > lastRow) {
                   // ends after the new range, so also adjust the end
                   it.value() = it.key() - lastRow;
                   ++it;
               } else {
                   it = m_data.erase(it);
               }
           } else {
               // starts inside the new range, ends in or after the new range
               if (it.key() <= lastRow) {
                   // ends inside the new range, so just remove
                   it = m_data.erase(it);
               } else {
                   // ends after the new range, adjust and go to the next
                   it.value() = it.key() - lastRow;
                   ++it;
               }
           }
       }
   }

   // finally set the new range of row-repeat values
   if (rowRepeat != 1)
       m_data[lastRow] = rowRepeat;

   foreach (const intPair& p, newRanges) {
       if (p.second > 1) m_data[p.first] = p.second;
   }
}

int RowRepeatStorage::rowRepeat(int row) const
{
    // first range that ends at or after row
    QMap<int, int>::const_iterator it = m_data.lowerBound(row);
    // not found? default value = 1
    if (it == m_data.end()) return 1;
    // otherwise, see if row is actually inside the range
    if (it.key() - it.value() + 1 <= row) {
        return it.value();
    }
    return 1;
}

int RowRepeatStorage::firstIdenticalRow(int row) const
{
    // first range that ends at or after row
    QMap<int, int>::const_iterator it = m_data.lowerBound(row);
    // not found? default value = row
    if (it == m_data.end()) return row;
    // otherwise, see if row is actually inside the range
    if (it.key() - it.value() + 1 <= row) {
        return it.key() - it.value() + 1;
    }
    return row;
}

void RowRepeatStorage::insertRows(int row, int count)
{
    typedef QPair<int, int> intPair;
    QList<intPair> newRanges;

    // first range that ends at or after row
    QMap<int, int>::iterator it = m_data.lowerBound(row);
    while (it != m_data.end()) {
        if (it.key() - it.value() + 1 < row) {
            // starts before the newly inserted rows, so split it up
            newRanges.append(qMakePair(row-1, row - it.key() + it.value() - 1));
            newRanges.append(qMakePair(it.key() + count, it.key() - row + 1));
        } else {
            newRanges.append(qMakePair(it.key() + count, it.value()));
        }
        it = m_data.erase(it);
    }

    m_data[row+count-1] = count;
    foreach (const intPair& p, newRanges) {
        if (p.second > 1) m_data[p.first] = p.second;
    }
}

void RowRepeatStorage::removeRows(int row, int count)
{
    typedef QPair<int, int> intPair;
    QList<intPair> newRanges;

    // first range that ends at or after row
    QMap<int, int>::iterator it = m_data.lowerBound(row);
    while (it != m_data.end()) {
        if (it.key() - it.value() + 1 < row) {
            // starts before removed rows
            newRanges.append(qMakePair(row-1, row - it.key() + it.value() - 1));
        }
        if (it.key() >= row + count) {
            // ends after the removed rows
            newRanges.append(qMakePair(it.key() - count, qMin(it.value(), it.key() - row - count + 1)));
        }
        it = m_data.erase(it);
    }

    foreach (const intPair& p, newRanges) {
        if (p.second > 1) m_data[p.first] = p.second;
    }
}

void RowRepeatStorage::insertShiftDown(const QRect &rect)
{
    RowRepeatStorage s2 = *this;
    s2.insertRows(rect.top(), rect.height());

    typedef QPair<int, int> intPair;
    QList<intPair> newRanges;

    for (int row = 1; row <= KS_rowMax;) {
        int repeat1 = rowRepeat(row);
        int repeat2 = s2.rowRepeat(row);
        repeat1 -= row - firstIdenticalRow(row);
        repeat2 -= row - s2.firstIdenticalRow(row);
        int repeat = qMin(repeat1, repeat2);
        if (repeat > 1) {
            newRanges.append(qMakePair(row + repeat - 1, repeat));
        }
        row += repeat;
    }

    m_data.clear();
    foreach (const intPair& p, newRanges) {
        if (p.second > 1) m_data[p.first] = p.second;
    }
}

void RowRepeatStorage::removeShiftUp(const QRect &rect)
{
    RowRepeatStorage s2 = *this;
    s2.removeRows(rect.top(), rect.height());

    typedef QPair<int, int> intPair;
    QList<intPair> newRanges;

    for (int row = 1; row <= KS_rowMax;) {
        int repeat1 = rowRepeat(row);
        int repeat2 = s2.rowRepeat(row);
        repeat1 -= row - firstIdenticalRow(row);
        repeat2 -= row - s2.firstIdenticalRow(row);
        int repeat = qMin(repeat1, repeat2);
        if (repeat > 1) {
            newRanges.append(qMakePair(row + repeat - 1, repeat));
        }
        row += repeat;
    }

    m_data.clear();
    foreach (const intPair& p, newRanges) {
        if (p.second > 1) m_data[p.first] = p.second;
    }
}

void RowRepeatStorage::insertShiftRight(const QRect &rect)
{
    // first range that ends at or after rect.top
    QMap<int, int>::iterator it = m_data.lowerBound(rect.top());
    if (it != m_data.end() && (it.key() - it.value() + 1) < rect.top()) {
        // range starts before rect, ends in or after it; split it
        int start = it.key() - it.value() + 1;
        int count = rect.top() - start;
        it.value() = it.key() - rect.top() + 1;
        if (count > 1) m_data[start+count-1] = count;
    }

    // and now same code for bottom+1 of rect
    it = m_data.lowerBound(rect.bottom()+1);
    if (it != m_data.end() && (it.key() - it.value() + 1) < rect.bottom()+1) {
        // range starts before rect, ends in or after it; split it
        int start = it.key() - it.value() + 1;
        int count = rect.bottom() + 1 - start;
        it.value() = it.key() - rect.bottom();
        if (count > 1) m_data[start+count-1] = count;
    }
}

void RowRepeatStorage::removeShiftLeft(const QRect &rect)
{
    // identical to insert
    insertShiftRight(rect);
}
