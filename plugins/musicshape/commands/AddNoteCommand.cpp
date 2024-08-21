/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "AddNoteCommand.h"

#include "../MusicShape.h"
#include "../core/Chord.h"
#include "../core/Note.h"
#include "../core/VoiceBar.h"

#include <KLocalizedString>

using namespace MusicCore;

AddNoteCommand::AddNoteCommand(MusicShape *shape, Chord *chord, Staff *staff, Duration duration, int pitch, int accidentals)
    : m_shape(shape)
    , m_chord(chord)
    , m_oldDuration(chord->duration())
    , m_newDuration(duration)
    , m_oldDots(chord->dots())
    , m_note(nullptr)
{
    bool exists = false;
    for (int i = 0; i < m_chord->noteCount(); i++) {
        Note *n = m_chord->note(i);
        if (n->staff() == staff && n->pitch() == pitch) {
            exists = true;
            break;
        }
    }
    if (exists) {
        setText(kundo2_i18n("Set chord duration"));
    } else {
        setText(kundo2_i18n("Add note"));
        m_note = new Note(m_chord, staff, pitch, accidentals);
    }
}

void AddNoteCommand::redo()
{
    m_chord->setDuration(m_newDuration);
    m_chord->setDots(0);
    if (m_note) {
        m_chord->addNote(m_note);
    }
    m_chord->voiceBar()->updateAccidentals();
    m_shape->engrave();
    m_shape->update();
}

void AddNoteCommand::undo()
{
    m_chord->setDuration(m_oldDuration);
    m_chord->setDots(m_oldDots);
    if (m_note) {
        m_chord->removeNote(m_note, false);
    }
    m_chord->voiceBar()->updateAccidentals();
    m_shape->engrave();
    m_shape->update();
}
