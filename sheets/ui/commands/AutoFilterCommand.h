/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_AUTOFILTER_COMMAND
#define CALLIGRA_SHEETS_AUTOFILTER_COMMAND

#include "AbstractRegionCommand.h"

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

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_AUTOFILTER_COMMAND
