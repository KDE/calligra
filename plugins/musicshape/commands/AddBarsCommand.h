/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ADDBARSCOMMAND_H
#define ADDBARSCOMMAND_H

#include <kundo2command.h>

namespace MusicCore
{
class Sheet;
}
class MusicShape;

class AddBarsCommand : public KUndo2Command
{
public:
    AddBarsCommand(MusicShape *shape, int bars);
    void redo() override;
    void undo() override;

private:
    MusicCore::Sheet *m_sheet;
    int m_bars;
    MusicShape *m_shape;
};

#endif // ADDBARSCOMMAND_H
