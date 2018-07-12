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

#ifndef KOCHART_DATASETTABLEMODEL_H
#define KOCHART_DATASETTABLEMODEL_H

#include <QAbstractTableModel>

#include "ChartShape.h"
#include "ChartProxyModel.h"
#include "ChartTableView.h"
#include "DataSet.h"
#include "ChartDebug.h"


#include <QObject>
#include <QModelIndex>

namespace KoChart {

class DataSetTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DataSetTableModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    void setModel(QAbstractItemModel *m);
    ChartProxyModel *model() const;

protected Q_SLOTS:
    void chartModelChanged();

protected:
    bool submitData(const QModelIndex &idx, const QVariant &value, int role);

public:
    ChartProxyModel *chartModel;
    TableSource *tableSource;
};


} // namespace KoChart

#endif //KOCHART_DATASETTABLEMODEL_H
