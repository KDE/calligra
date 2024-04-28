/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SETCLEFCOMMAND_H
#define SETCLEFCOMMAND_H

#include <kundo2command.h>

#include "../core/Clef.h"

class MusicShape;
namespace MusicCore
{
class Bar;
class Staff;
}

class SetClefCommand : public KUndo2Command
{
public:
    SetClefCommand(MusicShape *shape, MusicCore::Bar *bar, MusicCore::Staff *staff, MusicCore::Clef::ClefShape clefShape, int line, int octaveChange);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::Bar *m_bar;
    MusicCore::Clef *m_clef, *m_oldClef;
};

#endif // SETCLEFCOMMAND_H
