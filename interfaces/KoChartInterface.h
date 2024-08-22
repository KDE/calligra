/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000-2002 Kalle Dalheimer <kalle@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KOCHART_INTERFACE
#define KOCHART_INTERFACE

#include <Qt>
#include <QtPlugin>

#define ChartShapeId "ChartShape"

class QAbstractItemModel;
class QString;

namespace KoChart
{

/**
 * Interface for ChartShape to embed it into a spreadsheet.
 */
class ChartInterface
{
public:
    virtual ~ChartInterface() = default;

    /**
     * Sets the SheetAccessModel to be used by this chart. Use this method if
     * you want to embed the ChartShape into a spreadsheet.
     *
     * See sheets/SheetAccessModel.h for details.
     */
    virtual void setSheetAccessModel(QAbstractItemModel *model) = 0;

    /**
     * Re-initializes the chart with data from an arbitrary region.
     *
     * @param region             Name of region to use, e.g. "Table1.A1:B3"
     * @param firstRowIsLabel    Whether to interpret the first row as labels
     * @param firstColumnIsLabel Whether to interpret the first column as labels
     * @param dataDirection      orientation of a data set. Qt::Horizontal means a row is
     *                           to be interpreted as one data set, columns with Qt::Vertical.
     */
    virtual void reset(const QString &region, bool firstRowIsLabel, bool firstColumnIsLabel, Qt::Orientation dataDirection) = 0;
};

} // namespace KoChart

Q_DECLARE_INTERFACE(KoChart::ChartInterface, "org.calligra.KoChart.ChartInterface:1.0")

#endif // KOCHART_INTERFACE
