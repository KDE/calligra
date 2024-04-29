/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
   SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
     Contact: Suresh Chande suresh.chande@nokia.com

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_KDCHARTMODEL_H
#define KCHART_KDCHARTMODEL_H

// Qt
#include <QAbstractItemModel>

// KoChart
#include "ChartShape.h"

namespace KoChart
{

class DataSet;

/**
 * Takes a list of DataSet's and compiles them into a
 * QAbstractItemModel for use with KChart.
 *
 * Data sets in this model are aligned column-wise. Each column
 * occupies dimension() columns. For example, for an X/Y chart,
 * the data of this model would be structured as follows:
 *
 *             Brush 0       Brush 1
 *             Pen 0         Pen 1
 *             Label 0       Label 1
 * -----------|------|------|------|------|
 * Category 1 | x0,0 | y0,0 | x1,0 | x1,0 |
 * -----------|------|------|------|------|
 * Category 2 | x0,1 | y0,1 | x1,1 | x1,1 |
 * -----------|------|------|------|------|
 * Category 3 | x0,2 | y0,2 | x1,2 | x1,2 |
 * -----------|------|------|------|------|
 *
 */

/**
 * Note on data directions in KChart's models:
 *
 * For circular (polar) charts, items shown in the legend should not be the
 * data set labels, but the category labels instead. For example, a pie chart
 * contains exactly one data set (if there's more series in the table, they are
 * ignored). Obviously showing the title of the data set wouldn't be very useful
 * in the legend. So the categories are shown instead.
 *
 * Since KChart extracts the legend items from horizontal header data (the headers
 * in each column) data sets have to be inserted row-wise instead of column-wise for
 * these charts. To do so, KChartModel::setDataDirection(Qt::Horizontal) is used.
 *
 * In all other cases, we show the data set labels in the legend. Therefore we insert
 * data sets column-wise, which is done by calling setDataDirection(Qt::Vertical).
 */

class KChartModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit KChartModel(PlotArea *plotArea, QObject *parent = nullptr);
    ~KChartModel();

    enum DataRole {
        XDataRole,
        YDataRole,
        ZDataRole,
        LabelDataRole,
        CategoryDataRole,
        CustomDataRole,
        BrushDataRole,
        PenDataRole,
        PieAttributesRole,
        DataValueAttributesRole
    };

    /**
     * Specifies in what direction a data set 'points'. More specifically,
     * if the data direction is Qt::Vertical, a data set occupies one
     * column (in case only one data dimension is being used).
     *
     * See "Note on data directions in KChart's models" above.
     */
    void setDataDirection(Qt::Orientation direction);
    /**
     * See \a setDataDirection
     */
    Qt::Orientation dataDirection() const;
    /**
     * Returns the opposite of dataDirection().
     */
    Qt::Orientation categoryDirection() const;

public Q_SLOTS:
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void slotColumnsInserted(const QModelIndex &parent, int start, int end);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void setDataDimensions(int dataDimensions);
    int dataDimensions() const;

    void addDataSet(KoChart::DataSet *dataSet);
    void removeDataSet(KoChart::DataSet *dataSet, bool silent = false);
    QList<KoChart::DataSet *> dataSets() const;

    /**
     * Called by DataSet whenever a property that is global to all its data
     * points changes, e.g. its label or its pen
     */
    void dataSetChanged(KoChart::DataSet *dataSet);

    /**
     * Called by DataSet whenever one or more of its data points changes,
     * e.g. the x value of a data point.
     *
     * FIXME: @a role doesn't make sense here, it's not needed for emitting
     *        the dataChanged() signal. Removing it would conflict with
     *        dataSetChanged(DataSet*), that's why it's still there.
     *
     * @param first First data point that changed. If -1 it is assumed that
     *              all data points in this series changed.
     * @param last Last data point that changed. If -1 it is assumed that
     *             only a single data point changed.
     */
    void dataSetChanged(KoChart::DataSet *dataSet, KoChart::KChartModel::DataRole role, int first = -1, int last = -1);

    /**
     * Called by DataSet when the total number of data points it has changed.
     */
    void dataSetSizeChanged(KoChart::DataSet *dataSet, int newSize);

private:
    class Private;
    Private *const d;
};

} // namespace KoChart

#endif // KCHART_KDCHARTMODEL_H
