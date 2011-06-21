/* This file is part of the KDE project
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWCLIPFRAMECOMMAND_H
#define KWCLIPFRAMECOMMAND_H

#include <kundo2command.h>
#include <QList>

class KWFrame;
class KWDocument;
class KoShapeContainer;

/// The undo / redo command for making a frame 'clipped'.
class KWClipFrameCommand : public KUndo2Command
{
public:
    explicit KWClipFrameCommand(const QList<KWFrame*> &frames, KWDocument *document, KUndo2Command *parent = 0);
    ~KWClipFrameCommand();

    /// (re)do the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

protected:
    KWDocument *m_document;
    QList<KWFrame*> m_frames;
    QList<KoShapeContainer*> m_clipShapes;
    bool m_ownClipShapes;
};

#endif
