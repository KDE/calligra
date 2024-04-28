/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GoalSeek.h"
#include "Actions.h"
#include "ui/actions/dialogs/GoalSeekDialog.h"

#include <KLocalizedString>
#include <KMessageBox>

#include "core/Map.h"
#include "core/Sheet.h"
#include "engine/CalculationSettings.h"
#include "engine/CellBase.h"
#include "engine/Formula.h"
#include "engine/Localization.h"
#include "ui/Selection.h"
#include "ui/commands/DataManipulators.h"

using namespace Calligra::Sheets;

GoalSeek::GoalSeek(Actions *actions)
    : CellAction(actions, "goalSeek", i18n("&Goal Seek..."), QIcon(), i18n("Repeating calculation to find a specific value"))
    , m_selection(nullptr)
    , m_dlg(nullptr)
    , m_canvasWidget(nullptr)
{
    m_closeEditor = true;
}

GoalSeek::~GoalSeek()
{
    if (m_dlg)
        delete m_dlg;
}

void GoalSeek::execute(Selection *selection, Sheet *, QWidget *canvasWidget)
{
    m_selection = selection;
    m_canvasWidget = canvasWidget;

    if (!m_dlg) {
        m_dlg = new GoalSeekDialog(canvasWidget, selection);
        connect(m_dlg, &GoalSeekDialog::calculate, this, &GoalSeek::calculate);
    }
    m_dlg->show(); // dialog deletes itself later
}

void GoalSeek::calculate()
{
    Sheet *sheet = m_selection->activeSheet();

    const Region source = sheet->map()->regionFromName(m_dlg->selectorValue(3), sheet);
    if (!source.isValid() || !source.isSingular()) {
        KMessageBox::error(m_canvasWidget, i18n("Cell reference is invalid."));
        m_dlg->focusSelector(3);
        return;
    }

    const Region target = sheet->map()->regionFromName(m_dlg->selectorValue(1), sheet);
    if (!target.isValid() || !target.isSingular()) {
        KMessageBox::error(m_canvasWidget, i18n("Cell reference is invalid."));
        m_dlg->focusSelector(1);
        return;
    }

    bool ok = false;
    Localization *locale = sheet->map()->calculationSettings()->locale();
    double goal = locale->readNumber(m_dlg->selectorValue(2), &ok);
    if (!ok) {
        KMessageBox::error(m_canvasWidget, i18n("Target value is invalid."));
        m_dlg->focusSelector(2);
        return;
    }

    CellBase sourceCell = CellBase(source.firstSheet(), source.firstRange().topLeft());
    CellBase targetCell = CellBase(target.firstSheet(), target.firstRange().topLeft());

    if (!sourceCell.value().isNumber()) {
        KMessageBox::error(m_canvasWidget, i18n("Source cell must contain a numeric value."));
        m_dlg->focusSelector(3);
        return;
    }

    if (!targetCell.isFormula()) {
        KMessageBox::error(m_canvasWidget, i18n("Target cell must contain a formula."));
        m_dlg->focusSelector(1);
        return;
    }

    m_dlg->setNotice(i18n("Solving ..."));

    // TODO: make these configurable
    double eps = 0.0000001;
    int maxIter = 1000;

    double resultA, resultB;

    // save old value
    double oldSource = numToDouble(sourceCell.value().asFloat());
    resultA = numToDouble(targetCell.value().asFloat()) - goal;

    // initialize start value
    double startA = oldSource;
    double startB = startA;
    double x = startB + 0.5;

    int iterations = maxIter;
    const Formula formula = targetCell.formula();
    ok = true;

    // while the result is not close enough to zero
    // or while the max number of iterations is not reached...
    while (fabs(resultA) > eps && (iterations >= 0)) {
        startA = startB;
        startB = x;

        sourceCell.setValue(Value(startA));
        const double targetValueA = numToDouble(formula.eval().asFloat());
        resultA = targetValueA - goal;
        //         debugSheets << "Target A:" << targetValueA << "," << targetCell.userInput() << "Calc:" << resultA;

        sourceCell.setValue(Value(startB));
        const double targetValueB = numToDouble(formula.eval().asFloat());
        resultB = targetValueB - goal;
        //         debugSheets << "Target B:" << targetValueB << "," << targetCell.userInput() << "Calc:" << resultB;

        //         debugSheets <<"Iteration:" << iterations <<", StartA:" << startA
        //                  << ", ResultA: " << resultA << " (eps: " << eps << "), StartB: "
        //                  << startB << ", ResultB: " << resultB << Qt::endl;

        // find zero with secant method (rough implementation was provided by Franz-Xaver Meier):
        // if the function returns the same for two different
        // values we have something like a horizontal line
        // => can't get zero.
        if (resultB == resultA) {
            //         debugSheets <<" resultA == resultB";
            if (fabs(resultA) < eps) {
                ok = true;
                break;
            }

            ok = false;
            break;
        }

        // Point of intersection of secant with x-axis
        x = (startA * resultB - startB * resultA) / (resultB - resultA);

        if (fabs(x) > 100000000) {
            //             debugSheets <<"fabs(x) > 100000000:" << x;
            ok = false;
            break;
        }

        //         debugSheets <<"X:" << x <<", fabs (resultA):" << fabs(resultA) <<", Real start:" << startA <<", Real result:" << resultA <<", Iteration:" <<
        //         iterations;

        --iterations;
    }

    // Put the original value back, so that undo works correctly.
    sourceCell.setValue(Value(oldSource));
    if (ok) {
        m_dlg->setNotice(i18n("Goal seeking with cell %1 found a solution.", m_dlg->selectorValue(3)));

        // Reset the value for a proper undo value.
        Sheet *const sheet = dynamic_cast<Sheet *>(sourceCell.sheet());
        DataManipulator *const command = new DataManipulator();
        command->setSheet(sheet);
        command->add(Region(sourceCell.cellPosition(), sheet));
        command->setValue(Value(startA));
        command->execute(m_selection->canvas());
    } else {
        m_dlg->setNotice(i18n("Goal seeking with cell %1 has found NO solution.", m_dlg->selectorValue(3)));
    }
}
