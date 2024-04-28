/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "MakeRestCommand.h"
#include "../MusicShape.h"
#include "../core/Chord.h"
#include "../core/Note.h"
#include "../core/VoiceBar.h"

#include <KLocalizedString>

using namespace MusicCore;

MakeRestCommand::MakeRestCommand(MusicShape *shape, Chord *chord)
    : m_chord(chord)
    , m_shape(shape)
{
    setText(kundo2_i18n("Convert chord to rest"));
    for (int i = 0; i < chord->noteCount(); i++) {
        m_notes.append(chord->note(i));
    }
}

void MakeRestCommand::redo()
{
    foreach (Note *n, m_notes) {
        m_chord->removeNote(n, false);
    }
    m_chord->voiceBar()->updateAccidentals();
    m_shape->engrave();
    m_shape->update();
}

void MakeRestCommand::undo()
{
    foreach (Note *n, m_notes) {
        m_chord->addNote(n);
    }
    m_chord->voiceBar()->updateAccidentals();
    m_shape->engrave();
    m_shape->update();
}
