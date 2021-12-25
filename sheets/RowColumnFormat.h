/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000-2003 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ROW_COLUMN_FORMAT
#define CALLIGRA_SHEETS_ROW_COLUMN_FORMAT

#include <QBrush>

#include <KoXmlReader.h>

#include "sheets_export.h"
#include "Global.h"
#include "Style.h"

namespace Calligra
{
namespace Sheets
{
class Sheet;
class RowFormatStorage;

/**
 * A row style.
 */
class CALLIGRA_SHEETS_ODF_EXPORT RowFormat
{
public:
    RowFormat();
    RowFormat(const RowFormat& other);
    RowFormat(const RowFormatStorage* rows, int row);
    ~RowFormat();

    void setSheet(Sheet* sheet);

    /**
     * \return the row's height
     */
    double height() const;

    /**
     * The visible row height, respecting hiding and filtering attributes.
     * \return the visible row height
     */
    double visibleHeight() const;

    /**
     * Sets the height to _h zoomed pixels.
     *
     * @param _h is calculated in display pixels as double value. The function cares for zooming.
     * Use this function when setting the height, to not get rounding problems.
     */
    void setHeight(double _h);

    /**
     * @reimp
     */
    bool isDefault() const;

    /**
     * @return the row for this RowFormat. May be 0 if this is the default format.
     */
    int row() const;
    void setRow(int row);

    RowFormat* next() const;
    RowFormat* previous() const;
    void setNext(RowFormat* c);
    void setPrevious(RowFormat* c);

    /**
     * Sets the hide flag
     */
    void setHidden(bool _hide, bool repaint = true);
    bool isHidden() const;

    void setFiltered(bool filtered);
    bool isFiltered() const;

    bool isHiddenOrFiltered() const;

    /**
     * Sets a page break before this row, if \p enable is \c true.
     */
    void setPageBreak(bool enable);

    /**
     * \return \c true, if there's a page break set before this row.
     */
    bool hasPageBreak() const;

    bool operator==(const RowFormat& other) const;
    inline bool operator!=(const RowFormat& other) const {
        return !operator==(other);
    }

private:
    // do not allow assignment
    RowFormat& operator=(const RowFormat&);

    class Private;
    Private * const d;
};

/**
 * A column style.
 */
class CALLIGRA_SHEETS_ODF_EXPORT ColumnFormat
{
public:
    ColumnFormat();
    ColumnFormat(const ColumnFormat& other);
    ~ColumnFormat();

    void setSheet(Sheet* sheet);

    /**
     * \return the column's width
     */
    double width() const;

    /**
     * The visible column width, respecting hiding and filtering attributes.
     * \return the visible column width
     */
    double visibleWidth() const;

    /**
     * Sets the width to _w zoomed pixels as double value.
     * Use this function to set the width without getting rounding problems.
     *
     * @param _w is calculated in display pixels. The function cares for
     *           zooming.
     */
    void setWidth(double _w);

    /**
     * @reimp
     */
    bool isDefault() const;

    /**
     * @return the column of this ColumnFormat. May be 0 if this is the default format.
     */
    int column() const;
    void setColumn(int column);

    ColumnFormat* next() const;
    ColumnFormat* previous() const;
    void setNext(ColumnFormat* c);
    void setPrevious(ColumnFormat* c);

    void setHidden(bool _hide);
    bool isHidden() const;

    void setFiltered(bool filtered);
    bool isFiltered() const;

    bool isHiddenOrFiltered() const;

    /**
     * Sets a page break before this row, if \p enable is \c true.
     */
    void setPageBreak(bool enable);

    /**
     * \return \c true, if there's a page break set before this row.
     */
    bool hasPageBreak() const;

    bool operator==(const ColumnFormat& other) const;
    inline bool operator!=(const ColumnFormat& other) const {
        return !operator==(other);
    }

private:
    // do not allow assignment
    ColumnFormat& operator=(const ColumnFormat&);

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ROW_COLUMN_FORMAT
