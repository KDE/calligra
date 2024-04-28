/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __KODELETEDROWCOLUMNDATASTORE_H__
#define __KODELETEDROWCOLUMNDATASTORE_H__

#include <QMap>
#include <QVector>

class KoDeletedRowData;
class KoDeletedColumnData;
class QTextTable;

class KoDeletedRowColumnDataStore
{
public:
    typedef enum { eDeletedRow, eDeletedColumn, eUnknownDeleteType } DeleteType;

    KoDeletedRowColumnDataStore();

    ~KoDeletedRowColumnDataStore();

    KoDeletedRowData *addDeletedRow(QTextTable *table, int rowNumber, int changeId);

    KoDeletedColumnData *addDeletedColumn(QTextTable *table, int columnNumber, int changeId);

    const QVector<int> *deletedRowColumnChangeIds(QTextTable *table);

    DeleteType deleteType(int changeId);

    KoDeletedRowData *deletedRowData(int changeId);

    KoDeletedColumnData *deletedColumnData(int changeId);

private:
    QMap<QTextTable *, QVector<int> *> tableChangeIdsMap;

    QMap<int, KoDeletedRowData *> deletedRowDataMap;

    QMap<int, KoDeletedColumnData *> deletedColumnDataMap;
};
#endif
