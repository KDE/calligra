/* This file is part of the KDE project
   Copyright 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2003 Philipp Müller <philipp.mueller@gmx.de>
   Copyright 1998, 1999 Torben Weis <weis@kde.org>,

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

#ifndef KSPREAD_SHEET_PRINT
#define KSPREAD_SHEET_PRINT

#include <QObject>
#include <QRectF>

#include "kspread_export.h"
#include "HeadFoot.h"

class QPainter;

namespace KSpread
{
class HeaderFooter;
class PrintSettings;
class Sheet;

/**
 * \class SheetPrint
 * Manages the layout of pages for printing.
 * Supports next to the obligatory page dimensions
 * zooming, page limits and column/row repetitions.
 * \ingroup Printing
 */
class KSPREAD_TEST_EXPORT SheetPrint : public QObject
{
    Q_OBJECT
public:
    explicit SheetPrint(Sheet * sheet = 0);
    SheetPrint(const SheetPrint &other);
    ~SheetPrint();

    /**
     * \return the print settings
     */
    PrintSettings *settings() const;

    /**
     * Sets the print \p settings.
     * \param force Forces a relayout of the pages, if \c true.
     */
    void setSettings(const PrintSettings &settings, bool force = false);

    /**
     * \return the header & footer object
     */
    HeaderFooter *headerFooter() const;

    /**
     * Tests whether @p column is the first column of a new page. In this
     * case the left border of this column may be drawn highlighted to show
     * that this is a page break.
     */
    bool isColumnOnNewPage( int column );

    /**
     * Tests whether \p row is the first row of a new page. In this
     * case the top border of this row may be drawn highlighted to show
     * that this is a page break.
     */
    bool isRowOnNewPage( int row );

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
     * Updates the print range, according to the inserted columns
     * \param col the column index
     * \param nbCol number of inserted columns
     */
    void insertColumn( int col, int nbCol );

    /**
     * Updates the print range, according to the removed columns
     * \param col the column index
     * \param nbCol number of removed columns
     */
    void removeColumn( int col, int nbCol );

    /**
     * Updates the print range, according to the inserted rows
     * \param row the row index
     * \param nbRow number of inserted rows
     */
    void insertRow( int row, int nbRow );

    /**
     * Updates the print range, according to the removed rows
     * \param row the row index
     * \param nbRow number of removed rows
     */
    void removeRow( int row, int nbRow );

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

Q_SIGNALS:
    void sig_updateView(Sheet *_sheet);

private:
    class Private;
    Private *const d;
};

} // namespace KSpread

#endif // KSPREAD_SHEET_PRINT
