/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "RemoveNoteCommand.h"
#include "../MusicShape.h"
#include "../core/Chord.h"
#include "../core/Note.h"
#include "../core/VoiceBar.h"

#include <KLocalizedString>

using namespace MusicCore;

RemoveNoteCommand::RemoveNoteCommand(MusicShape *shape, Chord *chord, Note *note)
    : m_chord(chord)
    , m_note(note)
    , m_shape(shape)
{
    setText(kundo2_i18n("Remove note"));
}

void RemoveNoteCommand::redo()
{
    m_chord->removeNote(m_note, false);
    m_chord->voiceBar()->updateAccidentals();
    m_shape->engrave();
    m_shape->update();
}

void RemoveNoteCommand::undo()
{
    m_chord->addNote(m_note);
    m_chord->voiceBar()->updateAccidentals();
    m_shape->engrave();
    m_shape->update();
}
