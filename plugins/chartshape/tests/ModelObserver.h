/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2009 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_MODELOBSERVER_H
#define KCHART_MODELOBSERVER_H

#include <QModelIndex>
#include <QObject>

class QAbstractItemModel;

struct HeaderDataChange {
    Qt::Orientation orientation;
    int first;
    int last;
    bool valid;
};

struct DataChange {
    QModelIndex topLeft;
    QModelIndex bottomRight;
    bool valid;
};

class ModelObserver : public QObject
{
    Q_OBJECT

public:
    ModelObserver(QAbstractItemModel *source);

private Q_SLOTS:
    void slotRowsInserted(const QModelIndex &parent, int start, int end);
    void slotColumnsInserted(const QModelIndex &parent, int start, int end);
    void slotRowsRemoved(const QModelIndex &parent, int start, int end);
    void slotColumnsRemoved(const QModelIndex &parent, int start, int end);
    void slotHeaderDataChanged(Qt::Orientation, int first, int last);
    void slotDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void slotModelReset();

public:
    QAbstractItemModel *m_source;
    int m_numRows;
    int m_numCols;
    HeaderDataChange m_lastHeaderDataChange;
    DataChange m_lastDataChange;
};

#endif // KCHART_MODELOBSERVER_H
