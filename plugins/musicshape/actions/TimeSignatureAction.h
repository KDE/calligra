/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TIMESIGNATUREACTION_H
#define TIMESIGNATUREACTION_H

#include "AbstractMusicAction.h"

class TimeSignatureAction : public AbstractMusicAction
{
public:
    TimeSignatureAction(SimpleEntryTool *tool, int beats, int beat);

    void mousePress(MusicCore::Staff *staff, int bar, const QPointF &pos) override;

private:
    int m_beats, m_beat;
};

#endif // TIMESIGNATUREACTION_H
