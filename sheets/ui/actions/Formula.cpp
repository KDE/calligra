/* This file is part of the KDE project
   SPDX-FileSelectAllrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileSelectAllrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Formula.h"
#include "Actions.h"
#include "dialogs/FormulaDialog.h"

#include "ui/CellToolBase.h"
#include "ui/Selection.h"

#include <KLocalizedString>
#include <KSelectAction>

using namespace Calligra::Sheets;

InsertFormula::InsertFormula(Actions *actions)
    : DialogCellAction(actions, "insertFormula", i18n("&Function..."), koIcon("insert-math-expression"), i18n("Insert math expression"))
{
}

InsertFormula::~InsertFormula() = default;

ActionDialog *InsertFormula::createDialog(QWidget *canvasWidget)
{
    CellToolBase *tool = m_actions->tool();
    if (!tool->createEditor())
        return nullptr; // Nothing if we don't have the editor.

    return new FormulaDialog(canvasWidget, m_selection, tool->editor());
}

void InsertFormula::onEditorDeleted()
{
    onDialogClosed();
}

void InsertFormula::setFunction(const QString &function)
{
    m_function = function;
    if (m_dlg) {
        FormulaDialog *dlg = dynamic_cast<FormulaDialog *>(m_dlg);
        dlg->setFormula(function);
    }
}

FormulaSelection::FormulaSelection(Actions *actions)
    : CellAction(actions, "formulaSelection", i18n("Formula Selection"), QIcon(), i18n("Insert a function"))
{
}

FormulaSelection::~FormulaSelection() = default;

QAction *FormulaSelection::createAction()
{
    KSelectAction *selectAction = new KSelectAction(i18n("Formula Selection"), m_actions->tool());
    selectAction->setToolTip(i18n("Insert a function"));
    QStringList functionList = {"SUM", "AVERAGE", "IF", "COUNT", "MIN", "MAX", i18n("Others...")};
    selectAction->setItems(functionList);
    selectAction->setComboWidth(80);
    selectAction->setCurrentItem(0);
    connect(selectAction, &KSelectAction::textTriggered, this, &FormulaSelection::triggerFormulaSelection);
    return selectAction;
}

void FormulaSelection::triggerFormulaSelection(const QString &expression)
{
    CellAction *a = m_actions->cellAction("insertFormula");
    if (!a)
        return;
    InsertFormula *ia = dynamic_cast<InsertFormula *>(a);

    if (expression != i18n("Others..."))
        ia->setFunction(expression);

    ia->trigger();
}
