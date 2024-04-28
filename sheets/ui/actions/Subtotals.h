/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_SUBTOTALS
#define CALLIGRA_SHEETS_ACTION_SUBTOTALS

#include "CellAction.h"

#include "ui/commands/DataManipulators.h"

namespace Calligra
{
namespace Sheets
{
class SubtotalDialog;

class Subtotals : public CellAction
{
    Q_OBJECT
public:
    Subtotals(Actions *actions);
    virtual ~Subtotals();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual bool enabledForSelection(Selection *selection, const Cell &) override;

    SubtotalDialog *m_dlg;
    Selection *m_selection;
};

class SubtotalsManipulator : public AbstractDFManipulator
{
public:
    SubtotalsManipulator();
    ~SubtotalsManipulator() override;

    void setIgnoreEmptyColumns(bool ignore);
    void setSummaryBelow(bool summary);
    void setSummaryOnly(bool summary);

    void setFuncCode(int code);
    void setRange(const Region &range);
    void setPrimaryColumn(int col);
    void setColumns(QList<int> columns);

protected:
    bool preProcess() override;
    bool postProcess() override;
    Value newValue(Element *element, int col, int row, bool *parse, Format::Type *fmtType) override;
    Style newFormat(Element *element, int col, int row) override;

private:
    class Private;
    Private *d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_SUBTOTALS
