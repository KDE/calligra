/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2011 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AddTextRangeCommand.h"
#include "ArtisticTextShape.h"
#include <KLocalizedString>

AddTextRangeCommand::AddTextRangeCommand(ArtisticTextTool *tool, ArtisticTextShape *shape, const QString &text, int from)
    : m_tool(tool)
    , m_shape(shape)
    , m_plainText(text)
    , m_formattedText(QString(), QFont())
    , m_from(from)
{
    setText(kundo2_i18n("Add text range"));
    m_oldFormattedText = shape->text();
}

AddTextRangeCommand::AddTextRangeCommand(ArtisticTextTool *tool, ArtisticTextShape *shape, const ArtisticTextRange &text, int from)
    : m_tool(tool)
    , m_shape(shape)
    , m_formattedText(text)
    , m_from(from)
{
    setText(kundo2_i18n("Add text range"));
    m_oldFormattedText = shape->text();
}

void AddTextRangeCommand::redo()
{
    KUndo2Command::redo();

    if (!m_shape)
        return;

    if (m_plainText.isEmpty())
        m_shape->insertText(m_from, m_formattedText);
    else
        m_shape->insertText(m_from, m_plainText);

    if (m_tool) {
        if (m_plainText.isEmpty())
            m_tool->setTextCursor(m_shape, m_from + m_formattedText.text().length());
        else
            m_tool->setTextCursor(m_shape, m_from + m_plainText.length());
    }
}

void AddTextRangeCommand::undo()
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
