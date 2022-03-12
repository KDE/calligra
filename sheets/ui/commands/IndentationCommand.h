/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_INDENTATION_COMMAND
#define CALLIGRA_SHEETS_INDENTATION_COMMAND


#include "AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Commands
 * \brief Changes the indentation level.
 */
class IndentationCommand : public AbstractRegionCommand
{
public:
    IndentationCommand();

    void setReverse(bool reverse) override;

protected:
    bool mainProcessing() override;
    bool postProcessing() override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_INDENTATION_COMMAND
