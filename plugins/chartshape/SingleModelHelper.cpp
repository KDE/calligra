/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2010 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "SingleModelHelper.h"

// Qt
#include <QAbstractItemModel>

// KoChart
#include "CellRegion.h"
#include "ChartProxyModel.h"
#include "TableSource.h"

using namespace KoChart;

SingleModelHelper::SingleModelHelper(Table *table, ChartProxyModel *proxyModel)
    : m_table(table)
    , m_proxyModel(proxyModel)
{
    Q_ASSERT(table);
    Q_ASSERT(proxyModel);

    QAbstractItemModel *model = table->model();
    connect(model, &QAbstractItemModel::modelReset, this, &SingleModelHelper::slotModelStructureChanged);
    connect(model, &QAbstractItemModel::rowsInserted, this, &SingleModelHelper::slotModelStructureChanged);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &SingleModelHelper::slotModelStructureChanged);
    connect(model, &QAbstractItemModel::columnsInserted, this, &SingleModelHelper::slotModelStructureChanged);
    connect(model, &QAbstractItemModel::columnsRemoved, this, &SingleModelHelper::slotModelStructureChanged);

    // Initialize the proxy with this model
    slotModelStructureChanged();
}

void SingleModelHelper::slotModelStructureChanged()
{
    QAbstractItemModel *model = m_table->model();
    const int columnCount = model->columnCount();
    const int rowCount = model->rowCount();
    CellRegion region(m_table);
    if (columnCount >= 1 && rowCount >= 1) {
        QPoint topLeft(1, 1);
        QPoint bottomRight(columnCount, rowCount);
        region.add(QRect(topLeft, bottomRight));
    }
    m_proxyModel->reset(region);
}
