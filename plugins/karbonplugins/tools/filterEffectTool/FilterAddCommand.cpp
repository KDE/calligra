/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FilterAddCommand.h"
#include "KoFilterEffect.h"
#include "KoFilterEffectStack.h"
#include "KoShape.h"

#include <KLocalizedString>

FilterAddCommand::FilterAddCommand(KoFilterEffect *filterEffect, KoShape *shape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_filterEffect(filterEffect)
    , m_shape(shape)
    , m_isAdded(false)
{
    Q_ASSERT(m_shape);
    setText(kundo2_i18n("Add filter effect"));
}

FilterAddCommand::~FilterAddCommand()
{
    if (!m_isAdded)
        delete m_filterEffect;
}

void FilterAddCommand::redo()
{
    KUndo2Command::redo();

    if (m_shape->filterEffectStack()) {
        m_shape->update();
        m_shape->filterEffectStack()->appendFilterEffect(m_filterEffect);
        m_shape->update();
        m_isAdded = true;
    }
}

void FilterAddCommand::undo()
{
    if (m_shape->filterEffectStack()) {
        int index = m_shape->filterEffectStack()->filterEffects().indexOf(m_filterEffect);
        if (index >= 0) {
            m_shape->update();
            m_shape->filterEffectStack()->takeFilterEffect(index);
            m_shape->update();
        }
        m_isAdded = false;
    }
    KUndo2Command::undo();
}
