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

#include <BubbleDataSetTableModel.h>

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

using namespace KoChart;

using namespace Bubble;

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
                return ds->xDataRegion().toString();
            }
            case 2: {
                return ds->yDataRegion().toString();
            }
            case 3: {
                return ds->customDataRegion().toString();
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
    Table *table = tableSource->tableMap().first();
    switch (idx.column()) {
        case 0:
            
            break;
        case 1: {
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
        case 2: {
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
        case 3: {
            if (ds->xDataRegion().table()) {
                table = ds->xDataRegion().table();
            }
            QString v = value.toString();
            if (v.length() == 1) {
                v = QString("%1%2:%3%4").arg(v).arg(2).arg(v).arg(table->model()->rowCount());
                ds->setCustomDataRegion(CellRegion(tableSource, table->name() + '.' + v));
            } else {
                ds->setCustomDataRegion(CellRegion(tableSource, v));
            }
            return true;
        }
    }
    return false;
}

