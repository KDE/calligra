/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextCommandBase.h"

KoTextCommandBase::KoTextCommandBase(KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_tool(nullptr)
{
}

KoTextCommandBase::~KoTextCommandBase() = default;

void KoTextCommandBase::redo()
{
    KUndo2Command::redo();
    if (m_tool) {
        m_tool->setAddUndoCommandAllowed(false);
    }
}

void KoTextCommandBase::setTool(KoUndoableTool *tool)
{
    m_tool = tool;
}

void KoTextCommandBase::undo()
{
    KUndo2Command::undo();
    if (m_tool) {
        m_tool->setAddUndoCommandAllowed(false);
    }
}

void KoTextCommandBase::setAllow(bool set)
{
    if (m_tool) {
        m_tool->setAddUndoCommandAllowed(set);
    }
}

KoTextCommandBase::UndoRedoFinalizer::~UndoRedoFinalizer()
{
    if (m_parent) {
        m_parent->setAllow(true);
    }
}
