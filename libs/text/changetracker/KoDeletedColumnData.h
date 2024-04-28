/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __KODELETEDCOLUMNDATA_H__
#define __KODELETEDCOLUMNDATA_H__

#include <QVector>

class KoDeletedCellData;
class KoTableColumnStyle;
class QTextTable;

class KoDeletedColumnData
{
public:
    explicit KoDeletedColumnData(int columnNumber);

    ~KoDeletedColumnData();

    int columnNumber();

    void setColumnStyle(KoTableColumnStyle *columnStyle);

    KoTableColumnStyle *columnStyle();

    const QVector<KoDeletedCellData *> &deletedCells();

    void storeDeletedCells(QTextTable *table);

private:
    int column_number;

    KoTableColumnStyle *column_style;

    QVector<KoDeletedCellData *> deleted_cells;
};

#endif
