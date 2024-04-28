/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "ToggleTiedNoteCommand.h"

#include "../MusicShape.h"

#include "../core/Chord.h"
#include "../core/Note.h"
#include "../core/VoiceBar.h"

#include <KLocalizedString>

ToggleTiedNoteCommand::ToggleTiedNoteCommand(MusicShape *shape, MusicCore::Note *note)
    : m_shape(shape)
    , m_note(note)
{
    setText(kundo2_i18n("Toggle Note Tie"));
}

void ToggleTiedNoteCommand::redo()
{
    m_note->setStartTie(!m_note->isStartTie());
    m_note->chord()->voiceBar()->updateAccidentals();
    m_shape->update();
}

void ToggleTiedNoteCommand::undo()
{
    m_note->setStartTie(!m_note->isStartTie());
    m_note->chord()->voiceBar()->updateAccidentals();
    m_shape->update();
}
