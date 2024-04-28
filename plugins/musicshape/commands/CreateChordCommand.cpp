/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "CreateChordCommand.h"

#include "../MusicShape.h"
#include "../core/Chord.h"
#include "../core/VoiceBar.h"

#include <KLocalizedString>

using namespace MusicCore;

CreateChordCommand::CreateChordCommand(MusicShape *shape, VoiceBar *voiceBar, Staff *staff, Duration duration, int before, int pitch, int accidentals)
    : m_shape(shape)
    , m_voiceBar(voiceBar)
    , m_before(before)
{
    setText(kundo2_i18n("Add chord"));
    m_chord = new Chord(staff, duration);
    m_chord->addNote(staff, pitch, accidentals);
}

CreateChordCommand::CreateChordCommand(MusicShape *shape, VoiceBar *voiceBar, Staff *staff, Duration duration, int before)
    : m_shape(shape)
    , m_voiceBar(voiceBar)
    , m_before(before)
{
    setText(kundo2_i18n("Add rest"));
    m_chord = new Chord(staff, duration);
}

void CreateChordCommand::redo()
{
    m_voiceBar->insertElement(m_chord, m_before);
    m_shape->engrave();
    m_shape->update();
}

void CreateChordCommand::undo()
{
    m_voiceBar->removeElement(m_chord, false);
    m_shape->engrave();
    m_shape->update();
}
