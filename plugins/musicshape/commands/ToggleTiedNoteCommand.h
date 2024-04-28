/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TOGGLETIEDNOTECOMMAND_H
#define TOGGLETIEDNOTECOMMAND_H

#include <kundo2command.h>

namespace MusicCore
{
class Note;
}
class MusicShape;

class ToggleTiedNoteCommand : public KUndo2Command
{
public:
    ToggleTiedNoteCommand(MusicShape *shape, MusicCore::Note *note);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::Note *m_note;
};

#endif // TOGGLETIEDNOTECOMMAND_H
