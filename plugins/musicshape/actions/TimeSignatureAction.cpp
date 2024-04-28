/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TimeSignatureAction.h"

#include "../core/Bar.h"
#include "../core/Part.h"
#include "../core/Sheet.h"
#include "../core/Staff.h"
#include "../core/TimeSignature.h"

#include "../commands/SetTimeSignatureCommand.h"

#include "../MusicShape.h"
#include "../SimpleEntryTool.h"

using namespace MusicCore;

static QString getText(int beats, int beat)
{
    return QString("%1/%2").arg(beats).arg(beat);
}

TimeSignatureAction::TimeSignatureAction(SimpleEntryTool *tool, int beats, int beat)
    : AbstractMusicAction(getText(beats, beat), tool)
    , m_beats(beats)
    , m_beat(beat)
{
    setCheckable(false);
}

void TimeSignatureAction::mousePress(Staff *staff, int barIdx, const QPointF &pos)
{
    Q_UNUSED(pos);

    Bar *bar = staff->part()->sheet()->bar(barIdx);
    m_tool->addCommand(new SetTimeSignatureCommand(m_tool->shape(), bar, m_beats, m_beat));
}
