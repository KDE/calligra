/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2009 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ModelObserver.h"
#include <QAbstractItemModel>
#include <QDebug>

ModelObserver::ModelObserver(QAbstractItemModel *source)
    : QObject()
{
    m_source = source;
    m_numRows = 0;
    m_numCols = 0;
    m_lastDataChange.valid = false;
    m_lastHeaderDataChange.valid = false;

    connect(source, &QAbstractItemModel::rowsInserted, this, &ModelObserver::slotRowsInserted);
    connect(source, &QAbstractItemModel::columnsInserted, this, &ModelObserver::slotColumnsInserted);
    connect(source, &QAbstractItemModel::rowsRemoved, this, &ModelObserver::slotRowsRemoved);
    connect(source, &QAbstractItemModel::columnsRemoved, this, &ModelObserver::slotColumnsRemoved);
    connect(source, &QAbstractItemModel::headerDataChanged, this, &ModelObserver::slotHeaderDataChanged);
    connect(source, &QAbstractItemModel::dataChanged, this, &ModelObserver::slotDataChanged);
    connect(source, &QAbstractItemModel::modelReset, this, &ModelObserver::slotModelReset);
}

void ModelObserver::slotRowsInserted(const QModelIndex & /*parent*/, int start, int end)
{
    Q_ASSERT(start <= end);

    m_numRows += end - start + 1;

    qDebug() << "m_numRows: " << m_numRows;
}

void ModelObserver::slotColumnsInserted(const QModelIndex & /*parent*/, int start, int end)
{
    Q_ASSERT(start <= end);

    m_numCols += end - start + 1;

    qDebug() << "m_numCols: " << m_numCols;
}

void ModelObserver::slotRowsRemoved(const QModelIndex & /*parent*/, int start, int end)
{
    Q_ASSERT(start <= end);
    Q_ASSERT(end < m_numRows);

    m_numRows -= end - start + 1;

    qDebug() << "m_numRows: " << m_numRows;
}

void ModelObserver::slotColumnsRemoved(const QModelIndex & /*parent*/, int start, int end)
{
    Q_ASSERT(start <= end);
    Q_ASSERT(end < m_numCols);

    m_numCols -= end - start + 1;

    qDebug() << "m_numCols: " << m_numCols;
}

void ModelObserver::slotModelReset()
{
    qDebug() << "TestModel was reset";
    m_numRows = m_source->rowCount();
    m_numCols = m_source->columnCount();
}

void ModelObserver::slotHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    m_lastHeaderDataChange.orientation = orientation;
    m_lastHeaderDataChange.first = first;
    m_lastHeaderDataChange.last = last;
    m_lastHeaderDataChange.valid = true;
}

void ModelObserver::slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    m_lastDataChange.topLeft = topLeft;
    m_lastDataChange.bottomRight = bottomRight;
    m_lastHeaderDataChange.valid = true;
}
