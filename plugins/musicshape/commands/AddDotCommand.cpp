/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "AddDotCommand.h"

#include "../MusicShape.h"

#include "../core/Chord.h"

#include <KLocalizedString>

AddDotCommand::AddDotCommand(MusicShape *shape, MusicCore::Chord *chord)
    : m_shape(shape)
    , m_chord(chord)
{
    setText(kundo2_i18n("Add dot"));
}

void AddDotCommand::redo()
{
    m_chord->setDots(m_chord->dots() + 1);
    m_shape->engrave();
    m_shape->update();
}

void AddDotCommand::undo()
{
    m_chord->setDots(m_chord->dots() - 1);
    m_shape->engrave();
    m_shape->update();
}
