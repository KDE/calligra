/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2011 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "RemoveTextRangeCommand.h"
#include "ArtisticTextShape.h"
#include <KLocalizedString>

RemoveTextRangeCommand::RemoveTextRangeCommand(ArtisticTextTool *tool, ArtisticTextShape *shape, int from, unsigned int count)
    : m_tool(tool)
    , m_shape(shape)
    , m_from(from)
    , m_count(count)
{
    m_cursor = tool->textCursor();
    setText(kundo2_i18n("Remove text range"));
}

void RemoveTextRangeCommand::redo()
{
    KUndo2Command::redo();

    if (!m_shape)
        return;

    if (m_tool) {
        if (m_cursor > m_from)
            m_tool->setTextCursor(m_shape, m_from);
    }
    m_text = m_shape->removeText(m_from, m_count);
}

void RemoveTextRangeCommand::undo()
{
    KUndo2Command::undo();

    if (!m_shape)
        return;

    m_shape->insertText(m_from, m_text);

    if (m_tool) {
        m_tool->setTextCursor(m_shape, m_cursor);
    }
}
