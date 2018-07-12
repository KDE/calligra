/* This file is part of the KDE project
 * 
 * Copyright 2018 Dag Andersen <danders@get2net.dk>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <DataSetTableModel.h>

#include "ChartShape.h"
#include "ChartProxyModel.h"
#include "ChartTableView.h"
#include "DataSet.h"
#include "ChartDebug.h"

#include <KLocalizedString>

#include <QObject>
#include <QModelIndex>

using namespace KoChart;

DataSetTableModel::DataSetTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , chartModel(0)
    , tableSource(0)
{
    
}


Qt::ItemFlags DataSetTableModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

int DataSetTableModel::columnCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return 4; // name, x, y, size
}

int DataSetTableModel::rowCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return chartModel ? chartModel->rowCount() : 0;
}

QVariant DataSetTableModel::headerData(int section, Qt::Orientation orientation, int role/* = Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Vertical) {
            return section + 1;
        } else if (orientation == Qt::Horizontal) {
            switch (section) {
                case 0: return i18n("Dataset Name");
                case 1: return i18n("X-Values");
                case 2: return i18n("Y-Values");
                case 3: return i18n("Bubble Size");
            }
        }
    }
    return QVariant();
}

QVariant DataSetTableModel::data(const QModelIndex &idx, int role/* = Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole) {
        if (!chartModel) {
            return QVariant();
        }
        DataSet *ds = chartModel->dataSets().value(idx.row());
        if (!ds) {
            return QVariant();
        }
        switch (idx.column()) {
            case 0: return ds->labelData();
            case 1: {
                QStringList s = ds->xDataRegion().toString().split('.', QString::SkipEmptyParts);
                return s.value(1).remove('$');
            }
            case 2: {
                QStringList s = ds->yDataRegion().toString().split('.', QString::SkipEmptyParts);
                return s.value(1).remove('$');
            }
            case 3: {
                QStringList s = ds->customDataRegion().toString().split('.', QString::SkipEmptyParts);
                return s.value(1).remove('$');
            }
        }
    }
    return QVariant();
}

bool DataSetTableModel::setData(const QModelIndex &index, const QVariant &value, int role/* = Qt::EditRole*/)
{
    if (role == Qt::EditRole) {
        DataSet *ds = chartModel->dataSets().value(index.row());
        if (!ds) {
            return false;
        }
        if (submitData(index, value, role)) {
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

ChartProxyModel *DataSetTableModel::model() const
{
    return chartModel;
}

void DataSetTableModel::setModel(QAbstractItemModel *m)
{
    if (chartModel) {
        disconnect(this);
    }
    chartModel = qobject_cast<ChartProxyModel*>(m);
    connect(chartModel, SIGNAL(dataChanged()), this, SLOT(chartModelChanged()));
    connect(chartModel, SIGNAL(modelReset()), this, SLOT(chartModelChanged()));
//     connect(chartModel, &ChartProxyModel::dataChanged, this, &DataSetTableModel::chartModelChanged);
    Q_ASSERT(chartModel);
}

void DataSetTableModel::chartModelChanged()
{
    beginResetModel();
    endResetModel();
}

bool DataSetTableModel::submitData(const QModelIndex &idx, const QVariant &value, int role)
{
    DataSet *ds = chartModel->dataSets().value(idx.row());
    QString s = ds->customDataRegion().toString().split('.').value(0)+ '.'; // table name
    CellRegion region(tableSource, s + value.toString());
    debugChartUiBubble<<region.toString()<<ds;
    switch (idx.column()) {
        case 0:
            
            break;
        case 1:
            ds->setXDataRegion(region);
            return true;
        case 2:
            ds->setYDataRegion(region);
            return true;
        case 3:
            ds->setCustomDataRegion(region);
            return true;
    }
    return false;
}

