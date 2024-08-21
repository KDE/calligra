/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ROWFORMATSTORAGE_H
#define CALLIGRA_SHEETS_ROWFORMATSTORAGE_H

#include "sheets_core_export.h"

namespace Calligra
{
namespace Sheets
{

class Sheet;

/** Convenience class for formats */
class CALLIGRA_SHEETS_CORE_EXPORT RowFormat
{
public:
    RowFormat()
    {
        hidden = false;
        filtered = false;
        hasPageBreak = false;
        height = 0;
    }

    double height;
    bool hidden;
    bool filtered;
    bool hasPageBreak;
};

/** first and last row are both inclusive in all functions */
class CALLIGRA_SHEETS_CORE_EXPORT RowFormatStorage
{
public:
    explicit RowFormatStorage(Sheet *sheet);
    ~RowFormatStorage();

    Sheet *sheet() const;

    /** Height of the given row. firstRow and lastRow are set to the first and last row that share the same height. */
    double rowHeight(int row, int *lastRow = nullptr, int *firstRow = nullptr) const;
    void setRowHeight(int firstRow, int lastRow, double height);

    bool isHidden(int row, int *lastRow = nullptr, int *firstRow = nullptr) const;
    void setHidden(int firstRow, int lastRow, bool hidden);

    bool isFiltered(int row, int *lastRow = nullptr, int *firstRow = nullptr) const;
    void setFiltered(int firstRow, int lastRow, bool filtered);

    bool isHiddenOrFiltered(int row, int *lastRow = nullptr, int *firstRow = nullptr) const;

    double visibleHeight(int row, int *lastRow = nullptr, int *firstRow = nullptr) const;

    double totalRowHeight(int firstRow, int lastRow) const;
    double totalVisibleRowHeight(int firstRow, int lastRow) const;

    int rowForPosition(double ypos, double *topOfRow = nullptr) const;

    bool hasPageBreak(int row, int *lastRow = nullptr, int *firstRow = nullptr) const;
    void setPageBreak(int firstRow, int lastRow, bool pageBreak);

    /** Convenience function - sets everything at once. */
    void setRowFormat(int firstRow, int lastRow, const RowFormat &f);
    RowFormat getRowFormat(int row) const;

    bool isDefaultRow(int row, int *lastRow = nullptr, int *firstRow = nullptr) const;
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

    RowFormatStorage &operator=(const RowFormatStorage &r);

private:
    RowFormatStorage(const RowFormatStorage &);
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ROWFORMATSTORAGE_H
