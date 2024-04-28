/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef REMOVEBARACTION_H
#define REMOVEBARACTION_H

#include "AbstractMusicAction.h"

class RemoveBarAction : public AbstractMusicAction
{
public:
    explicit RemoveBarAction(SimpleEntryTool *tool);

    void mousePress(MusicCore::Staff *staff, int bar, const QPointF &pos) override;
};

#endif // REMOVEBARACTION_H
