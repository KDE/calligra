/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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
#ifndef KWCREATEOUTLINECOMMAND_H
#define KWCREATEOUTLINECOMMAND_H

#include <QUndoCommand>

class KWFrame;
class KoShapeControllerBase;
class KoShapeContainer;
class KWOutlineShape;

/// The undo / redo command for creating a custom-runaround outline for an existing shape
class KWCreateOutlineCommand : public QUndoCommand
{
public:
    explicit KWCreateOutlineCommand(KoShapeControllerBase *controller, KWFrame *frame, QUndoCommand *parent = 0);
    ~KWCreateOutlineCommand();

    /// (re)do the command
    virtual void redo();
    /// revert the actions done in redo
    virtual void undo();

private:
    KoShapeControllerBase *m_controller;
    KWFrame *m_frame;
    KoShapeContainer *m_container;
    KWOutlineShape *m_path;
    bool m_deleteOnExit;
};

#endif

