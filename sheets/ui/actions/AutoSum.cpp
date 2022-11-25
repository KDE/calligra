/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "AutoSum.h"
#include "Actions.h"

#include <KLocalizedString>

#include "engine/CellBase.h"
#include "engine/Value.h"
#include "core/Sheet.h"
#include "ui/CellEditorBase.h"
#include "ui/CellToolBase.h"



using namespace Calligra::Sheets;



AutoSum::AutoSum(Actions *actions)
    : CellAction(actions, "autoSum", i18n("Autosum"), koIcon("black_sum"), i18n("Insert the 'sum' function"))
{
    m_closeEditor = true;
}

AutoSum::~AutoSum()
{
}

void AutoSum::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    QRect sel = selection->lastRange();

    // shrink to used area, reduce to 1 column/row if outside (there's nothing in those cells anyway).
    QRect used = sheet->usedArea();
    int usex = used.right();
    int usey = used.bottom();
    if (sel.top() > usey) sel.setBottom(sel.top());
    else if (sel.bottom() > usey) sel.setBottom(usey);
    if (sel.left() > usex) sel.setRight(sel.left());
    else if (sel.right() > usex) sel.setRight(usex);

    // Remove empty rows/columns
    while (sel.bottom() > sel.top()) {
        int y = sel.bottom();
        bool used = false;
        for (int x = sel.left(); x <= sel.right(); ++x) {
            Value v = CellBase(sheet, x, y).value();
            if (!v.isNull()) used = true;
        }
        if (used) break;
        sel.setBottom(y - 1);
    }
    while (sel.right() > sel.left()) {
        int x = sel.right();
        bool used = false;
        for (int y = sel.top(); y <= sel.bottom(); ++y) {
            Value v = CellBase(sheet, x, y).value();
            if (!v.isNull()) used = true;
        }
        if (used) break;
        sel.setRight(x - 1);
    }

    // If we have a single cell, we want to expand to the full column of values.
    if ((sel.height() == 1) && (sel.width() == 1)) {
        int x = sel.left();
        int y = sel.top();
        Value cur = CellBase(sheet, x, y).value();
        int yy = y;
        // We do not check the current x/y cell - this lets us place the cursor directly under a column of numbers and have it be counted.
        while (yy > 1) {
            yy--;
            Value v = CellBase(sheet, x, yy).value();
            if (v.isNumber()) continue;
            sel.setTop(yy + 1);
            break;
        }
        // If the current cell contains a number, we expand to the down, too.
        if (cur.isNumber()) {
            yy = y;
            while (yy <= usey) {  // need to go to usey+1 so that we set the bottom correctly if our edge is = usey
                yy++;
                Value v = CellBase(sheet, x, yy).value();
                if (v.isNumber()) continue;
                sel.setBottom(yy - 1);
                break;
            }
        }

        // If we fail to find the column, try the row instead.
        if (sel.height() == 1) {
            int xx = x;
            while (xx > 1) {
                xx--;
                Value v = CellBase(sheet, xx, y).value();
                if (v.isNumber()) continue;
                sel.setLeft(xx + 1);
                break;
            }
            if (cur.isNumber()) {
                xx = x;
                while (xx <= usex) {
                    xx++;
                    Value v = CellBase(sheet, xx, y).value();
                    if (v.isNumber()) continue;
                    sel.setRight(xx - 1);
                    break;
                }
            }
        }
    }

    if ((sel.height() > 1) || (sel.width() > 1)) {
        AutoSumCommand* command = new AutoSumCommand();
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

bool AutoSumCommand::process(Element* element)
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
            if (v.isNumber()) sumy++;
            if (maxy < sumy) maxy = sumy;
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
        if (v.isNumber()) sumx++;
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


