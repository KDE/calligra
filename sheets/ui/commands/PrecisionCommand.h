/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PRECISION_COMMAND
#define CALLIGRA_SHEETS_PRECISION_COMMAND

#include "AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Sets the decimal precision level.
 */
class PrecisionCommand : public AbstractRegionCommand
{
public:
    PrecisionCommand();

    void setDecrease(bool decrease);

protected:
    bool performCommands() override;
    bool m_decrease  : 1;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PRECISION_COMMAND
