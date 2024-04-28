/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef REMOVENOTECOMMAND_H
#define REMOVENOTECOMMAND_H

#include <kundo2command.h>

namespace MusicCore
{
class Chord;
class Note;
}
class MusicShape;

class RemoveNoteCommand : public KUndo2Command
{
public:
    RemoveNoteCommand(MusicShape *shape, MusicCore::Chord *chord, MusicCore::Note *note);
    void redo() override;
    void undo() override;

private:
    MusicCore::Chord *m_chord;
    MusicCore::Note *m_note;
    MusicShape *m_shape;
};

#endif // REMOVENOTECOMMAND_H
