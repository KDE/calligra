/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MAKERESTCOMMAND_H
#define MAKERESTCOMMAND_H

#include <QList>
#include <kundo2command.h>

namespace MusicCore
{
class Chord;
class Note;
}
class MusicShape;

class MakeRestCommand : public KUndo2Command
{
public:
    MakeRestCommand(MusicShape *shape, MusicCore::Chord *chord);
    void redo() override;
    void undo() override;

private:
    MusicCore::Chord *m_chord;
    QList<MusicCore::Note *> m_notes;
    MusicShape *m_shape;
};

#endif // MAKERESTCOMMAND_H
