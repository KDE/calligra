/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

#ifndef KSPREAD_PRINT_SETTINGS
#define KSPREAD_PRINT_SETTINGS

#include <KoPageLayout.h>

#include <QPair>

#include "kspread_export.h"

class QSize;

namespace KSpread
{
class Region;
class Sheet;

/**
 * Settings used for printing.
 */
class KSPREAD_EXPORT PrintSettings
{
public:
    enum PageOrder { TopToBottom, LeftToRight };

    /**
     * Constructor.
     */
    PrintSettings();

    /**
     * Constructor.
     */
    PrintSettings(const PrintSettings& other);

    /**
     * Destructor.
     */
    virtual ~PrintSettings();

    /**
     * \return the page layout associated with this document (margins, pageSize, etc).
     * \see KoPageLayout
     */
    const KoPageLayout& pageLayout() const;

    void setPageLayout(const KoPageLayout& pageLayout);

    void setPageFormat(KoPageFormat::Format format);
    void setPageOrientation(KoPageFormat::Orientation orientation);

    /**
     * \return the name of the paper orientation (like Portrait, Landscape)
     */
    QString orientationString() const;

    /**
     * \return the name of the paper format (like A4, Letter etc.)
     */
    QString paperFormatString() const;

    /**
     * \return the print width of the paper.
     */
    double printWidth() const;

    /**
     * \return the print height of the paper.
     */
    double printHeight() const;

    /**
     * The order in which the pages should be created.
     * Either they are created beginning at the left, continuing to the right and
     * then the next row of pages, or they are created vertically page column-wise.
     *
     * \return the page order
     */
    PageOrder pageOrder() const;
    void setPageOrder(PageOrder order);

    /**
     * Returns, if the grid shall be shown on printouts.
     */
    bool printGrid() const;

    /**
     * Sets, if the grid shall be shown on printouts.
     */
    void setPrintGrid(bool printGrid);

    /**
     * Returns, if the charts shall be shown on printouts.
     */
    bool printCharts() const;

    /**
     * Sets, if the charts shall be shown on printouts.
     */
    void setPrintCharts(bool printCharts);

    /**
     * Returns, if the objects shall be shown on printouts
     */
    bool printObjects() const;

    /**
     * Sets, if the objects shall be shown on printouts.
     */
    void setPrintObjects(bool printObjects);

    /**
     * Returns, if the graphics shall be shown on printouts.
     */
    bool printGraphics() const;

    /**
     * Sets, if the graphics shall be shown on printouts.
     */
    void setPrintGraphics(bool printGraphics);

    /**
     * Returns, if the comment rect shall be shown on printouts
     */
    bool printCommentIndicator() const;

    /**
     * Sets, if the comment rect shall be shown on printouts
     */
    void setPrintCommentIndicator(bool printCommentIndicator);

    /**
     * Returns, if the formula rect shall be shown on printouts.
     */
    bool printFormulaIndicator() const;

    /**
     * Sets, if the formula Rect shall be shown on printouts.
     */
    void setPrintFormulaIndicator(bool printFormulaIndicator);

    /**
     * Returns, if the headers shall be shown on printouts.
     */
    bool printHeaders() const;

    /**
     * Sets, if the headers shall be shown on printouts.
     */
    void setPrintHeaders(bool printHeaders);

    /**
     * Returns, if the zero values shall be shown on printouts.
     */
    bool printZeroValues() const;

    /**
     * Sets, if the zero values shall be shown on printouts.
     */
    void setPrintZeroValues(bool printZeroValues);

    bool centerHorizontally() const;
    void setCenterHorizontally(bool center);
    bool centerVertically() const;
    void setCenterVertically(bool center);

    const Region& printRegion() const;
    void setPrintRegion(const Region& region);
    void addPrintRange(const QRect& range);
    void removePrintRange(const QRect& range);

    double zoom() const;
    void setZoom(double zoom);
    const QSize& pageLimits() const;
    void setPageLimits(const QSize& pageLimits);

    /**
     * Returns the columns, which are printed on each page.
     * Returns QPair (0, 0) if nothing is defined.
     */
    const QPair<int, int>& repeatedColumns() const;

    /**
     * Sets the columns to be printed on each page.
     * Only the x-values of the points are used
     * Set it to QPair (0, 0) to undefine it
     */
    void setRepeatedColumns(const QPair<int, int>& repeatedColumns);

    /**
     * Returns the rows, which are printed on each page.
     * Returns QPair (0, 0) if nothing is defined
     */
    const QPair<int, int>& repeatedRows() const;

    /**
     * Sets the rows to be printed on each page.
     * Only the y-values of the points are used
     * Set it to QPair (0, 0) to undefine it
     */
    void setRepeatedRows(const QPair<int, int>& repeatedRows);

    /**
     * Saves a page layout style.
     * Adds the style to \p mainStyles.
     * Saved OpenDocument properties:
     * \li 15.2.1 Page Size
     * \li 15.2.4 Print Orientation
     * \li 15.2.5 Margins
     * \li 15.2.13 Print
     * \li 15.2.14 Print Page Order
     * \li 15.2.16 Scale
     * \li 15.2.17 Table Centering
     * \param mainStyles generic styles
     * \param formulas print formulas
     * \param zeros print zero values
     * \return the style's name
     * \ingroup OpenDocument
     */
    QString saveOdfPageLayout(KoGenStyles &mainStyles, bool formulas, bool zeros);

    /**
     * Assignment operator.
     */
    void operator=(const PrintSettings& settings);

    /**
     * Equality operator.
     */
    bool operator==(const PrintSettings& other) const;
    inline bool operator!=(const PrintSettings& other) const {
        return !operator==(other);
    }

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_PRINT_SETTINGS
