/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "AutoSum.h"
#include "Actions.h"

#include <KLocalizedString>

#include "core/Sheet.h"
#include "engine/CellBase.h"
#include "engine/Value.h"
#include "ui/CellEditorBase.h"
#include "ui/CellToolBase.h"

using namespace Calligra::Sheets;

AutoSum::AutoSum(Actions *actions)
    : CellAction(actions, "autoSum", i18n("Autosum"), koIcon("black_sum"), i18n("Insert the 'sum' function"))
{
    m_closeEditor = true;
}

AutoSum::~AutoSum() = default;

void AutoSum::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    QRect sel = selection->lastRange();

    // shrink to used area, reduce to 1 column/row if outside (there's nothing in those cells anyway).
    sel = shrinkToUsedArea(sel, sheet);

    CellBase cur = CellBase(sheet, sel.left(), sel.top());
    // If we have a single cell, we want to expand to the full column of values.
    if ((sel.height() == 1) && (sel.width() == 1))
        sel = extendSelectionToRow(cur, true);
    if ((sel.height() == 1) && (sel.width() == 1))
        sel = extendSelectionToColumn(cur, true);

    if ((sel.height() > 1) || (sel.width() > 1)) {
        AutoSumCommand *command = new AutoSumCommand();
        command->setSheet(sheet);
        command->add(sel, sheet);
        command->setSelection(selection);
        command->execute(selection->canvas());
        return;
    }

    // We have nothing.
    CellToolBase *tool = m_actions->tool();
    tool->createEditor(true, true, true);
    tool->editor()->setText("=SUM()");
    tool->editor()->setCursorPosition(5);
}

AutoSumCommand::AutoSumCommand()
    : m_selection(nullptr)
{
    setText(kundo2_i18n("Auto Sum"));
}

bool AutoSumCommand::process(Element *element)
{
    QRect sel = element->rect();
    SheetBase *sheet = element->sheet();
    Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);
    // Now place the sums.
    if (sel.height() > 1) {
        int y1 = sel.top();
        int y2 = sel.bottom();
        int maxy = y2;
        for (int x = sel.left(); x <= sel.right(); ++x) {
            int sumy = y2;
            // We can place the sum at the bottom-most selected cell, or one below if said cell still contains a value.
            Value v = CellBase(sheet, x, sumy).value();
            if (v.isNumber())
                sumy++;
            if (maxy < sumy)
                maxy = sumy;
            CellBase sumCell = CellBase(sheet, x, sumy);

            const Region region(QRect(QPoint(x, y1), QPoint(x, sumy - 1)), sheet);
            const QString str = region.name(sheet);
            sumCell.setUserInput("=SUM(" + str + ")");
        }
        QRect range = QRect(QPoint(sel.left(), y1), QPoint(sel.right(), maxy));
        m_selection->initialize(range, fullSheet);
        m_selection->emitModified();
        return true;
    }
    if (sel.width() > 1) {
        // we know that the height is 1
        int x1 = sel.left();
        int x2 = sel.right();
        int y = sel.top();
        int sumx = x2;
        Value v = CellBase(sheet, sumx, y).value();
        if (v.isNumber())
            sumx++;
        CellBase sumCell = CellBase(sheet, sumx, y);

        const Region region(QRect(QPoint(x1, y), QPoint(sumx - 1, y)), sheet);
        const QString str = region.name(sheet);
        sumCell.setUserInput("=SUM(" + str + ")");
        QRect range = QRect(QPoint(x1, y), QPoint(sumx, y));
        m_selection->initialize(range, fullSheet);
        m_selection->emitModified();
        return true;
    }
    return false;
}
