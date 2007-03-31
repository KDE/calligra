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

#include "KWFrameDeleteCommand.h"
#include "frames/KWFrame.h"
#include "frames/KWFrameSet.h"

#include <KoShapeDeleteCommand.h>

#include <KLocale>

KWFrameDeleteCommand::KWFrameDeleteCommand(KoShapeControllerBase *shapeController, KWFrame *frame, QUndoCommand *parent)
    :QUndoCommand(i18n("Delete Frame"), parent),
    m_frame(frame),
    m_frameSet(frame->frameSet())
{
    new KoShapeDeleteCommand(shapeController, frame->shape(), this);
}

void KWFrameDeleteCommand::redo() {
    QUndoCommand::redo();
    m_frameSet->removeFrame(m_frame);
}

void KWFrameDeleteCommand::undo() {
    QUndoCommand::undo();
    m_frameSet->addFrame(m_frame);
}

