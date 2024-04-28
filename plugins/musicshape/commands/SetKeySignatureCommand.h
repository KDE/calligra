/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SETKEYSIGNATURECOMMAND_H
#define SETKEYSIGNATURECOMMAND_H

#include <QList>
#include <QPair>
#include <kundo2command.h>

class MusicShape;
namespace MusicCore
{
class Bar;
class Staff;
class KeySignature;
}

class SetKeySignatureCommand : public KUndo2Command
{
public:
    enum RegionType { EndOfPiece, NextChange };
    SetKeySignatureCommand(MusicShape *shape, int bar, RegionType type, MusicCore::Staff *staff, int accidentals);
    SetKeySignatureCommand(MusicShape *shape, int startBar, int endBar, MusicCore::Staff *staff, int accidentals);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::Staff *m_staff;
    QList<QPair<MusicCore::Bar *, MusicCore::KeySignature *>> m_newKeySignatures, m_oldKeySignatures;
};

#endif // SETKEYSIGNATURECOMMAND_H
