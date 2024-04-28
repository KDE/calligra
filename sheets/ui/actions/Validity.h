/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_VALIDITY
#define CALLIGRA_SHEETS_ACTION_VALIDITY

#include "DialogCellAction.h"

#include "engine/Validity.h"
#include "ui/commands/AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{
class ValidityDialog;

class SetValidity : public DialogCellAction
{
    Q_OBJECT
public:
    SetValidity(Actions *actions);
    virtual ~SetValidity();

protected Q_SLOTS:
    void applyValidity(const Validity &validity);

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
    virtual void onSelectionChanged() override;
};

class ClearValidity : public CellAction
{
    Q_OBJECT
public:
    ClearValidity(Actions *actions);
    virtual ~ClearValidity();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    virtual QAction *createAction() override;

    virtual bool enabledForSelection(Selection *selection, const Cell &activeCell) override;
};

/**
 * \class ValidityCommand
 * \ingroup Commands
 * \brief Adds/Removes validity checks to/of a cell region.
 */
class ValidityCommand : public AbstractRegionCommand
{
public:
    ValidityCommand();
    void setValidity(Validity validity);

protected:
    bool process(Element *element) override;

private:
    Validity m_validity;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_VALIDITY
