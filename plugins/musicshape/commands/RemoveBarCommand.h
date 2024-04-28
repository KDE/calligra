/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef REMOVEBARCOMMAND_H
#define REMOVEBARCOMMAND_H

#include <kundo2command.h>

namespace MusicCore
{
class Bar;
}
class MusicShape;

class RemoveBarCommand : public KUndo2Command
{
public:
    RemoveBarCommand(MusicShape *shape, int barIdx);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::Bar *m_bar;
    int m_index;
};

#endif // REMOVEBARCOMMAND_H
