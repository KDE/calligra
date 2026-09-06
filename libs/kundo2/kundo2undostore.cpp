/*
 * SPDX-FileCopyrightText: 2026 Calligra contributors
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "kundo2undostore.h"

#include "kundo2stack.h"

KUndo2StackUndoStore::KUndo2StackUndoStore(KUndo2QStack *stack)
    : m_stack(stack)
{
}

const KUndo2Command *KUndo2StackUndoStore::presentCommand() const
{
    return m_stack && m_stack->index() > 0 ? m_stack->command(m_stack->index() - 1) : nullptr;
}

void KUndo2StackUndoStore::undoLastCommand()
{
    if (m_stack) {
        m_stack->undo();
    }
}

void KUndo2StackUndoStore::addCommand(KUndo2Command *command)
{
    if (m_stack) {
        m_stack->push(command);
    } else {
        delete command;
    }
}

void KUndo2StackUndoStore::beginMacro(const KUndo2MagicString &text)
{
    if (m_stack) {
        m_stack->beginMacro(text);
    }
}

void KUndo2StackUndoStore::endMacro()
{
    if (m_stack) {
        m_stack->endMacro();
    }
}

void KUndo2StackUndoStore::purgeRedoState()
{
    if (m_stack) {
        m_stack->purgeRedoState();
    }
}

const KUndo2Command *KUndo2NullUndoStore::presentCommand() const
{
    return nullptr;
}

void KUndo2NullUndoStore::undoLastCommand()
{
}

void KUndo2NullUndoStore::addCommand(KUndo2Command *command)
{
    delete command;
}

void KUndo2NullUndoStore::beginMacro(const KUndo2MagicString &text)
{
    Q_UNUSED(text);
}

void KUndo2NullUndoStore::endMacro()
{
}

void KUndo2NullUndoStore::purgeRedoState()
{
}
