/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef CREATECHORDCOMMAND_H
#define CREATECHORDCOMMAND_H

#include <kundo2command.h>

#include "../core/Global.h"

namespace MusicCore
{
class Staff;
class VoiceBar;
class Chord;
}
class MusicShape;

class CreateChordCommand : public KUndo2Command
{
public:
    CreateChordCommand(MusicShape *shape,
                       MusicCore::VoiceBar *voiceBar,
                       MusicCore::Staff *staff,
                       MusicCore::Duration duration,
                       int before,
                       int pitch,
                       int accidentals);
    CreateChordCommand(MusicShape *shape, MusicCore::VoiceBar *voiceBar, MusicCore::Staff *staff, MusicCore::Duration duration, int before);
    void redo() override;
    void undo() override;

private:
    MusicShape *m_shape;
    MusicCore::VoiceBar *m_voiceBar;
    int m_before;
    MusicCore::Chord *m_chord;
};

#endif // CREATECHORDCOMMAND_H
