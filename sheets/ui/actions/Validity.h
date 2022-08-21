/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_VALIDITY
#define CALLIGRA_SHEETS_ACTION_VALIDITY


#include "CellAction.h"

#include "ui/commands/AbstractRegionCommand.h"
#include "engine/Validity.h"


namespace Calligra
{
namespace Sheets
{
class ValidityDialog;

class SetValidity : public CellAction {
Q_OBJECT
public:
    SetValidity(Actions *actions);
    virtual ~SetValidity();

protected Q_SLOTS:

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    ValidityDialog *m_dlg;
    Selection *m_selection;
};

class ClearValidity : public CellAction {
Q_OBJECT
public:
    ClearValidity(Actions *actions);
    virtual ~ClearValidity();

protected Q_SLOTS:

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    
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
    bool process(Element* element) override;

private:
    Validity m_validity;
};



} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_ACTION_VALIDITY
