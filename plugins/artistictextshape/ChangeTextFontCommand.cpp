/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2011 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Rob Buis <buis@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChangeTextFontCommand.h"
#include "ArtisticTextShape.h"
#include <KLocalizedString>

ChangeTextFontCommand::ChangeTextFontCommand(ArtisticTextShape *shape, const QFont &font, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(shape)
    , m_newFont(font)
    , m_rangeStart(-1)
    , m_rangeCount(-1)
{
    Q_ASSERT(m_shape);
    setText(kundo2_i18n("Change font"));
}

ChangeTextFontCommand::ChangeTextFontCommand(ArtisticTextShape *shape, int from, int count, const QFont &font, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(shape)
    , m_newFont(font)
    , m_rangeStart(from)
    , m_rangeCount(count)
{
    Q_ASSERT(m_shape);
}

void ChangeTextFontCommand::redo()
{
    if (m_oldText.isEmpty()) {
        m_oldText = m_shape->text();
        if (m_rangeStart >= 0) {
            m_shape->setFont(m_rangeStart, m_rangeCount, m_newFont);
        } else {
            m_shape->setFont(m_newFont);
        }
        if (m_newText.isEmpty()) {
            m_newText = m_shape->text();
        }
    } else {
        m_shape->clear();
        foreach (const ArtisticTextRange &range, m_newText) {
            m_shape->appendText(range);
        }
    }
}

void ChangeTextFontCommand::undo()
{
    m_shape->clear();
    foreach (const ArtisticTextRange &range, m_oldText) {
        m_shape->appendText(range);
    }
}
