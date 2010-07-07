/* Swinder - Portable library for spreadsheet
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

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
 * Boston, MA 02110-1301, USA
*/

#ifndef SWINDER_SHEET_H
#define SWINDER_SHEET_H

#include "ustring.h"
#include "format.h"
#include <QImage>

class QPoint;

namespace Swinder
{

class Workbook;
class Cell;
class Column;
class Row;
struct VerticalPageBreak;
struct HorizontalPageBreak;

class Sheet
{
public:

    explicit Sheet(Workbook* workbook);
    virtual ~Sheet();

    // get workbook that owns this sheet
    Workbook* workbook();

    /*
     * Clears the sheet, i.e. makes it as if it is just constructed.
     */
    void clear();

    void setName(const UString& name);

    UString name() const;

    // return cell at specified column and row
    // automatically create the cell if previously there is no cell there
    // return NULL if no cell there _and_ autoCreate is false
    Cell* cell(unsigned column, unsigned row, bool autoCreate = true);

    Column* column(unsigned index, bool autoCreate = true);

    Row* row(unsigned index, bool autoCreate = true);

    bool visible() const;
    void setVisible(bool v);

    bool protect() const;
    void setProtect(bool p);

    /*
     * Returns true if automatic calculation is enabled.
     */
    bool autoCalc() const;

    /*
     * Sets the automatic calculation.
     */
    void setAutoCalc(bool a);

    /*
     *   &P  current page number
     *   &D  current date
     *   &T  current time
     *   &A  sheet name
     *   &F  file name without path
     *   &Z  file path without file name
     *   &G  picture
     *   &B  bold on/off
     *   &I  italic on/off
     *   &U  underlining on/off
     *   &E  double underlining on/off
     *   &S  strikeout on/off
     *   &X  superscript on/off
     *   &Y  subscript on/off
     *
     *   &"<fontname>"  set font name
     *   &"<fontname>,<fontstyle>" set font name and style
     *   &<fontheight>  set font height

     */

    UString leftHeader() const;
    void setLeftHeader(const UString& h);
    UString centerHeader() const;
    void setCenterHeader(const UString& h);
    UString rightHeader() const;
    void setRightHeader(const UString& h);

    UString leftFooter() const;
    void setLeftFooter(const UString& f);
    UString centerFooter() const;
    void setCenterFooter(const UString& f);
    UString rightFooter() const;
    void setRightFooter(const UString& f);

    // left margin, in points (pt)
    double leftMargin() const;
    void setLeftMargin(double m);

    // right margin, in points (pt)
    double rightMargin() const;
    void setRightMargin(double m);

    // top margin, in points (pt)
    double topMargin() const;
    void setTopMargin(double m);

    // bottom margin, in points (pt)
    double bottomMargin() const;
    void setBottomMargin(double m);

    unsigned maxRow() const;
    unsigned maxColumn() const;

    // explicitly set the maxRow/maxColumn values to at least a specific value
    void setMaxRow(unsigned row);
    void setMaxColumn(unsigned column);

    // returns the maximal number of cells within a row
    unsigned maxCellsInRow(int rowIndex) const;

    long defaultRowHeight() const;
    void setDefaultRowHeight(long);
    long defaultColWidth() const;
    void setDefaultColWidth(long);

    double zoomLevel() const;
    void setZoomLevel(double fraction);

    bool showGrid() const;
    void setShowGrid(bool show);

    bool showZeroValues() const;
    void setShowZeroValues(bool show);

    QPoint firstVisibleCell() const;
    void setFirstVisibleCell(const QPoint &point);

    bool isPageBreakViewEnabled() const;
    void setPageBreakViewEnabled(bool enabled);

    unsigned long password() const;
    void setPassword(unsigned long hash);

    UString backgroundImage();
    void setBackgroundImage(const Swinder::UString& imagePath);

    void addVerticalPageBreak(const VerticalPageBreak& pageBreak);
    QList<VerticalPageBreak> verticalPageBreaks();

    void addHorizontalPageBreak(const HorizontalPageBreak& pageBreak);
    QList<HorizontalPageBreak> horizontalPageBreaks();

private:
    // no copy or assign
    Sheet(const Sheet&);
    Sheet& operator=(const Sheet&);

    class Private;
    Private *d;
};

struct VerticalPageBreak {
    quint16 col;
    quint16 rowStart;
    quint16 rowEnd;
};

struct HorizontalPageBreak {
    quint16 row;
    quint16 colStart;
    quint16 colEnd;
};

class Column
{
public:
    //TODO We righ now use Arial, the usage of whatever is in the document is yet to be done.
    static double columnUnitsToPts(const double columnUnits);

    Column(Sheet* sheet, unsigned index);
    virtual ~Column();

    Sheet* sheet() const;
    unsigned index() const;

    // width of column, in pt
    double width() const;
    // set the width of column, in pt
    void setWidth(double w);

    const Format& format() const;
    void setFormat(const Format* f);

    bool visible() const;
    void setVisible(bool v);

    unsigned outlineLevel() const;
    void setOutlineLevel(unsigned level);
    bool collapsed() const;
    void setCollapsed(bool collapsed);

    bool operator==(const Column &other) const;
    bool operator!=(const Column &other) const;

private:
    // no copy or assign
    Column(const Column&);
    Column& operator=(const Column&);

    class Private;
    Private *d;
};

class Row
{
public:

    Row(Sheet* sheet, unsigned index);
    virtual ~Row();

    Sheet* sheet() const;
    unsigned index() const;

    // height of row, in pt
    double height() const;
    // set the height of row, in pt
    void setHeight(double w);

    const Format& format() const;
    void setFormat(const Format* f);

    bool visible() const;
    void setVisible(bool v);

    unsigned outlineLevel() const;
    void setOutlineLevel(unsigned level);
    bool collapsed() const;
    void setCollapsed(bool collapsed);

    bool operator==(const Row &other) const;
    bool operator!=(const Row &other) const;

private:
    // no copy or assign
    Row(const Row&);
    Row& operator=(const Row&);

    class Private;
    Private *d;
};


}

#endif // SWINDER_SHEET_H

