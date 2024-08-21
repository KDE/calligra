/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2003 Philipp Müller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>,

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SHEET_PRINT
#define CALLIGRA_SHEETS_SHEET_PRINT

#include <QObject>
#include <QRectF>

#include "sheets_core_export.h"

namespace Calligra
{
namespace Sheets
{
class HeaderFooter;
class Sheet;
class PrintSettings;

/**
 * \class SheetPrint
 * Manages the layout of pages for printing.
 * Supports next to the obligatory page dimensions
 * zooming, page limits and column/row repetitions.
 * \ingroup Printing
 */
class CALLIGRA_SHEETS_CORE_EXPORT SheetPrint : public QObject
{
    Q_OBJECT
public:
    explicit SheetPrint(Sheet *sheet = nullptr);
    SheetPrint(const SheetPrint &other);
    ~SheetPrint();

    /**
     * Sets the print \p settings.
     * \param settings the print settings.
     * \param force Forces a relayout of the pages, if \c true.
     */
    void setSettings(const PrintSettings &settings, bool force = false);

    /**
     * Tests whether @p column is the first column of a new page. In this
     * case the left border of this column may be drawn highlighted to show
     * that this is a page break.
     */
    bool isColumnOnNewPage(int column);

    /**
     * Tests whether \p row is the first row of a new page. In this
     * case the top border of this row may be drawn highlighted to show
     * that this is a page break.
     */
    bool isRowOnNewPage(int row);

    /**
     * Updates the page parameters in horizontal direction (for columns)
     * starting at \p column.
     * Actually, only removes them and they get calculated on demand.
     * Triggers an update of the repeated columns' pre-calculated width,
     * if \p column is not beyond the repetition.
     */
    void updateHorizontalPageParameters(int column);

    /**
     * Updates the page parameters in vertical direction (for rows)
     * starting at \p row.
     * Actually, only removes them and they get calculated on demand.
     * Triggers an update of the repeated rows' pre-calculated height,
     * if \p row is not beyond the repetition.
     */
    void updateVerticalPageParameters(int row);

    /**
     * The number of pages.
     */
    int pageCount() const;

    /**
     * Return the cell range of the requested page.
     * \param page the page number
     * \return the page's cell range
     */
    QRect cellRange(int page) const;

    /**
     * Return the document area of the requested page.
     * \param page the page number
     * \return the page's document area
     */
    QRectF documentArea(int page) const;

    /**
     * Assignment operator.
     */
    void operator=(const SheetPrint &);

public Q_SLOTS:
    /**
     * Updates the print range, according to the inserted columns
     * \param col the column index
     * \param nbCol number of inserted columns
     */
    void insertColumn(int col, int nbCol);

    /**
     * Updates the print range, according to the removed columns
     * \param col the column index
     * \param nbCol number of removed columns
     */
    void removeColumn(int col, int nbCol);

    /**
     * Updates the print range, according to the inserted rows
     * \param row the row index
     * \param nbRow number of inserted rows
     */
    void insertRow(int row, int nbRow);

    /**
     * Updates the print range, according to the removed rows
     * \param row the row index
     * \param nbRow number of removed rows
     */
    void removeRow(int row, int nbRow);

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SHEET_PRINT
