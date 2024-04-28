/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ADDPARTCOMMAND_H
#define ADDPARTCOMMAND_H

#include <kundo2command.h>

namespace MusicCore
{
class Sheet;
class Part;
}
class MusicShape;

class AddPartCommand : public KUndo2Command
{
public:
    explicit AddPartCommand(MusicShape *shape);
    void redo() override;
    void undo() override;

private:
    MusicCore::Sheet *m_sheet;
    MusicCore::Part *m_part;
    MusicShape *m_shape;
};

#endif // ADDPARTCOMMAND_H
