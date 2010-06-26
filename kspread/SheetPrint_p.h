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

#ifndef KSPREAD_SHEET_PRINT_P
#define KSPREAD_SHEET_PRINT_P

#include <QList>

namespace KSpread
{
class HeaderFooter;
class Sheet;
class SheetPrint;

/**
 * Page parameters for both directions, horizontal and vertical
 * (or columns and rows).
 * Stores the start column/row, the end column/row, the document offset
 * and the dimension (width/height) of a page.
 */
class PrintNewPageEntry
{
public:
    explicit PrintNewPageEntry(int startItem, int endItem = 0, double size = 0, double offset = 0)
            : m_iStartItem(startItem)
            , m_iEndItem(endItem)
            , m_dSize(size)
            , m_dOffset(offset) {}

    int startItem() const {
        return m_iStartItem;
    }
    void setStartItem(int startItem) {
        m_iStartItem = startItem;
    }

    int endItem() const {
        return m_iEndItem;
    }
    void setEndItem(int endItem) {
        m_iEndItem = endItem;
    }

    double size() const {
        return m_dSize;
    }
    void setSize(double size) {
        m_dSize = size;
    }

    double offset() const {
        return m_dOffset;
    }
    void setOffset(double offset) {
        m_dOffset = offset;
    }

    bool operator==(PrintNewPageEntry const & entry) const;


private:
    int m_iStartItem;   // column or row index
    int m_iEndItem;     // column or row index
    double m_dSize;     // width or height
    double m_dOffset;   // horizontal or vertical offset
};


class SheetPrint::Private
{
public:
    Private(SheetPrint *parent) : q(parent) {}

    /**
     * Calculates the zoom factor, so that the printout fits on pages in X direction.
     */
    void calculateZoomForPageLimitX();

    /**
     * Calculates the zoom factor, so that the printout fits on pages in Y direction.
     */
    void calculateZoomForPageLimitY();

    /**
     * Calculates the missing horizontal page parameters up to \p column.
     */
    void calculateHorizontalPageParameters(int column);

    /**
     * Calculates the missing vertical page parameters up to \p row.
     */
    void calculateVerticalPageParameters(int row);

    /**
     * Updates the pre-calculated width of the repeated columns.
     */
    void updateRepeatedColumnsWidth();

    /**
     * Updates the pre-calculated height of the repeated rows.
     */
    void updateRepeatedRowsHeight();

public:
    SheetPrint *q;
    Sheet * m_pSheet;

    PrintSettings* m_settings;
    HeaderFooter *m_headerFooter;

    /**
     * Width of repeated columns in points, stored for performance reasons
     */
    double m_dPrintRepeatColumnsWidth;

    /**
     * Height of repeated rows in points, stored for performance reasons
     */
    double m_dPrintRepeatRowsHeight;

    /**
     * Stores the horizontal page parameters (for columns).
     */
    QList<PrintNewPageEntry> m_lnewPageListX;

    /**
     * Stores the vertical page parameters (for rows).
     */
    QList<PrintNewPageEntry> m_lnewPageListY;

    /**
     * Stores the column, up to which the horizontal page parameters got calculated.
     */
    int m_maxCheckedNewPageX;

    /**
     * Stores the row, up to which the vertical page parameters got calculated.
     */
    int m_maxCheckedNewPageY;
};

} // namespace KSpread

#endif // KSPREAD_SHEET_PRINT_P
