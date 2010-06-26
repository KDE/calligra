/* This file is part of the KDE project
   Copyright 2007,2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1999-2007 The KSpread Team <koffice-devel@kde.org>
   Copyright 1998,1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "CopyCommand.h"

#include "CellStorage.h"
#include "Region.h"
#include "RowColumnFormat.h"
#include "Sheet.h"

using namespace KSpread;

// era: encode references absolutely
QDomDocument CopyCommand::saveAsXml(const Region& region, bool era)
{
    QDomDocument xmlDoc("spreadsheet-snippet");
    xmlDoc.appendChild(xmlDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement root = xmlDoc.createElement("spreadsheet-snippet");
    xmlDoc.appendChild(root);

    // find the upper left corner of the selection
    const QRect boundingRect = region.boundingRect();
    int left = boundingRect.left();
    int top = boundingRect.top();

    // for tiling the clipboard content in the selection
    root.setAttribute("rows", boundingRect.height());
    root.setAttribute("columns", boundingRect.width());

    const Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
        Sheet *const sheet = (*it)->sheet();
        const QRect range = (*it)->rect();
        CellStorage *const storage = sheet->cellStorage();

        //
        // Entire rows selected?
        //
        if ((*it)->isRow()) {
            QDomElement rows = xmlDoc.createElement("rows");
            rows.setAttribute("count", range.height());
            rows.setAttribute("row", range.top() - top + 1);
            root.appendChild(rows);

            // Save all cells.
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell = storage->firstInRow(row);
                for (; !cell.isNull(); cell = storage->nextInRow(cell.column(), cell.row())) {
                    if (!cell.isPartOfMerged()) {
                        root.appendChild(cell.save(xmlDoc, 0, top - 1, era));
                    }
                }
            }

            // TODO Stefan: Inefficient, use cluster functionality
            // Save the row formats if there are any
            const RowFormat* format;
            for (int row = range.top(); row <= range.bottom(); ++row) {
                format = sheet->rowFormat(row);
                if (format && !format->isDefault()) {
                    QDomElement e = format->save(xmlDoc, top - 1);
                    if (!e.isNull()) {
                        rows.appendChild(e);
                    }
                }
            }
            continue;
        }

        //
        // Entire columns selected?
        //
        if ((*it)->isColumn()) {
            QDomElement columns = xmlDoc.createElement("columns");
            columns.setAttribute("count", range.width());
            columns.setAttribute("column", range.left() - left + 1);
            root.appendChild(columns);

            // Save all cells.
            for (int col = range.left(); col <= range.right(); ++col) {
                Cell cell = storage->firstInColumn(col);
                for (; !cell.isNull(); cell = storage->nextInColumn(cell.column(), cell.row())) {
                    if (!cell.isPartOfMerged()) {
                        root.appendChild(cell.save(xmlDoc, left - 1, 0, era));
                    }
                }
            }

            // TODO Stefan: Inefficient, use the cluster functionality
            // Save the column formats if there are any
            const ColumnFormat* format;
            for (int col = range.left(); col <= range.right(); ++col) {
                format = sheet->columnFormat(col);
                if (format && !format->isDefault()) {
                    QDomElement e = format->save(xmlDoc, left - 1);
                    if (!e.isNull()) {
                        columns.appendChild(e);
                    }
                }
            }
            continue;
        }

        // Save all cells.
        Cell cell;
        for (int row = range.top(); row <= range.bottom(); ++row) {
            if (range.left() == 1) {
                cell = storage->firstInRow(row);
            } else {
                cell = storage->nextInRow(range.left() - 1, row);
            }
            while (!cell.isNull() && cell.column() >= range.left() && cell.column() <= range.right()) {
                if (!cell.isPartOfMerged()) {
                    root.appendChild(cell.save(xmlDoc, left - 1, top - 1, era));
                }
                cell = storage->nextInRow(cell.column(), cell.row());
            }
        }
    }
    return xmlDoc;
}

static QString cellAsText(const Cell& cell, bool addTab)
{
    QString result;
    if (!cell.isDefault()) {
        result += cell.displayText();
    }
    if (addTab) {
        result += '\t';
    }
    return result;
}

QString CopyCommand::saveAsPlainText(const Region &region)
{
    // Only one cell selected? => copy active cell
    if (region.isSingular()) {
        const Cell cell(region.firstSheet(), region.firstRange().topLeft());
        return cell.displayText();
    }

    QRect lastRange(region.lastRange());

    // Find area
    int top = lastRange.bottom();
    int bottom = lastRange.top();
    int left = lastRange.right();
    int right = lastRange.left();

    QString result;
    for (int row = top; row <= bottom; ++row) {
        for (int col = left; col <= right; ++col) {
            Cell cell(region.lastSheet(), col, row);
            result += cellAsText(cell, col != right);
        }
        result += '\n';
    }
    return result;
}

QDomDocument CopyCommand::saveAsHtml(const Region &region)
{
    QDomDocument doc("spreadsheet-html");
    QDomElement html = doc.createElement("html");
    doc.appendChild(html);
    QDomElement body = doc.createElement("body");
    html.appendChild(body);
    QDomElement table = doc.createElement("table");
    body.appendChild(table);

    const Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        Sheet *const sheet = (*it)->sheet();
        const QRect range = (*it)->rect();

        // TODO
        Q_UNUSED(sheet);
        Q_UNUSED(range);
    }
    return doc;
}

QString CopyCommand::saveAsCSV(const Region &region)
{
    // TODO
    Q_UNUSED(region);
    return QString();
}
