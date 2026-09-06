/*
 * SPDX-FileCopyrightText: 2026 Calligra contributors
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "kundo2commandutils.h"

KUndo2AggregateCommand::KUndo2AggregateCommand(KUndo2Command *parent)
    : KUndo2Command(parent)
{
}

KUndo2AggregateCommand::KUndo2AggregateCommand(const KUndo2MagicString &text, KUndo2Command *parent)
    : KUndo2Command(text, parent)
{
}

void KUndo2AggregateCommand::redo()
{
    if (m_firstRedo) {
        m_firstRedo = false;
        populateChildCommands();
    }
    KUndo2Command::redo();
}

void KUndo2AggregateCommand::undo()
{
    KUndo2Command::undo();
}

void KUndo2AggregateCommand::addCommand(std::unique_ptr<KUndo2Command> &&command)
{
    if (command) {
        KUndo2Command::addCommand(std::move(command));
    }
}

KUndo2LambdaCommand::KUndo2LambdaCommand(std::function<std::unique_ptr<KUndo2Command>()> factory, KUndo2Command *parent)
    : KUndo2AggregateCommand(parent)
    , m_factory(std::move(factory))
{
}

KUndo2LambdaCommand::KUndo2LambdaCommand(const KUndo2MagicString &text, std::function<std::unique_ptr<KUndo2Command>()> factory, KUndo2Command *parent)
    : KUndo2AggregateCommand(text, parent)
    , m_factory(std::move(factory))
{
}

void KUndo2LambdaCommand::populateChildCommands()
{
    if (m_factory) {
        addCommand(m_factory());
        m_factory = {};
    }
}

KUndo2SkipFirstRedoCommand::KUndo2SkipFirstRedoCommand(std::unique_ptr<KUndo2Command> command, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_command(std::move(command))
{
}

void KUndo2SkipFirstRedoCommand::redo()
{
    if (m_firstRedo) {
        m_firstRedo = false;
        return;
    }
    if (m_command) {
        m_command->redo();
    }
    KUndo2Command::redo();
}

void KUndo2SkipFirstRedoCommand::undo()
{
    KUndo2Command::undo();
    if (m_command) {
        m_command->undo();
    }
}

KUndo2FlipFlopCommand::KUndo2FlipFlopCommand(State state, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_state(state)
{
}

void KUndo2FlipFlopCommand::redo()
{
    m_state == State::Initializing ? partA() : partB();
    m_state = State::Finalizing;
}

void KUndo2FlipFlopCommand::undo()
{
    m_state == State::Finalizing ? partA() : partB();
    m_state = State::Initializing;
}

void KUndo2FlipFlopCommand::partA()
{
}

void KUndo2FlipFlopCommand::partB()
{
}

KUndo2CompositeCommand::KUndo2CompositeCommand(KUndo2Command *parent)
    : KUndo2Command(parent)
{
}

void KUndo2CompositeCommand::addCommand(std::unique_ptr<KUndo2Command> &&command)
{
    KUndo2Command::addCommand(std::move(command));
}

std::unique_ptr<KUndo2Command> composeKUndo2Commands(std::unique_ptr<KUndo2Command> parent, std::unique_ptr<KUndo2Command> command)
{
    if (!parent) {
        return command;
    }
    if (!command) {
        return parent;
    }
    auto composite = std::make_unique<KUndo2CompositeCommand>();
    composite->setText(parent->text());
    composite->addCommand(std::move(parent));
    composite->addCommand(std::move(command));
    return composite;
}
