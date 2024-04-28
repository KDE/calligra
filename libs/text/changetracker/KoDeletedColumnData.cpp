/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoDeletedColumnData.h"

#include <QTextCursor>
#include <QTextTable>

#include "KoDeletedCellData.h"

#include <styles/KoTableColumnStyle.h>

KoDeletedColumnData::KoDeletedColumnData(int columnNumber)
{
    this->column_number = columnNumber;
}

KoDeletedColumnData::~KoDeletedColumnData()
{
    KoDeletedCellData *cellData;
    foreach (cellData, deleted_cells) {
        delete cellData;
    }
}

int KoDeletedColumnData::columnNumber()
{
    return column_number;
}

void KoDeletedColumnData::setColumnStyle(KoTableColumnStyle *columnStyle)
{
    this->column_style = columnStyle;
}

KoTableColumnStyle *KoDeletedColumnData::columnStyle()
{
    return column_style;
}

const QVector<KoDeletedCellData *> &KoDeletedColumnData::deletedCells()
{
    return deleted_cells;
}

void KoDeletedColumnData::storeDeletedCells(QTextTable *table)
{
    QTextCursor cursor(table->document());
    int rows = table->rows();

    for (int i = 0; i < rows; i++) {
        KoDeletedCellData *cellData = new KoDeletedCellData(i, column_number);
        QTextTableCell cell = table->cellAt(i, column_number);
        cursor.setPosition(cell.firstCursorPosition().position());
        cursor.setPosition(cell.lastCursorPosition().position(), QTextCursor::KeepAnchor);
        cellData->setCellFormat(cell.format().toTableCellFormat());
        cellData->setCellContent(cursor.selection());
        deleted_cells.push_back(cellData);
    }
}
