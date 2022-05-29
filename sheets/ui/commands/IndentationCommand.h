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

    void setIndent(double val);

protected:
    bool performCommands() override;
private:
    double m_indent;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_INDENTATION_COMMAND
