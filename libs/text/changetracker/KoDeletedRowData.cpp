/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoDeletedRowData.h"

#include <QTextCursor>
#include <QTextTable>

#include "KoDeletedCellData.h"

#include <styles/KoTableRowStyle.h>

KoDeletedRowData::KoDeletedRowData(int rowNumber)
{
    this->row_number = rowNumber;
}

KoDeletedRowData::~KoDeletedRowData()
{
    KoDeletedCellData *cellData;
    foreach (cellData, deleted_cells) {
        delete cellData;
    }
}

int KoDeletedRowData::rowNumber()
{
    return row_number;
}

void KoDeletedRowData::setRowStyle(KoTableRowStyle *rowStyle)
{
    this->row_style = rowStyle;
}

KoTableRowStyle *KoDeletedRowData::rowStyle()
{
    return row_style;
}

const QVector<KoDeletedCellData *> &KoDeletedRowData::deletedCells()
{
    return deleted_cells;
}

void KoDeletedRowData::storeDeletedCells(QTextTable *table)
{
    QTextCursor cursor(table->document());
    int columns = table->columns();

    for (int i = 0; i < columns; i++) {
        KoDeletedCellData *cellData = new KoDeletedCellData(row_number, i);
        QTextTableCell cell = table->cellAt(row_number, i);
        cursor.setPosition(cell.firstCursorPosition().position());
        cursor.setPosition(cell.lastCursorPosition().position(), QTextCursor::KeepAnchor);
        cellData->setCellFormat(cell.format().toTableCellFormat());
        cellData->setCellContent(cursor.selection());
        deleted_cells.push_back(cellData);
    }
}
