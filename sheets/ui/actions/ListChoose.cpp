/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ListChoose.h"
#include "Actions.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "engine/ValueStorage.h"
#include "ui/CellToolBase.h"
#include "ui/CellView.h"
#include "ui/SheetView.h"
#include "ui/commands/DataManipulators.h"

#include <KLocalizedString>

#include <QMenu>

// This one copies the alt+down selector from LO, but isn't particularly useful right now. Improvements needed.

using namespace Calligra::Sheets;

ListChoose::ListChoose(Actions *actions)
    : CellAction(actions, "listChoose", i18n("Selection List..."), QIcon(), QString())
    , popupListChoose(nullptr)
{
}

ListChoose::~ListChoose()
{
    delete popupListChoose;
}

void ListChoose::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;
    const Cell cursorCell(sheet, selection->cursor());
    QStringList itemList = items(selection, cursorCell);
    if (!itemList.length())
        return;

    delete popupListChoose;
    popupListChoose = new QMenu();

    for (QString &item : itemList)
        popupListChoose->addAction(item);

    QPoint cursor = selection->cursor();
    double tx = sheet->columnPosition(cursor.x());
    double ty = sheet->rowPosition(cursor.y());
    double h = cursorCell.height();
    CellToolBase *tool = m_actions->tool();
    if (tool->sheetView(sheet)->obscuresCells(cursor)) {
        const CellView &cellView = tool->sheetView(sheet)->cellView(cursor.x(), cursor.y());
        h = cellView.cellHeight();
    }
    ty += h;

    if (sheet->layoutDirection() == Qt::RightToLeft) {
        tx = canvasWidget->width() - tx;
    }

    QPoint p((int)tx, (int)ty);
    QPoint p2 = canvasWidget->mapToGlobal(p);

    if (sheet->layoutDirection() == Qt::RightToLeft) {
        p2.setX(p2.x() - popupListChoose->sizeHint().width() + 1);
    }

    popupListChoose->popup(p2);
    connect(popupListChoose, &QMenu::triggered, this, &ListChoose::itemSelected);
}

QStringList ListChoose::items(Selection *selection, const Cell &cursorCell, int limit)
{
    Sheet *sheet = selection->activeSheet();
    CellStorage *storage = sheet->fullCellStorage();
    ValueStorage *values = storage->valueStorage();
    int col = cursorCell.column();
    QStringList res;

    const Region::ConstIterator end(selection->constEnd());
    for (Region::ConstIterator it(selection->constBegin()); it != end; ++it) {
        const QRect range = (*it)->rect();
        if (col < range.left() || col > range.right())
            continue; // next range

        Value val;
        int row = range.top() - 1;

        if (row <= 0)
            val = values->firstInColumn(col, &row);
        else
            val = values->nextInColumn(col, row, &row);

        while (row && (row <= range.bottom())) {
            // Suitable value? We have found at least one, and thus we're good to show the popup.
            if ((row != cursorCell.row()) && (!val.asString().isEmpty())) {
                CellBase c(sheet, col, row);
                QString text = c.userInput();
                if (text != cursorCell.userInput()) {
                    if (res.indexOf(text) == -1) {
                        res.append(text);
                        if (limit && (res.size() >= limit))
                            return res;
                    }
                }
            }

            // Value not suitable, let's continue.
            val = values->nextInColumn(col, row, &row);
        }
    }
    return res;
}

bool ListChoose::enabledForSelection(Selection *selection, const Cell &cursorCell)
{
    // Check if the list would contain at least 1 item. If so, we're enabled.
    QStringList lst = items(selection, cursorCell, 1);
    if (lst.length())
        return true;
    return false;
}

void ListChoose::itemSelected(QAction *action)
{
    const Cell cell(m_selection->activeSheet(), m_selection->cursor());

    DataManipulator *command = new DataManipulator;
    command->setSheet(m_selection->activeSheet());
    command->setValue(Value(action->text()));
    command->setParsing(true);
    command->add(cell.cellPosition());
    command->execute(m_selection->canvas());
}
