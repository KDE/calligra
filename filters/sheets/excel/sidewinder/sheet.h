/* Swinder - Portable library for spreadsheet
   SPDX-FileCopyrightText: 2003 Ariya Hidayat <ariya@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SWINDER_SHEET_H
#define SWINDER_SHEET_H

#include "cell.h"
#include "format.h"
#include <QImage>
#include <QString>
#include <generated/simpleParser.h>

#include "sheets/core/DataFilter.h"

class QPoint;

namespace Swinder
{

class Workbook;
class Cell;
class Column;
class Row;
struct VerticalPageBreak;
struct HorizontalPageBreak;
class ConditionalFormat;

class Sheet
{
public:
    explicit Sheet(Workbook *workbook);
    virtual ~Sheet();

    // get workbook that owns this sheet
    Workbook *workbook();

    /*
     * Clears the sheet, i.e. makes it as if it is just constructed.
     */
    void clear();

    void setName(const QString &name);

    QString name() const;

    // return cell at specified column and row
    // automatically create the cell if previously there is no cell there
    // return NULL if no cell there _and_ autoCreate is false
    Cell *cell(unsigned column, unsigned row, bool autoCreate = true);

    Column *column(unsigned index, bool autoCreate = true);

    Row *row(unsigned index, bool autoCreate = true);

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

    QString leftHeader() const;
    void setLeftHeader(const QString &h);
    QString centerHeader() const;
    void setCenterHeader(const QString &h);
    QString rightHeader() const;
    void setRightHeader(const QString &h);

    QString leftFooter() const;
    void setLeftFooter(const QString &f);
    QString centerFooter() const;
    void setCenterFooter(const QString &f);
    QString rightFooter() const;
    void setRightFooter(const QString &f);

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

    QString backgroundImage();
    void setBackgroundImage(const QString &imagePath);

    void addVerticalPageBreak(const VerticalPageBreak &pageBreak);
    QList<VerticalPageBreak> verticalPageBreaks();

    void addHorizontalPageBreak(const HorizontalPageBreak &pageBreak);
    QList<HorizontalPageBreak> horizontalPageBreaks();

    int drawObjectsGroupCount() const;
    MSO::OfficeArtSpgrContainer drawObjectsGroup(int groupId) const;
    QList<OfficeArtObject *> drawObjects(int groupId = -1) const;
    void addDrawObject(OfficeArtObject *drawObject, const MSO::OfficeArtSpgrContainer *group = nullptr);

    void addConditionalFormat(ConditionalFormat *format);
    QList<ConditionalFormat *> conditionalFormats() const;

    void setAutoFilters(const Calligra::Sheets::Filter &filter);
    Calligra::Sheets::Filter autoFilters() const;

    void setRightToLeft(bool rtl);
    bool isRightToLeft() const;

#ifdef SWINDER_XLS2RAW
    void dumpStats();
#endif
private:
    // no copy or assign
    Sheet(const Sheet &) = delete;
    Sheet &operator=(const Sheet &) = delete;

    class Private;
    Private *d;

    friend class Cell;

    Hyperlink hyperlink(unsigned column, unsigned row) const;
    void setHyperlink(unsigned column, unsigned row, const Hyperlink &link);

    QList<ChartObject *> charts(unsigned column, unsigned row) const;
    void setCharts(unsigned column, unsigned row, const QList<ChartObject *> &charts);
    void addChart(unsigned column, unsigned row, ChartObject *chart);

    QList<OfficeArtObject *> drawObjects(unsigned column, unsigned row) const;
    void setDrawObjects(unsigned column, unsigned row, const QList<OfficeArtObject *> &drawObjects);
    void addDrawObject(unsigned column, unsigned row, OfficeArtObject *drawObject);
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
    // TODO We righ now use Arial, the usage of whatever is in the document is yet to be done.
    static double columnUnitsToPts(const double columnUnits);

    Column(Sheet *sheet, unsigned index);
    virtual ~Column();

    Sheet *sheet() const;
    unsigned index() const;

    // width of column, in pt
    double width() const;
    // set the width of column, in pt
    void setWidth(double w);

    const Format &format() const;
    void setFormat(const Format *f);

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
    Column(const Column &) = delete;
    Column &operator=(const Column &) = delete;

    class Private;
    Private *d;
};

class Row
{
public:
    Row(Sheet *sheet, unsigned index);
    virtual ~Row();

    Sheet *sheet() const;
    unsigned index() const;

    // height of row, in pt
    double height() const;
    // set the height of row, in pt
    void setHeight(double w);

    const Format &format() const;
    void setFormat(const Format *f);

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
    Row(const Row &) = delete;
    Row &operator=(const Row &) = delete;

    class Private;
    Private *d;
};
}

#endif // SWINDER_SHEET_H
