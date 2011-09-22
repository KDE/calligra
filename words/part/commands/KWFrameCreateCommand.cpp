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

#include "KWFrameCreateCommand.h"
#include "frames/KWFrame.h"
#include "frames/KWFrameSet.h"

#include <KoShapeCreateCommand.h>

#include <KLocale>

KWFrameCreateCommand::KWFrameCreateCommand(KoShapeBasedDocumentBase *shapeController, KWFrame *frame, KUndo2Command *parent)
        : KUndo2Command(i18nc("(qtundo-format)", "Create Frame"), parent),
        m_frame(frame),
        m_frameSet(frame->frameSet())
{
    new KoShapeCreateCommand(shapeController, frame->shape(), this);
}

void KWFrameCreateCommand::redo()
{
    m_frameSet->addFrame(m_frame);
    KUndo2Command::redo();
}

void KWFrameCreateCommand::undo()
{
    KUndo2Command::undo();
    m_frameSet->removeFrame(m_frame);
}

