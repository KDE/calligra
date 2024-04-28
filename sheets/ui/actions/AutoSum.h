/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_AUTOSUM
#define CALLIGRA_SHEETS_ACTION_AUTOSUM

#include "CellAction.h"

#include "ui/commands/AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

class AutoSum : public CellAction
{
    Q_OBJECT
public:
    AutoSum(Actions *actions);
    virtual ~AutoSum();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

/**
 * \ingroup Commands
 * \brief Automatically sums a chosen or auto-detected range.
 */
class AutoSumCommand : public AbstractRegionCommand
{
public:
    AutoSumCommand();
    void setSelection(Selection *selection)
    {
        m_selection = selection;
    }

protected:
    bool process(Element *element) override;
    Selection *m_selection;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_AUTOSUM
