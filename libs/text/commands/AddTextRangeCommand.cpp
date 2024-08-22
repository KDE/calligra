// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Ganesh Paramasivam <ganesh@crystalfab.com>
// SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
// SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "AddTextRangeCommand.h"

#include "TextDebug.h"
#include <KLocalizedString>

#include <KoTextRange.h>
#include <KoTextRangeManager.h>

AddTextRangeCommand::AddTextRangeCommand(KoTextRange *range, KUndo2Command *parent)
    : KUndo2Command(kundo2_noi18n("internal step"), parent)
    , m_range(range)
{
}

void AddTextRangeCommand::undo()
{
    KUndo2Command::undo();
    m_range->manager()->remove(m_range);
}

void AddTextRangeCommand::redo()
{
    KUndo2Command::redo();
    m_range->manager()->insert(m_range);
}

AddTextRangeCommand::~AddTextRangeCommand() = default;
