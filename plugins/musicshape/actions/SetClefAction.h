/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SETCLEFACTION_H
#define SETCLEFACTION_H

#include "../core/Clef.h"
#include "AbstractMusicAction.h"

class SetClefAction : public AbstractMusicAction
{
public:
    SetClefAction(MusicCore::Clef::ClefShape shape, int line, int octaveChange, SimpleEntryTool *tool);
    void mousePress(MusicCore::Staff *staff, int bar, const QPointF &pos) override;

private:
    MusicCore::Clef::ClefShape m_shape;
    int m_line;
    int m_octaveChange;
};

#endif // SETCLEFACTION_H
