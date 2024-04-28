/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "AbstractMusicAction.h"
#include "../SimpleEntryTool.h"

#include <QIcon>

#include <QPainter>

AbstractMusicAction::AbstractMusicAction(const QIcon &icon, const QString &text, SimpleEntryTool *tool)
    : QAction(icon, text, tool)
    , m_isVoiceAware(false)
    , m_tool(tool)
{
    setCheckable(true);
}

AbstractMusicAction::AbstractMusicAction(const QString &text, SimpleEntryTool *tool)
    : QAction(text, tool)
    , m_isVoiceAware(false)
    , m_tool(tool)
{
    setCheckable(true);
}

void AbstractMusicAction::renderPreview(QPainter &painter, const QPointF &point)
{
    Q_UNUSED(painter);
    Q_UNUSED(point);
}

bool AbstractMusicAction::isVoiceAware()
{
    return m_isVoiceAware;
}

void AbstractMusicAction::mouseMove(MusicCore::Staff *, int, const QPointF &)
{
}

void AbstractMusicAction::renderKeyboardPreview(QPainter &painter, const MusicCursor & /*cursor*/)
{
    Q_UNUSED(painter);
}

void AbstractMusicAction::keyPress(QKeyEvent *event, const MusicCursor & /*cursor*/)
{
    Q_UNUSED(event);
}
