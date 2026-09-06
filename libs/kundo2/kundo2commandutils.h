/*
 * SPDX-FileCopyrightText: 2026 Calligra contributors
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include "kundo2stack.h"

#include <functional>
#include <memory>

class KUNDO2_EXPORT KUndo2AggregateCommand : public KUndo2Command
{
public:
    explicit KUndo2AggregateCommand(KUndo2Command *parent = nullptr);
    KUndo2AggregateCommand(const KUndo2MagicString &text, KUndo2Command *parent = nullptr);

    void redo() override;
    void undo() override;

protected:
    virtual void populateChildCommands() = 0;
    void addCommand(std::unique_ptr<KUndo2Command> &&command);

private:
    bool m_firstRedo = true;
};

class KUNDO2_EXPORT KUndo2LambdaCommand final : public KUndo2AggregateCommand
{
public:
    explicit KUndo2LambdaCommand(std::function<std::unique_ptr<KUndo2Command>()> factory, KUndo2Command *parent = nullptr);
    KUndo2LambdaCommand(const KUndo2MagicString &text, std::function<std::unique_ptr<KUndo2Command>()> factory, KUndo2Command *parent = nullptr);

protected:
    void populateChildCommands() override;

private:
    std::function<std::unique_ptr<KUndo2Command>()> m_factory;
};

class KUNDO2_EXPORT KUndo2SkipFirstRedoCommand final : public KUndo2Command
{
public:
    explicit KUndo2SkipFirstRedoCommand(std::unique_ptr<KUndo2Command> command, KUndo2Command *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    bool m_firstRedo = true;
    std::unique_ptr<KUndo2Command> m_command;
};

class KUNDO2_EXPORT KUndo2FlipFlopCommand : public KUndo2Command
{
public:
    enum class State {
        Initializing,
        Finalizing
    };
    explicit KUndo2FlipFlopCommand(State state = State::Initializing, KUndo2Command *parent = nullptr);
    void redo() override;
    void undo() override;

protected:
    virtual void partA();
    virtual void partB();

private:
    State m_state;
};

class KUNDO2_EXPORT KUndo2CompositeCommand final : public KUndo2Command
{
public:
    explicit KUndo2CompositeCommand(KUndo2Command *parent = nullptr);
    void addCommand(std::unique_ptr<KUndo2Command> &&command);
};

KUNDO2_EXPORT std::unique_ptr<KUndo2Command> composeKUndo2Commands(std::unique_ptr<KUndo2Command> parent, std::unique_ptr<KUndo2Command> command);
