/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef NOTEENTRYACTION_H
#define NOTEENTRYACTION_H

#include "../core/Global.h"
#include "AbstractMusicAction.h"

class NoteEntryAction : public AbstractMusicAction
{
public:
    NoteEntryAction(MusicCore::Duration duration, bool isRest, SimpleEntryTool *tool);

    void renderPreview(QPainter &painter, const QPointF &point) override;
    void mousePress(MusicCore::Staff *staff, int bar, const QPointF &pos) override;

    void renderKeyboardPreview(QPainter &painter, const MusicCursor &cursor) override;
    void keyPress(QKeyEvent *event, const MusicCursor &cursor) override;

private:
    MusicCore::Duration m_duration;
    bool m_isRest;
};

#endif // NOTEENTRYACTION_H
