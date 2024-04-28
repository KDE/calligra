/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "PageLayoutCommand.h"

#include "Sheet.h"

using namespace Calligra::Sheets;

// ----- PageLayoutCommand -----

PageLayoutCommand::PageLayoutCommand(Sheet *sheet, const PrintSettings &settings, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_sheet(sheet)
    , m_settings(settings)
{
    setText(kundo2_i18n("Set Page Layout"));
}

void PageLayoutCommand::redo()
{
    // Store the old settings.
    PrintSettings settings = *m_sheet->printSettings();
    // Set the new ones.
    m_sheet->setPrintSettings(m_settings);
    // Store the old settings permanently.
    m_settings = settings;
}

void PageLayoutCommand::undo()
{
    // Exchange the settings.
    redo();
}
