/*
 * SPDX-FileCopyrightText: 2026 Calligra contributors
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include "kundo2_export.h"
#include "kundo2magicstring.h"

class KUndo2Command;
class KUndo2QStack;

class KUNDO2_EXPORT KUndo2UndoStore
{
public:
    virtual ~KUndo2UndoStore() = default;
    virtual const KUndo2Command *presentCommand() const = 0;
    virtual void undoLastCommand() = 0;
    virtual void addCommand(KUndo2Command *command) = 0;
    virtual void beginMacro(const KUndo2MagicString &text) = 0;
    virtual void endMacro() = 0;
    virtual void purgeRedoState() = 0;
};

class KUNDO2_EXPORT KUndo2StackUndoStore final : public KUndo2UndoStore
{
public:
    explicit KUndo2StackUndoStore(KUndo2QStack *stack);

    const KUndo2Command *presentCommand() const override;
    void undoLastCommand() override;
    void addCommand(KUndo2Command *command) override;
    void beginMacro(const KUndo2MagicString &text) override;
    void endMacro() override;
    void purgeRedoState() override;

private:
    KUndo2QStack *m_stack;
};

class KUNDO2_EXPORT KUndo2NullUndoStore final : public KUndo2UndoStore
{
public:
    const KUndo2Command *presentCommand() const override;
    void undoLastCommand() override;
    void addCommand(KUndo2Command *command) override;
    void beginMacro(const KUndo2MagicString &text) override;
    void endMacro() override;
    void purgeRedoState() override;
};
