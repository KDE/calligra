/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SELECTIONACTION_H
#define SELECTIONACTION_H

#include "AbstractMusicAction.h"

class SelectionAction : public AbstractMusicAction
{
public:
    explicit SelectionAction(SimpleEntryTool *tool);

    void mousePress(MusicCore::Staff *staff, int bar, const QPointF &pos) override;
    void mouseMove(MusicCore::Staff *staff, int bar, const QPointF &pos) override;

private:
    int m_firstBar;
    MusicCore::Staff *m_startStaff;
};

#endif // SELECTIONACTION_H
