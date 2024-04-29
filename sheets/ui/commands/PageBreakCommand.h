/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PAGE_BREAK_COMMAND
#define CALLIGRA_SHEETS_PAGE_BREAK_COMMAND

#include "AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Sets/Unsets manual page breaks.
 */
class PageBreakCommand : public AbstractRegionCommand
{
public:
    explicit PageBreakCommand(KUndo2Command *parent = nullptr);
    ~PageBreakCommand() override;

    enum Mode { BreakBeforeColumn, BreakBeforeRow };
    void setMode(Mode mode);

    void setBreak(bool breaking)
    {
        m_breaking = breaking;
    }

protected:
    bool performNonCommandActions() override;
    bool undoNonCommandActions() override;
    bool setRangeBreak(Sheet *sheet, const QRect range, bool enable);

private:
    Mode m_mode;
    bool m_breaking : 1;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PAGE_BREAK_COMMAND
