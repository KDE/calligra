/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1999-2007 The KSpread Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CopyCommand.h"

#include "core/CellStorage.h"
#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"

using namespace Calligra::Sheets;

QString CopyCommand::saveAsSnippet(const Region &region)
{
    QString res;
    Region::ConstIterator it;
    for (it = region.constBegin(); it != region.constEnd(); ++it) {
        SheetBase *const sheet = (*it)->sheet();
        const QRect range = (*it)->rect();
        int x1 = range.left();
        int y1 = range.top();
        int x2 = range.right();
        int y2 = range.bottom();
        QString entryName = "range";
        if ((*it)->isRow())
            entryName = "row";
        if ((*it)->isColumn())
            entryName = "column";
        res += entryName + " " + QString::number(x1) + " " + QString::number(y1) + " " + QString::number(x2) + " " + QString::number(y2) + " "
            + sheet->sheetName() + "\n";
    }
    return res;
}

static QString cellAsText(const Cell &cell, bool addTab)
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
        Sheet *sheet = dynamic_cast<Sheet *>(region.firstSheet());
        const Cell cell(sheet, region.firstRange().topLeft());
        return cell.displayText();
    }

    QString result;
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        if (result.length())
            result += QLatin1Char('\n');
        Region::Element *el = *it;
        Sheet *sheet = dynamic_cast<Sheet *>(el->sheet());
        QRect used = sheet->usedArea(true);
        QRect rect = el->rect().intersected(used);
        for (int row = rect.top(); row <= rect.bottom(); ++row) {
            for (int col = rect.left(); col <= rect.right(); ++col) {
                Cell cell(sheet, col, row);
                result += cellAsText(cell, col != rect.right());
            }
            result += QLatin1Char('\n');
        }
    }
    return result;
}
