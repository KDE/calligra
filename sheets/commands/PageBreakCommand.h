/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
    explicit PageBreakCommand(KUndo2Command *parent = 0);
    ~PageBreakCommand() override;

    enum Mode { BreakBeforeColumn, BreakBeforeRow };
    void setMode(Mode mode);

protected:
    bool process(Element *) override;
    bool postProcessing() override;

private:
    Mode m_mode;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PAGE_BREAK_COMMAND
