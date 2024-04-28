/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FilterStackSetCommand.h"
#include "KoFilterEffectStack.h"
#include "KoShape.h"

#include <KLocalizedString>

FilterStackSetCommand::FilterStackSetCommand(KoFilterEffectStack *newStack, KoShape *shape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_newFilterStack(newStack)
    , m_shape(shape)
{
    Q_ASSERT(m_shape);
    m_oldFilterStack = m_shape->filterEffectStack();
    if (m_newFilterStack)
        m_newFilterStack->ref();
    if (m_oldFilterStack)
        m_oldFilterStack->ref();

    setText(kundo2_i18n("Set filter stack"));
}

FilterStackSetCommand::~FilterStackSetCommand()
{
    if (m_newFilterStack && !m_newFilterStack->deref())
        delete m_newFilterStack;
    if (m_oldFilterStack && !m_oldFilterStack->deref())
        delete m_oldFilterStack;
}

void FilterStackSetCommand::redo()
{
    KUndo2Command::redo();

    m_shape->update();
    m_shape->setFilterEffectStack(m_newFilterStack);
    m_shape->update();
}

void FilterStackSetCommand::undo()
{
    m_shape->update();
    m_shape->setFilterEffectStack(m_oldFilterStack);
    m_shape->update();

    KUndo2Command::undo();
}
