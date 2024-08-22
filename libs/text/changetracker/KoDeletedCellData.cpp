/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoDeletedCellData.h"

KoDeletedCellData::KoDeletedCellData(int rowNumber, int columnNumber)
{
    this->row_number = rowNumber;
    this->column_number = columnNumber;
}

KoDeletedCellData::~KoDeletedCellData() = default;

int KoDeletedCellData::rowNumber() const
{
    return row_number;
}

int KoDeletedCellData::columnNumber() const
{
    return column_number;
}

void KoDeletedCellData::setCellFormat(const QTextTableCellFormat &cellFormat)
{
    this->cell_format = cellFormat;
}

const QTextTableCellFormat &KoDeletedCellData::cellFormat() const
{
    return this->cell_format;
}

void KoDeletedCellData::setCellContent(const QTextDocumentFragment &cellContent)
{
    this->cell_content = cellContent;
}

const QTextDocumentFragment &KoDeletedCellData::cellContent() const
{
    return this->cell_content;
}
