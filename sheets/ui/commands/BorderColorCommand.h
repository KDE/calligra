/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_BORDER_COLOR_COMMAND
#define CALLIGRA_SHEETS_BORDER_COLOR_COMMAND

#include "AbstractRegionCommand.h"

#include "core/Style.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Applies a color to an existing border.
 */
class BorderColorCommand : public AbstractRegionCommand
{
public:
    BorderColorCommand();
    void setColor(const QColor& color) {
        m_color = color;
    }

protected:
    bool performCommands() override;

private:
    QColor m_color;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_BORDER_COLOR_COMMAND
