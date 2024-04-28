/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KEYSIGNATUREACTION_H
#define KEYSIGNATUREACTION_H

#include "AbstractMusicAction.h"

class KeySignatureAction : public AbstractMusicAction
{
public:
    KeySignatureAction(SimpleEntryTool *tool, int accidentals);
    explicit KeySignatureAction(SimpleEntryTool *tool);

    void mousePress(MusicCore::Staff *staff, int bar, const QPointF &pos) override;

private:
    int m_accidentals;
    bool m_showDialog;
};

#endif // KEYSIGNATUREACTION_H
