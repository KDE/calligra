/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_PROXYMODEL_H
#define KCHART_PROXYMODEL_H

#define INTERNAL_TABLE_NAME "ChartTable"

// KoChart
#include "CellRegion.h"
#include "ChartShape.h"

namespace KoChart
{
class ChartModel;
}

// Qt
#include <QAbstractTableModel>

namespace KoChart
{

/**
 * @brief The ChartProxyModel is a factory for the DataSet's and decorates the ChartTableModel.
 *
 * TODO: Rename this class to something more meaningful (and correct) like
 * "DataProvider" and maybe split it up into one class that handles the
 * QAbstractItemModel part, and another one that handles CellRegions for
 * all the data points.
 */
class ChartProxyModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    ChartProxyModel(ChartShape *shape, TableSource *source);
    ~ChartProxyModel();

    /**
     * Used for data retrieval of all relevant dimensions: x, y, z, etc.
     *
     * This enum may be extended at a later point to store and retrieve
     * attributes.
     */
    enum DataRole {
        XDataRole = Qt::UserRole,
        YDataRole,
        CustomDataRole,
        LabelDataRole,
        CategoryDataRole
    };

    /**
     * Re-initializes the model with data from an arbitrary region.
     *
     * All data will be taken from the data source passed in the constructor.
     * The ProxyModel will not react on insertions or removals in one of
     * these models.
     */
    void reset(const CellRegion &region);

    /**
     * The CellRegion that data in this proxy model is taken from.
     *
     * In ODF, this is an attribute of the PlotArea, but here the proxy model
     * manages all data sets, thus it's also responsible for this attribute.
     *
     * See table:cell-range-address, ODF v1.2, §19.595
     */
    CellRegion cellRangeAddress() const;

    /**
     * Load series from ODF
     */
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context, ChartType type);
    void saveOdf(KoShapeSavingContext &context) const;

    /**
     * Returns data or properties of a data point.
     *
     * TODO: Not implemented yet. At the moment, DataSet's data and attribute
     * getter are used instead.
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * Returns properties that are global to either a data set or a category,
     * depending on the orientation.
     *
     * If @a orientation is Qt::Horizontal, this method will return properties
     * global do the data set with number @a section.
     * If @a orientation is Qt::Vertical, it will return properties global to
     * the category with index @a section.
     *
     * TODO: Not implemented yet. At the moment, DataSet's data and attribute
     * getter are used instead.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    /**
     * Returns the number of data sets in this model.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * Returns maximum the number of data points the data sets have.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // The following methods are specific to the chart
    void setFirstRowIsLabel(bool b);
    void setFirstColumnIsLabel(bool b);
    void setDataDirection(Qt::Orientation orientation);
    void setDataDimensions(int dimensions);

    bool firstRowIsLabel() const;
    bool firstColumnIsLabel() const;
    Qt::Orientation dataDirection();

    /**
     * @see setCategoryDataRegion()
     */
    CellRegion categoryDataRegion() const;

    /**
     * Sets the region to use for categories, i.e. the labels for a certain
     * index in all data sets. This is what will be used to label points
     * on the x axis in a cartesian chart and what will be used as legend
     * items in a polar chart.
     */
    void setCategoryDataRegion(const CellRegion &region);

    /**
     * A list of all data sets that are currently being used in the chart.
     */
    QList<DataSet *> dataSets() const;

    /**
     * Insert @p count number of rows at position @p row.
     *
     * Effectively adds DataSet(s) to the model
     */
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    /**
     * Remove @p count number of rows at position @p row.
     *
     * Effectively removes DataSet(s) from the model
     */
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    /**
     * Clears the list of data sets, but keeps them in a list of "removed"
     * data sets for the next time that reset() is called. The latter list
     * will be re-used so that properties of data sets don't get lost.
     */
    void invalidateDataSets();

    /**
     * Called by ChartShape when it begins loading from ODF.
     *
     * The proxy model will then avoid any insertion, removal or data-change
     * signals or calls to speed up loading (significantly for large amounts
     * of data).
     *
     * Properties like firstRowIsLabel() can still be modified, the changes
     * will simply not get propagated until endLoading() is called.
     */
    void beginLoading();

    /**
     * Called by ChartShape when it is done loading from ODF.
     *
     * The proxy model will then be reset with its current properties.
     */
    void endLoading();

    /**
     * Returns true if beginLoading() got called and endLoading() not
     * yet what means we are in a longer loading process.
     */
    bool isLoading() const;

    /**
     * Returns true if changes to datasets or data does not result in auto calculating changes
     */
    bool manualControl();

public Q_SLOTS:
    /**
     * Connected to dataChanged() signal of source models in TableSource.
     */
    virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    /**
     * Called by the TableSource whenever a table is added to it.
     *
     * TODO: It might improve performance if tables are only added when
     * they are really in use. That is not necessarily the case if they
     * are in the TableSource.
     */
    void addTable(KoChart::Table *table);

    /**
     * Called by the TableSource whenever a table is removed from it.
     */
    void removeTable(KoChart::Table *table);

    /**
     * Set manual control to @p value
     *
     * If false, dataset structure is recalculated on changes to base data.
     */
    void setManualControl(bool value);

Q_SIGNALS:
    void dataChanged();

private:
    void addDataSet(int pos);

private:
    class Private;
    Private *const d;
};

} // namespace KoChart

#endif // KCHART_PROXYMODEL_H
