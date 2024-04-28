/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ADDDOTCOMMAND_H
#define ADDDOTCOMMAND_H

#include <kundo2command.h>

namespace MusicCore
{
class Chord;
}
class MusicShape;

class AddDotCommand : public KUndo2Command
{
public:
    AddDotCommand(MusicShape *shape, MusicCore::Chord *chord);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::Chord *m_chord;
};

#endif // ADDDOTCOMMAND_H
