/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SETTIMESIGNATURECOMMAND_H
#define SETTIMESIGNATURECOMMAND_H

#include <QList>
#include <kundo2command.h>

#include "../core/TimeSignature.h"

class MusicShape;
namespace MusicCore
{
class Bar;
class TimeSignature;
}

class SetTimeSignatureCommand : public KUndo2Command
{
public:
    SetTimeSignatureCommand(MusicShape *shape, MusicCore::Bar *bar, int beats, int beat);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::Bar *m_bar;
    QList<MusicCore::TimeSignature *> m_oldSigs, m_newSigs;
};

#endif // SETTIMESIGNATURECOMMAND_H
