/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __KODELETEDCELLDATA_H__
#define __KODELETEDCELLDATA_H__

#include <QTextDocumentFragment>
#include <QTextTableCellFormat>

class KoDeletedCellData
{
public:
    KoDeletedCellData(int rowNumber, int columnNumber);

    ~KoDeletedCellData();

    int rowNumber() const;

    int columnNumber() const;

    void setCellFormat(const QTextTableCellFormat &cellFormat);

    const QTextTableCellFormat &cellFormat() const;

    void setCellContent(const QTextDocumentFragment &cellContent);

    const QTextDocumentFragment &cellContent() const;

private:
    int row_number;
    int column_number;
    QTextTableCellFormat cell_format;
    QTextDocumentFragment cell_content;
};
#endif
