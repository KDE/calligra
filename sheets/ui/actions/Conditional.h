/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_CONDITIONAL
#define CALLIGRA_SHEETS_ACTION_CONDITIONAL

#include "DialogCellAction.h"

#include "core/Condition.h"
#include "ui/commands/AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{
class ConditionalDialog;

class SetCondition : public DialogCellAction
{
    Q_OBJECT
public:
    SetCondition(Actions *actions);
    virtual ~SetCondition();

protected Q_SLOTS:
    void applyCondition();

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
    virtual void onSelectionChanged() override;
};

class ClearCondition : public CellAction
{
    Q_OBJECT
public:
    ClearCondition(Actions *actions);
    virtual ~ClearCondition();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual QAction *createAction() override;
};

/**
 * \class ConditionCommand
 * \ingroup Commands
 * \brief Adds/Removes conditional formatting to/of a cell region.
 */
class ConditionCommand : public AbstractRegionCommand
{
public:
    ConditionCommand();
    void setConditionList(const QList<Conditional> &list);

protected:
    bool process(Element *element) override;

private:
    Conditions m_conditions;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_CONDITIONAL
