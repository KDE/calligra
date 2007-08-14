/* This file is part of the KDE project
 * Copyright 2007 Marijn Kruisselbrink <m.Kruisselbrink@student.tue.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef SETKEYSIGNATURECOMMAND_H
#define SETKEYSIGNATURECOMMAND_H

#include <QUndoCommand>
#include <QList>

class MusicShape;
namespace MusicCore {
    class Bar;
    class Staff;
    class KeySignature;
}

class SetKeySignatureCommand : public QUndoCommand
{
public:
    SetKeySignatureCommand(MusicShape* shape, MusicCore::Bar* bar, MusicCore::Staff* staff, int accidentals);
    virtual void redo();
    virtual void undo();
private:
    MusicShape* m_shape;
    MusicCore::Bar* m_bar;
    QList<MusicCore::KeySignature*> m_newKeySignatures, m_oldKeySignatures;
};

#endif // SETKEYSIGNATURECOMMAND_H
