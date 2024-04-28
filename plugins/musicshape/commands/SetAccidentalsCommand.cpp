/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SetAccidentalsCommand.h"

#include "../MusicShape.h"

#include "../core/Chord.h"
#include "../core/Note.h"
#include "../core/VoiceBar.h"

#include <KLocalizedString>

SetAccidentalsCommand::SetAccidentalsCommand(MusicShape *shape, MusicCore::Note *note, int accidentals)
    : m_shape(shape)
    , m_note(note)
    , m_oldAccidentals(note->accidentals())
    , m_newAccidentals(accidentals)
{
    setText(kundo2_i18n("Set accidentals"));
}

void SetAccidentalsCommand::redo()
{
    m_note->setAccidentals(m_newAccidentals);
    m_note->chord()->voiceBar()->updateAccidentals();
    m_shape->engrave();
    m_shape->update();
}

void SetAccidentalsCommand::undo()
{
    m_note->setAccidentals(m_oldAccidentals);
    m_note->chord()->voiceBar()->updateAccidentals();
    m_shape->engrave();
    m_shape->update();
}
