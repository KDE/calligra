/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ReplaceTextRangeCommand.h"
#include "ArtisticTextShape.h"
#include <KLocalizedString>

ReplaceTextRangeCommand::ReplaceTextRangeCommand(ArtisticTextShape *shape,
                                                 const QString &text,
                                                 int from,
                                                 int count,
                                                 ArtisticTextTool *tool,
                                                 KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_tool(tool)
    , m_shape(shape)
    , m_from(from)
    , m_count(count)
{
    setText(kundo2_i18n("Replace text range"));
    m_newFormattedText.append(ArtisticTextRange(text, shape->fontAt(m_from)));
    m_oldFormattedText = shape->text();
}

ReplaceTextRangeCommand::ReplaceTextRangeCommand(ArtisticTextShape *shape,
                                                 const ArtisticTextRange &text,
                                                 int from,
                                                 int count,
                                                 ArtisticTextTool *tool,
                                                 KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_tool(tool)
    , m_shape(shape)
    , m_from(from)
    , m_count(count)
{
    setText(kundo2_i18n("Replace text range"));
    m_newFormattedText.append(text);
    m_oldFormattedText = shape->text();
}

ReplaceTextRangeCommand::ReplaceTextRangeCommand(ArtisticTextShape *shape,
                                                 const QList<ArtisticTextRange> &text,
                                                 int from,
                                                 int count,
                                                 ArtisticTextTool *tool,
                                                 KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_tool(tool)
    , m_shape(shape)
    , m_from(from)
    , m_count(count)
{
    setText(kundo2_i18n("Replace text range"));
    m_newFormattedText = text;
    m_oldFormattedText = shape->text();
}

void ReplaceTextRangeCommand::redo()
{
    KUndo2Command::redo();

    if (!m_shape)
        return;

    m_shape->replaceText(m_from, m_count, m_newFormattedText);

    if (m_tool) {
        int length = 0;
        foreach (const ArtisticTextRange &range, m_newFormattedText) {
            length += range.text().length();
        }
        m_tool->setTextCursor(m_shape, m_from + length);
    }
}

void ReplaceTextRangeCommand::undo()
{
    KUndo2Command::undo();

    if (!m_shape)
        return;

    m_shape->clear();
    foreach (const ArtisticTextRange &range, m_oldFormattedText) {
        m_shape->appendText(range);
    }
    if (m_tool) {
        m_tool->setTextCursor(m_shape, m_from);
    }
}
