/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ABSTRACTNOTEMUSICACTION_H
#define ABSTRACTNOTEMUSICACTION_H

#include "AbstractMusicAction.h"

class SimpleEntryTool;
namespace MusicCore
{
class Chord;
class Note;
class StaffElement;
}

class AbstractNoteMusicAction : public AbstractMusicAction
{
    Q_OBJECT
public:
    AbstractNoteMusicAction(const QIcon &icon, const QString &text, SimpleEntryTool *tool);
    AbstractNoteMusicAction(const QString &text, SimpleEntryTool *tool);

    void mousePress(MusicCore::Staff *staff, int bar, const QPointF &pos) override;
    void mouseMove(MusicCore::Staff *staff, int bar, const QPointF &pos) override;

    virtual void mousePress(MusicCore::Chord *chord, MusicCore::Note *note, qreal distance, const QPointF &pos) = 0;
    virtual void mouseMove(MusicCore::Chord *chord, MusicCore::Note *note, qreal distance, const QPointF &pos);

    virtual void mousePress(MusicCore::StaffElement *se, qreal distance, const QPointF &pos);
    virtual void mouseMove(MusicCore::StaffElement *se, qreal distance, const QPointF &pos);
};

#endif // ABSTRACTMUSICACTION_H
