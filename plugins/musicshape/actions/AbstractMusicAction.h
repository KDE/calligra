/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ABSTRACTMUSICACTION_H
#define ABSTRACTMUSICACTION_H

#include <QAction>
#include <QKeyEvent>

class SimpleEntryTool;
class MusicCursor;
namespace MusicCore
{
class Staff;
}

class QIcon;

class AbstractMusicAction : public QAction
{
    Q_OBJECT
public:
    AbstractMusicAction(const QIcon &icon, const QString &text, SimpleEntryTool *tool);
    AbstractMusicAction(const QString &text, SimpleEntryTool *tool);

    virtual void renderPreview(QPainter &painter, const QPointF &point);
    virtual void mousePress(MusicCore::Staff *staff, int bar, const QPointF &pos) = 0;
    virtual void mouseMove(MusicCore::Staff *staff, int bar, const QPointF &pos);
    bool isVoiceAware();

    virtual void renderKeyboardPreview(QPainter &painter, const MusicCursor &cursor);
    virtual void keyPress(QKeyEvent *event, const MusicCursor &cursor);

protected:
    bool m_isVoiceAware;
    SimpleEntryTool *m_tool;
};

#endif // ABSTRACTMUSICACTION_H
