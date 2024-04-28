/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SELECTTEXTSTRATEGY_H
#define SELECTTEXTSTRATEGY_H

#include <KoInteractionStrategy.h>

class ArtisticTextTool;
class ArtisticTextToolSelection;

/// A strategy to select text on a artistic text shape
class SelectTextStrategy : public KoInteractionStrategy
{
public:
    SelectTextStrategy(ArtisticTextTool *textTool, int cursor);
    ~SelectTextStrategy() override;

    // reimplemnted from KoInteractionStrategy
    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    // reimplemnted from KoInteractionStrategy
    KUndo2Command *createCommand() override;
    // reimplemnted from KoInteractionStrategy
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;

private:
    ArtisticTextToolSelection *m_selection;
    int m_oldCursor;
    int m_newCursor;
};

#endif // SELECTTEXTSTRATEGY_H
