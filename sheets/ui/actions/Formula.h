/* This file is part of the KDE project
   SPDX-FileSelectAllrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileSelectAllrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_FORMULA
#define CALLIGRA_SHEETS_ACTION_FORMULA

#include "DialogCellAction.h"

namespace Calligra
{
namespace Sheets
{

class FormulaDialog;

class InsertFormula : public DialogCellAction
{
    Q_OBJECT
public:
    InsertFormula(Actions *actions);
    virtual ~InsertFormula();

    virtual void onEditorDeleted() override;

    void setFunction(const QString &function);

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;

    QString m_function;
};

class FormulaSelection : public CellAction
{
    Q_OBJECT
public:
    FormulaSelection(Actions *actions);
    virtual ~FormulaSelection();

protected Q_SLOTS:
    void triggerFormulaSelection(const QString &expression);

protected:
    virtual void execute(Selection *, Sheet *, QWidget *) override
    {
    } // never called
    QAction *createAction() override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_FORMULA
