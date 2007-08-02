/* This file is part of the KDE project
 * Copyright 2007 Marijn Kruisselbrink <m.Kruisselbrink@student.tue.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "AddNoteCommand.h"

#include "../core/Note.h"
#include "../MusicShape.h"

#include <klocale.h>

using namespace MusicCore;

AddNoteCommand::AddNoteCommand(MusicShape* shape, Chord* chord, Staff* staff, Chord::Duration duration, int pitch)
    : m_shape(shape), m_chord(chord), m_oldDuration(chord->duration()), m_newDuration(duration)
{
    setText(i18n("Add note"));
    m_note = new Note(staff, pitch);
}

void AddNoteCommand::redo()
{
    m_chord->setDuration(m_newDuration);
    m_chord->addNote(m_note);
    m_shape->engrave();
    m_shape->repaint();

}

void AddNoteCommand::undo()
{
    m_chord->setDuration(m_oldDuration);
    m_chord->removeNote(m_note, false);
    m_shape->engrave();
    m_shape->repaint();
}
