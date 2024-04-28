/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "RemoveChordCommand.h"
#include "../MusicShape.h"
#include "../core/Chord.h"
#include "../core/VoiceBar.h"

#include <KLocalizedString>

using namespace MusicCore;

RemoveChordCommand::RemoveChordCommand(MusicShape *shape, Chord *chord)
    : m_chord(chord)
    , m_shape(shape)
    , m_index(m_chord->voiceBar()->indexOfElement(m_chord))
{
    if (chord->noteCount() == 0) {
        setText(kundo2_i18n("Remove rest"));
    } else {
        setText(kundo2_i18n("Remove chord"));
    }
}

void RemoveChordCommand::redo()
{
    m_chord->voiceBar()->removeElement(m_chord, false);
    m_shape->engrave();
    m_shape->update();
}

void RemoveChordCommand::undo()
{
    m_chord->voiceBar()->insertElement(m_chord, m_index);
    m_shape->engrave();
    m_shape->update();
}
