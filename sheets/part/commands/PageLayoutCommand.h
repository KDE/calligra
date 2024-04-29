/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PAGE_LAYOUT_COMMAND
#define CALLIGRA_SHEETS_PAGE_LAYOUT_COMMAND

#include "core/PrintSettings.h"

#include <kundo2command.h>

namespace Calligra
{
namespace Sheets
{
class Sheet;

/**
 * \ingroup Commands
 * Alters the print settings.
 */
class PageLayoutCommand : public KUndo2Command
{
public:
    explicit PageLayoutCommand(Sheet *sheet, const PrintSettings &settings, KUndo2Command *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    Sheet *m_sheet;
    PrintSettings m_settings;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PAGE_LAYOUT_COMMAND
