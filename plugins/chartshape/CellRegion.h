/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KCHART_CELLREGION_H
#define KCHART_CELLREGION_H

// Qt
#include <QRect>
#include <QVector>
#include <Qt>

// KoChart
#include "ChartShape.h"

class QRect;
class QPoint;

namespace KoChart
{

/**
 * @brief A CellRegion represents a selection of cells in a table.
 *
 * Seen from the outside, each CellRegion is one-dimensional, i.e. a
 * cell in it can be addressed by a single index.  However, the actual
 * cells in it can be broken up into several one-dimensional
 * rectangles with either width=1 or height=1.
 *
 * Every data set on the chart has five independent cell regions
 * that indicate where the data in a series comes from:
 * 1) a label to represent the data set (size 1x1)
 * 2) a region for y values
 * 3) one for x values (only for scatter and bubble charts)
 * 4) for bubble widths (only for bubble charts)
 * 5) for the category data (one label for every x value/column on
 *    the x axis; this region is the same among all data sets)
 *
 * A CellRegion can also represent a region in a spreadsheet that
 * is relevant for the data of a chart. That way, the initial chart
 * is created based on the cell selection the user made before
 * inserting the chart shape.
 *
 * In contrast to a QItemSelection, a CellRegion can include header
 * data. Therefore, CellRegion(QPoint(1, 1)) represents the
 * top-left item of a QAbstractItemModel.
 *
 * An instance can represent either a simple, continuous region of
 * cells, as in most cases, or a more complex discontinuous region.
 * In its second form, the orientation of each separate continuous
 * region can vary, as well as their sizes.
 */

// Definition in TableSource.h
class Table;

class CellRegion
{
public:
    CellRegion();
    CellRegion(const CellRegion &region);
    CellRegion(TableSource *source, const QString &regions);
    CellRegion(Table *table, const QPoint &point);
    CellRegion(Table *table, const QRect &rect);
    CellRegion(Table *table, const QVector<QRect> &rects);
    explicit CellRegion(Table *table);
    ~CellRegion();

    CellRegion &operator=(const CellRegion &region);
    bool operator==(const CellRegion &other) const;

    Table *table() const;

    QVector<QRect> rects() const;

    QString sheetName() const;

    bool isValid() const;

    QString toString() const;

    bool contains(const QPoint &point, bool proper = false) const;
    bool contains(const QRect &rect, bool proper = false) const;

    bool intersects(const CellRegion &other) const;

    CellRegion intersected(const QRect &rect) const;

    int cellCount() const;
    int rectCount() const;

    Qt::Orientation orientation() const;

    void add(const CellRegion &other);
    void add(const QPoint &point);
    void add(const QRect &rect);
    void add(const QVector<QRect> &rects);

    QRect boundingRect() const;

    bool hasPointAtIndex(int index) const;
    QPoint pointAtIndex(int index) const;
    int indexAtPoint(const QPoint &point) const;

    static int rangeCharToInt(char c);
    static int rangeStringToInt(const QString &string);
    static QString rangeIntToString(int i);

    static QString columnName(uint column);

private:
    class Private;
    Private *const d;
};

}

QDebug operator<<(QDebug dbg, const KoChart::CellRegion &r);

#endif // KCHART_CELLREGION_H
