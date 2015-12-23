/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

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

#ifndef CALLIGRA_SHEETS_ROWFORMATSTORAGE_H
#define CALLIGRA_SHEETS_ROWFORMATSTORAGE_H

#include <Qt>

#include "sheets_odf_export.h"

namespace Calligra {
namespace Sheets {

class Sheet;

/** first and last row are both inclusive in all functions */
class CALLIGRA_SHEETS_ODF_EXPORT RowFormatStorage
{
public:
    explicit RowFormatStorage(Sheet *sheet);
    ~RowFormatStorage();

    Sheet* sheet() const;

    qreal rowHeight(int row, int* lastRow = 0, int* firstRow = 0) const;
    void setRowHeight(int firstRow, int lastRow, qreal height);

    bool isHidden(int row, int* lastRow = 0, int* firstRow = 0) const;
    void setHidden(int firstRow, int lastRow, bool hidden);

    bool isFiltered(int row, int* lastRow = 0, int* firstRow = 0) const;
    void setFiltered(int firstRow, int lastRow, bool filtered);

    bool isHiddenOrFiltered(int row, int* lastRow = 0, int* firstRow = 0) const;

    qreal visibleHeight(int row, int* lastRow = 0, int* firstRow = 0) const;

    qreal totalRowHeight(int firstRow, int lastRow) const;
    qreal totalVisibleRowHeight(int firstRow, int lastRow) const;

    int rowForPosition(qreal ypos, qreal * topOfRow = 0) const;

    bool hasPageBreak(int row, int* lastRow = 0, int* firstRow = 0) const;
    void setPageBreak(int firstRow, int lastRow, bool pageBreak);

    bool isDefaultRow(int row, int* lastRow = 0, int* firstRow = 0) const;
    void setDefault(int firstRow, int lastRow);
    int lastNonDefaultRow() const;

    bool rowsAreEqual(int row1, int row2) const;


    /**
     * Insert \p number of rows at position \p row.
     * Also updates the sheets documentHeight property
     */
    void insertRows(int row, int number);

    /**
     * Removes \p number of rows starting at position \p row.
     * Also updates the sheets documentHeight property
     */
    void removeRows(int row, int number);

    RowFormatStorage& operator=(const RowFormatStorage& r);
private:
    RowFormatStorage(const RowFormatStorage&);
    class Private;
    Private * const d;
};


} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ROWFORMATSTORAGE_H

