/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_AUTOFILTER
#define CALLIGRA_SHEETS_ACTION_AUTOFILTER

#include "CellAction.h"

#include "ui/commands/AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Adds an auto-filter to a cell range.
 */
class AutoFilterCommand : public AbstractRegionCommand
{
public:
    /**
     * Constructor.
     */
    AutoFilterCommand();

    /**
     * Destructor.
     */
    ~AutoFilterCommand() override;

    bool performCommands() override;
};

class AutoFilter : public CellAction
{
    Q_OBJECT
public:
    AutoFilter(Actions *actions);
    virtual ~AutoFilter();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_AUTOFILTER
