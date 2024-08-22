/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "PageBreak.h"
#include "Actions.h"

#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "ui/commands/PageBreakCommand.h"

#include <KLocalizedString>

using namespace Calligra::Sheets;

PageBreakRow::PageBreakRow(Actions *actions)
    : ToggleableCellAction(actions, "format_break_before_row", i18n("Break Before Row"), QIcon(), i18n("Set a manual page break before the row"))
{
}

PageBreakRow::~PageBreakRow() = default;

QAction *PageBreakRow::createAction()
{
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Row Break"));
    return res;
}

void PageBreakRow::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    PageBreakCommand *command = new PageBreakCommand();
    command->setSheet(sheet);
    command->setMode(PageBreakCommand::BreakBeforeRow);
    command->setBreak(selected);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool PageBreakRow::checkedForSelection(Selection *selection, const Cell &)
{
    const Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it = selection->constBegin(); it != end; ++it) {
        const Sheet *const sheet = dynamic_cast<Sheet *>((*it)->sheet());
        if (!sheet)
            continue;
        const QRect range = (*it)->rect();
        const int row = range.top();
        if (sheet->rowFormats()->hasPageBreak(row))
            return true;
    }

    return false;
}

bool PageBreakRow::enabledForSelection(Selection *selection, const Cell &)
{
    const Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it = selection->constBegin(); it != end; ++it) {
        const QRect range = (*it)->rect();
        const int row = range.top();
        if (row != 1)
            return true;
    }

    return false;
}

PageBreakColumn::PageBreakColumn(Actions *actions)
    : ToggleableCellAction(actions, "format_break_before_column", i18n("Break Before Column"), QIcon(), i18n("Set a manual page break before the column"))
{
}

PageBreakColumn::~PageBreakColumn() = default;

QAction *PageBreakColumn::createAction()
{
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Column Break"));
    return res;
}

void PageBreakColumn::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    PageBreakCommand *command = new PageBreakCommand();
    command->setSheet(sheet);
    command->setMode(PageBreakCommand::BreakBeforeColumn);
    command->setBreak(selected);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool PageBreakColumn::checkedForSelection(Selection *selection, const Cell &)
{
    const Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it = selection->constBegin(); it != end; ++it) {
        const Sheet *const sheet = dynamic_cast<Sheet *>((*it)->sheet());
        if (!sheet)
            continue;
        const QRect range = (*it)->rect();
        const int column = range.left();
        if (sheet->columnFormats()->hasPageBreak(column))
            return true;
    }

    return false;
}

bool PageBreakColumn::enabledForSelection(Selection *selection, const Cell &)
{
    const Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it = selection->constBegin(); it != end; ++it) {
        const QRect range = (*it)->rect();
        const int column = range.left();
        if (column > 1)
            return true;
    }

    return false;
}
