/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ADDNOTECOMMAND_H
#define ADDNOTECOMMAND_H

#include <kundo2command.h>

#include "../core/Global.h"

namespace MusicCore
{
class Staff;
class Note;
class Chord;
}
class MusicShape;

class AddNoteCommand : public KUndo2Command
{
public:
    AddNoteCommand(MusicShape *shape, MusicCore::Chord *chord, MusicCore::Staff *staff, MusicCore::Duration duration, int pitch, int accidentals = 0);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::Chord *m_chord;
    MusicCore::Duration m_oldDuration, m_newDuration;
    int m_oldDots;
    MusicCore::Note *m_note;
};

#endif // ADDNOTECOMMAND_H
