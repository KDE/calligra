/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2004 Laurent Montel <montel@kde.org>

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

// Local
#include "PageLayoutCommand.h"

#include "Sheet.h"

using namespace KSpread;

// ----- PageLayoutCommand -----

PageLayoutCommand::PageLayoutCommand(Sheet* sheet, const PrintSettings& settings, QUndoCommand* parent)
        : QUndoCommand(parent)
        , m_sheet(sheet)
        , m_settings(settings)
{
    setText(i18n("Set Page Layout"));
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
