/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FilterRegionChangeCommand.h"
#include "KoFilterEffect.h"
#include "KoShape.h"

FilterRegionChangeCommand::FilterRegionChangeCommand(KoFilterEffect *effect, const QRectF &filterRegion, KoShape *shape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_effect(effect)
    , m_newRegion(filterRegion)
    , m_shape(shape)
{
    Q_ASSERT(m_effect);
    m_oldRegion = m_effect->filterRect();
}

void FilterRegionChangeCommand::redo()
{
    if (m_shape)
        m_shape->update();

    m_effect->setFilterRect(m_newRegion);

    if (m_shape) {
        m_shape->update();
        m_shape->notifyChanged();
    }

    KUndo2Command::redo();
}

void FilterRegionChangeCommand::undo()
{
    if (m_shape)
        m_shape->update();

    m_effect->setFilterRect(m_oldRegion);

    if (m_shape) {
        m_shape->update();
        m_shape->notifyChanged();
    }

    KUndo2Command::undo();
}
