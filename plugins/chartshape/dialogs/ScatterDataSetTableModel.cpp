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

#include <ScatterDataSetTableModel.h>

#include "ChartShape.h"
#include "ChartProxyModel.h"
#include "ChartTableView.h"
#include "DataSet.h"
#include "TableSource.h"
#include "ChartDebug.h"

#include <KLocalizedString>

#include <QObject>
#include <QModelIndex>
#include <QComboBox>
#include <QAbstractProxyModel>

namespace KoChart {
namespace Scatter {
    enum Roles {
        LabelTextRole = Qt::UserRole + 1
    };
}
}

using namespace KoChart;
using namespace Scatter;

LabelColumnDelegate::LabelColumnDelegate(QObject *parent)
: QStyledItemDelegate(parent)
, dataModel(0)
{
}

QWidget *LabelColumnDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->setEditable(true);
    editor->setInsertPolicy(QComboBox::InsertAtTop);
    editor->installEventFilter(const_cast<LabelColumnDelegate*>(this));
    return editor;
}

void LabelColumnDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStringList lst;
    lst << QString();
    QString s = index.data().toString();
    if (!s.isEmpty()) {
        lst << s;
    }
    QAbstractProxyModel *pm = qobject_cast<QAbstractProxyModel*>(dataModel);
    for (int i = 2; i < pm->sourceModel()->columnCount(); ++i) {
        lst << pm->sourceModel()->index(0, i).data().toString();
    }
    QComboBox *box = static_cast<QComboBox*>(editor);
    box->addItems( lst );
    box->setCurrentText(index.data().toString());
}

void LabelColumnDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *box = static_cast<QComboBox*>(editor);
    debugChartUiScatter<<box->currentIndex()<<box->currentText();
    switch(box->currentIndex()) {
        case 0: // new text
            model->setData(index, box->currentText(), LabelTextRole);
            break;
        case 1: // current text
            break;
        default:
            // point to new cellregion
            model->setData(index, box->currentIndex()+1, Qt::EditRole);
            break;
    }
}

void LabelColumnDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    QRect r = option.rect;
    editor->setGeometry(r);
}

DataColumnDelegate::DataColumnDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , dataModel(0)
{
}

QWidget *DataColumnDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
    QComboBox *editor = new QComboBox(parent);
    editor->setEditable(true);
    editor->setInsertPolicy(QComboBox::InsertAtTop);
    editor->installEventFilter(const_cast<DataColumnDelegate*>(this));
    return editor;
}

void DataColumnDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QStringList lst;
    lst << QString();
    QString s = index.data().toString();
    if (!s.isEmpty()) {
        lst << s;
    }
    for (int i = 0; i < dataModel->columnCount(); ++i) {
        lst << dataModel->headerData(i, Qt::Horizontal).toString();
    }
    QComboBox *box = static_cast<QComboBox*>(editor);
    box->addItems( lst );
    box->setCurrentText(index.data().toString());
}

void DataColumnDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *box = static_cast<QComboBox*>(editor);
    model->setData(index, box->currentText());
}

void DataColumnDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    QRect r = option.rect;
    editor->setGeometry(r);
}

DataSetTableModel::DataSetTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , chartModel(0)
    , tableSource(0)
    , blocksignals(false)
{

}

Qt::ItemFlags DataSetTableModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

int DataSetTableModel::columnCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
    Q_UNUSED(parent);
    return 3; // name, x, y
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
            }
        }
    }
    return QVariant();
}

QVariant DataSetTableModel::data(const QModelIndex &idx, int role/* = Qt::DisplayRole*/) const
{
    if (!chartModel) {
        return QVariant();
    }
    DataSet *ds = chartModel->dataSets().value(idx.row());
    if (!ds) {
        return QVariant();
    }
    if (role == Qt::DisplayRole) {
        switch (idx.column()) {
            case 0: return ds->labelData();
            case 1: {
                return ds->xDataRegion().toString();
            }
            case 2: {
                return ds->yDataRegion().toString();
            }
        }
    } else if (role == Qt::ToolTipRole) {
        switch (idx.column()) {
            case 0: {
                CellRegion region = ds->labelDataRegion();
                if (region.isValid()) {
                    return i18nc("@info:tooltip", "Label cell region: %1", region.toString());
                }
                return i18nc("@info:tooltip", "Default label: %1", ds->labelData().toString());
            }
            case 1: {
                return i18nc("@info:tooltip", "X-Values cell region: %1", ds->xDataRegion().toString());
            }
            case 2: {
                return i18nc("@info:tooltip", "Y-Values cell region: %1", ds->xDataRegion().toString());
            }
        }
    }
    return QVariant();
}

bool DataSetTableModel::setData(const QModelIndex &index, const QVariant &value, int role/* = Qt::EditRole*/)
{
    debugChartUiScatter<<index<<value<<role;
    if (role == Qt::EditRole || role == LabelTextRole) {
        if (submitData(index, value, role)) {
            chartModel->dataChanged(QModelIndex(), QModelIndex());
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
}

bool DataSetTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    debugChartUiScatter<<row<<count;
    return chartModel->removeRows(row, count, parent);
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
    Q_ASSERT(chartModel);
}

void DataSetTableModel::chartModelChanged()
{
    if (!blocksignals) {
        beginResetModel();
        endResetModel();
    }
}

bool DataSetTableModel::setRegionData(const CellRegion &region, int index, const QVariant &value, int role) const
{
    if (!region.isValid() || !region.hasPointAtIndex(index)) {
        return false;
    }
    // Convert the given index in this dataset to a data point in the source model.
    QPoint dataPoint = region.pointAtIndex(index);
    Table *table = region.table();
    Q_ASSERT(table);
    QAbstractItemModel *model = table->model();
    if (!model) {
        return false;
    }
    debugChartUiScatter<<dataPoint;
    // The top-left point is (1,1). (0,y) or (x,0) refers to header data.
    const bool verticalHeaderData = dataPoint.x() == 0;
    const bool horizontalHeaderData = dataPoint.y() == 0;
    const int row = dataPoint.y() - 1;
    const int col = dataPoint.x() - 1;

    bool status = false;
    blocksignals = true;
    if (verticalHeaderData) {
        status = model->setHeaderData(row, Qt::Vertical, value, role);
    } else if (horizontalHeaderData) {
        status = model->setHeaderData(col, Qt::Horizontal, value, role);
    } else {
        const QModelIndex &idx = model->index(row, col);
        if (idx.isValid()) {
            status = model->setData(idx, value, role);
        }
    }
    blocksignals = false;
    return status;
}

bool DataSetTableModel::submitData(const QModelIndex &idx, const QVariant &value, int role)
{
    DataSet *ds = chartModel->dataSets().value(idx.row());
    Table *table = tableSource->tableMap().first();
    if (!ds || !table) {
        warnChartUiScatter<<"No dataset or table:"<<table<<ds;
        return false;
    }
    switch (idx.column()) {
        case 0: { // labels
            if (role == LabelTextRole) {
                return setLabelText(idx, value.toString());
            }
            if (role == Qt::EditRole) {
                return setLabelCell(idx, value.toInt());
            }
            return false;
        }
        case 1: { // x-values
            if (role != Qt::EditRole) {
                return false;
            }
            if (ds->xDataRegion().table()) {
                table = ds->xDataRegion().table();
            }
            QString v = value.toString();
            if (v.length() == 1) {
                if (v.length() == 1) {
                    v = QString("%1%2:%3%4").arg(v).arg(2).arg(v).arg(table->model()->rowCount());
                }
                ds->setXDataRegion(CellRegion(tableSource, table->name() + '.' + v));
            } else {
                ds->setXDataRegion(CellRegion(tableSource, v));
            }
            return true;
        }
        case 2: { // y-values
            if (role != Qt::EditRole) {
                return false;
            }
            if (ds->xDataRegion().table()) {
                table = ds->xDataRegion().table();
            }
            QString v = value.toString();
            if (v.length() == 1) {
                v = QString("%1%2:%3%4").arg(v).arg(2).arg(v).arg(table->model()->rowCount());
                ds->setYDataRegion(CellRegion(tableSource, table->name() + '.' + v));
            } else {
                ds->setYDataRegion(CellRegion(tableSource, v));
            }
            return true;
        }
    }
    return false;
}

bool DataSetTableModel::setLabelText(const QModelIndex &idx, const QString &text)
{
    DataSet *ds = chartModel->dataSets().value(idx.row());
    if (!ds) {
        warnChartUiScatter<<"No dataset:"<<idx.row();
        return false;
    }
    CellRegion region = ds->labelDataRegion();
    if (!region.isValid()) {
        warnChartUiScatter<<"Invalid label cell region";
        return false;
    }
    debugChartUiScatter<<ds->labelData()<<':'<<region<<text;
    return setRegionData(region, 0, text);
}

bool DataSetTableModel::setLabelCell(const QModelIndex &idx, int cell)
{
    DataSet *ds = chartModel->dataSets().value(idx.row());
    Table *table = tableSource->tableMap().first();
    if (!ds || !table) {
        return false;
    }
    CellRegion region(table, QPoint(cell, 1));
    ds->setLabelDataRegion(region);
    debugChartUiScatter<<idx<<cell<<region<<ds;
    return true;
}
