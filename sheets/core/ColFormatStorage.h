/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_COLFORMATSTORAGE_H
#define CALLIGRA_SHEETS_COLFORMATSTORAGE_H

#include "sheets_core_export.h"

namespace Calligra
{
namespace Sheets
{

class Sheet;

/** Convenience class for formats */
class CALLIGRA_SHEETS_CORE_EXPORT ColFormat
{
public:
    ColFormat()
    {
        hidden = false;
        filtered = false;
        hasPageBreak = false;
        width = 0;
    }

    double width;
    bool hidden;
    bool filtered;
    bool hasPageBreak;
};

/** first and last col are both inclusive in all functions */
class CALLIGRA_SHEETS_CORE_EXPORT ColFormatStorage
{
public:
    explicit ColFormatStorage(Sheet *sheet);
    ~ColFormatStorage();

    Sheet *sheet() const;

    /** Width of the given col. firstCol and lastCol are set to the first and last col that share the same width. */
    double colWidth(int col, int *lastCol = nullptr, int *firstCol = nullptr) const;
    void setColWidth(int firstCol, int lastCol, double width);

    bool isHidden(int col, int *lastCol = nullptr, int *firstCol = nullptr) const;
    void setHidden(int firstCol, int lastCol, bool hidden);

    bool isFiltered(int col, int *lastCol = nullptr, int *firstCol = nullptr) const;
    void setFiltered(int firstCol, int lastCol, bool filtered);

    bool isHiddenOrFiltered(int col, int *lastCol = nullptr, int *firstCol = nullptr) const;

    double visibleWidth(int col, int *lastCol = nullptr, int *firstCol = nullptr) const;

    double totalColWidth(int firstCol, int lastCol) const;
    double totalVisibleColWidth(int firstCol, int lastCol) const;

    int colForPosition(double ypos, double *topOfCol = nullptr) const;

    bool hasPageBreak(int col, int *lastCol = nullptr, int *firstCol = nullptr) const;
    void setPageBreak(int firstCol, int lastCol, bool pageBreak);

    /** Convenience function - sets everything at once. */
    void setColFormat(int firstCol, int lastCol, const ColFormat &f);
    ColFormat getColFormat(int col) const;

    bool isDefaultCol(int col, int *lastCol = nullptr, int *firstCol = nullptr) const;
    void setDefault(int firstCol, int lastCol);
    int lastNonDefaultCol() const;

    bool colsAreEqual(int col1, int col2) const;

    /**
     * Insert \p number of cols at position \p col.
     * Also updates the sheets documentWidth property
     */
    void insertCols(int col, int number);

    /**
     * Removes \p number of cols starting at position \p col.
     * Also updates the sheets documentWidth property
     */
    void removeCols(int col, int number);

    ColFormatStorage &operator=(const ColFormatStorage &r);

private:
    ColFormatStorage(const ColFormatStorage &) = delete;
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_COLFORMATSTORAGE_H
