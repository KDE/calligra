/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SETACCIDENTALSCOMMAND_H
#define SETACCIDENTALSCOMMAND_H

#include <kundo2command.h>

namespace MusicCore
{
class Note;
}
class MusicShape;

class SetAccidentalsCommand : public KUndo2Command
{
public:
    SetAccidentalsCommand(MusicShape *shape, MusicCore::Note *note, int accidentals);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::Note *m_note;
    int m_oldAccidentals, m_newAccidentals;
};

#endif // SETACCIDENTALSCOMMAND_H
