/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef REMOVESTAFFELEMENTCOMMAND_H
#define REMOVESTAFFELEMENTCOMMAND_H

#include <kundo2command.h>

class MusicShape;
namespace MusicCore
{
class StaffElement;
class Bar;
}

class RemoveStaffElementCommand : public KUndo2Command
{
public:
    RemoveStaffElementCommand(MusicShape *shape, MusicCore::StaffElement *se, MusicCore::Bar *bar);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::StaffElement *m_element;
    MusicCore::Bar *m_bar;
    int m_index;
};

#endif // REMOVESTAFFELEMENTCOMMAND_H
