/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCHART_SCATTERDATASETTABLEMODEL_H
#define KOCHART_SCATTERDATASETTABLEMODEL_H

#include <QAbstractTableModel>

#include "ChartDebug.h"
#include "ChartProxyModel.h"
#include "ChartShape.h"
#include "ChartTableView.h"
#include "DataSet.h"

#include <QModelIndex>
#include <QObject>
#include <QStyledItemDelegate>

namespace KoChart
{
namespace Scatter
{

class DataColumnDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DataColumnDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

public:
    QAbstractItemModel *dataModel;
};

class DataSetTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DataSetTableModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    void setModel(QAbstractItemModel *m);
    ChartProxyModel *model() const;

protected Q_SLOTS:
    void chartModelChanged();

    void emitDataChanged();

protected:
    bool submitData(const QModelIndex &idx, const QVariant &value, int role);
    bool setRegionData(const CellRegion &region, int index, const QVariant &value, int role = Qt::EditRole) const;
    bool setLabelText(const QModelIndex &idx, const QString &text);
    bool setLabelCell(const QModelIndex &idx, int cell);

public:
    ChartProxyModel *chartModel;
    TableSource *tableSource;
};

} // namespace Scatter

} // namespace KoChart

#endif // KOCHART_SCATTERDATASETTABLEMODEL_H
