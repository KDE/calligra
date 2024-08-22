/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoDeletedRowColumnDataStore.h"

#include "KoDeletedColumnData.h"
#include "KoDeletedRowData.h"

KoDeletedRowColumnDataStore::KoDeletedRowColumnDataStore() = default;

KoDeletedRowColumnDataStore::~KoDeletedRowColumnDataStore() = default;

KoDeletedRowData *KoDeletedRowColumnDataStore::addDeletedRow(QTextTable *table, int rowNumber, int changeId)
{
    KoDeletedRowData *deletedRowData = new KoDeletedRowData(rowNumber);
    deletedRowDataMap.insert(changeId, deletedRowData);
    QVector<int> *tableChangeIds = tableChangeIdsMap.value(table, nullptr);
    if (!tableChangeIds) {
        tableChangeIds = new QVector<int>();
        tableChangeIdsMap.insert(table, tableChangeIds);
    }
    tableChangeIds->push_back(changeId);
    return deletedRowData;
}

KoDeletedColumnData *KoDeletedRowColumnDataStore::addDeletedColumn(QTextTable *table, int columnNumber, int changeId)
{
    KoDeletedColumnData *deletedColumnData = new KoDeletedColumnData(columnNumber);
    deletedColumnDataMap.insert(changeId, deletedColumnData);
    QVector<int> *tableChangeIds = tableChangeIdsMap.value(table, nullptr);
    if (!tableChangeIds) {
        tableChangeIds = new QVector<int>();
        tableChangeIdsMap.insert(table, tableChangeIds);
    }
    tableChangeIds->push_back(changeId);
    return deletedColumnData;
}

const QVector<int> *KoDeletedRowColumnDataStore::deletedRowColumnChangeIds(QTextTable *table)
{
    return tableChangeIdsMap.value(table, nullptr);
}

KoDeletedRowColumnDataStore::DeleteType KoDeletedRowColumnDataStore::deleteType(int changeId)
{
    KoDeletedRowColumnDataStore::DeleteType retValue;
    if (deletedRowDataMap.value(changeId, nullptr)) {
        retValue = KoDeletedRowColumnDataStore::eDeletedRow;
    } else if (deletedColumnDataMap.value(changeId, nullptr)) {
        retValue = KoDeletedRowColumnDataStore::eDeletedColumn;
    } else {
        retValue = KoDeletedRowColumnDataStore::eUnknownDeleteType;
    }

    return retValue;
}

KoDeletedRowData *KoDeletedRowColumnDataStore::deletedRowData(int changeId)
{
    return deletedRowDataMap.value(changeId, nullptr);
}

KoDeletedColumnData *KoDeletedRowColumnDataStore::deletedColumnData(int changeId)
{
    return deletedColumnDataMap.value(changeId, nullptr);
}
