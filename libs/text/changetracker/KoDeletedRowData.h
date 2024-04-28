/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __KODELETEDROWDATA_H__
#define __KODELETEDROWDATA_H__

#include <QVector>

class KoDeletedCellData;
class KoTableRowStyle;
class QTextTable;

class KoDeletedRowData
{
public:
    explicit KoDeletedRowData(int rowNumber);

    ~KoDeletedRowData();

    int rowNumber();

    void setRowStyle(KoTableRowStyle *rowStyle);

    KoTableRowStyle *rowStyle();

    const QVector<KoDeletedCellData *> &deletedCells();

    void storeDeletedCells(QTextTable *table);

private:
    int row_number;

    KoTableRowStyle *row_style;

    QVector<KoDeletedCellData *> deleted_cells;
};

#endif
