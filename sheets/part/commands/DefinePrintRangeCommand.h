/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DEFINE_PRINT_RANGE_COMMAND
#define CALLIGRA_SHEETS_DEFINE_PRINT_RANGE_COMMAND

#include "ui/commands/AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{
class Sheet;

/**
 * \ingroup Commands
 * Defines a print range.
 */
class DefinePrintRangeCommand : public AbstractRegionCommand
{
public:
    explicit DefinePrintRangeCommand();

    bool performNonCommandActions() override;
    bool undoNonCommandActions() override;

private:
    Region m_oldPrintRegion;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DEFINE_PRINT_RANGE_COMMAND
