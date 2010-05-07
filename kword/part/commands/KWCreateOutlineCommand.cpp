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

#include "KWCreateOutlineCommand.h"
#include "../frames/KWOutlineShape.h"
#include "../frames/KWFrame.h"

#include <KoShapeControllerBase.h>
#include <KoShapeContainer.h>

#include <KLocale>

KWCreateOutlineCommand::KWCreateOutlineCommand(KoShapeControllerBase *controller, KWFrame *frame, QUndoCommand *parent)
        : QUndoCommand(i18n("Create custom outline"), parent),
        m_controller(controller),
        m_frame(frame),
        m_container(0),
        m_path(0),
        m_deleteOnExit(false)
{
}

KWCreateOutlineCommand::~KWCreateOutlineCommand()
{
    if (m_deleteOnExit) {
        delete m_container;
        delete m_path;
    }
}

void KWCreateOutlineCommand::redo()
{
    QUndoCommand::redo();
    if (m_container == 0) {
        m_path = new KWOutlineShape(m_frame);
        m_container = m_path->parent();
    } else {
        KoShape *child = m_frame->shape();
        m_container->setTransformation(child->absoluteTransformation(0));
        QMatrix matrix;
        child->setTransformation(matrix);
        m_container->addShape(child);
        m_container->setApplicationData(m_frame);
    }
    m_frame->setOutlineShape(m_path);
    m_controller->addShape(m_container);
    m_deleteOnExit = false;
}

void KWCreateOutlineCommand::undo()
{
    QUndoCommand::undo();
    Q_ASSERT(m_container);
    KoShape *child = m_frame->shape();
    child->setTransformation(m_container->absoluteTransformation(0));
    m_container->removeShape(m_frame->shape());
    m_container->setApplicationData(0);
    m_controller->removeShape(m_container);
    m_frame->setOutlineShape(0);
    m_deleteOnExit = true;
}
