/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef REMOVECHORDCOMMAND_H
#define REMOVECHORDCOMMAND_H

#include <kundo2command.h>

namespace MusicCore
{
class Chord;
}
class MusicShape;

class RemoveChordCommand : public KUndo2Command
{
public:
    RemoveChordCommand(MusicShape *shape, MusicCore::Chord *chord);
    void redo() override;
    void undo() override;

private:
    MusicCore::Chord *m_chord;
    MusicShape *m_shape;
    int m_index;
};

#endif // REMOVECHORDCOMMAND_H
