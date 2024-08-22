/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SelectTextStrategy.h"
#include "ArtisticTextTool.h"
#include "ArtisticTextToolSelection.h"

SelectTextStrategy::SelectTextStrategy(ArtisticTextTool *textTool, int cursor)
    : KoInteractionStrategy(textTool)
    , m_selection(nullptr)
    , m_oldCursor(cursor)
    , m_newCursor(cursor)
{
    m_selection = dynamic_cast<ArtisticTextToolSelection *>(textTool->selection());
    Q_ASSERT(m_selection);
}

SelectTextStrategy::~SelectTextStrategy() = default;

void SelectTextStrategy::handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers /*modifiers*/)
{
    ArtisticTextTool *textTool = dynamic_cast<ArtisticTextTool *>(tool());
    if (!textTool)
        return;

    m_newCursor = textTool->cursorFromMousePosition(mouseLocation);
    if (m_newCursor >= 0)
        m_selection->selectText(qMin(m_oldCursor, m_newCursor), qMax(m_oldCursor, m_newCursor));
}

KUndo2Command *SelectTextStrategy::createCommand()
{
    return nullptr;
}

void SelectTextStrategy::finishInteraction(Qt::KeyboardModifiers /*modifiers*/)
{
    ArtisticTextTool *textTool = dynamic_cast<ArtisticTextTool *>(tool());
    if (!textTool)
        return;

    if (m_newCursor >= 0)
        textTool->setTextCursor(m_selection->selectedShape(), m_newCursor);
}
