/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef REMOVEPARTCOMMAND_H
#define REMOVEPARTCOMMAND_H

#include <kundo2command.h>

namespace MusicCore
{
class Sheet;
class Part;
}
class MusicShape;

class RemovePartCommand : public KUndo2Command
{
public:
    RemovePartCommand(MusicShape *shape, MusicCore::Part *part);
    void redo() override;
    void undo() override;

private:
    MusicCore::Sheet *m_sheet;
    MusicCore::Part *m_part;
    MusicShape *m_shape;
    int m_partIndex;
};

#endif // REMOVEPARTCOMMAND_H
