/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FilterRemoveCommand.h"
#include "KoFilterEffect.h"
#include "KoFilterEffectStack.h"
#include "KoShape.h"

#include <KLocalizedString>

FilterRemoveCommand::FilterRemoveCommand(int filterEffectIndex, KoFilterEffectStack *filterStack, KoShape *shape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_filterEffect(nullptr)
    , m_filterStack(filterStack)
    , m_shape(shape)
    , m_isRemoved(false)
    , m_filterEffectIndex(filterEffectIndex)
{
    Q_ASSERT(filterStack);
    setText(kundo2_i18n("Remove filter effect"));
}

FilterRemoveCommand::~FilterRemoveCommand()
{
    if (m_isRemoved)
        delete m_filterEffect;
}

void FilterRemoveCommand::redo()
{
    KUndo2Command::redo();

    if (m_shape)
        m_shape->update();

    m_filterEffect = m_filterStack->takeFilterEffect(m_filterEffectIndex);
    m_isRemoved = true;

    if (m_shape)
        m_shape->update();
}

void FilterRemoveCommand::undo()
{
    if (m_shape)
        m_shape->update();

    m_filterStack->insertFilterEffect(m_filterEffectIndex, m_filterEffect);
    m_isRemoved = false;

    if (m_shape)
        m_shape->update();

    KUndo2Command::undo();
}
