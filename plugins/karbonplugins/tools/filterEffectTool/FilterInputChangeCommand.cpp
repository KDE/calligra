/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FilterInputChangeCommand.h"
#include "KoFilterEffect.h"
#include "KoShape.h"

FilterInputChangeCommand::FilterInputChangeCommand(const InputChangeData &data, KoShape *shape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(shape)
{
    m_data.append(data);
}

FilterInputChangeCommand::FilterInputChangeCommand(const QList<InputChangeData> &data, KoShape *shape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_shape(shape)
{
    m_data = data;
}

void FilterInputChangeCommand::redo()
{
    if (m_shape)
        m_shape->update();

    foreach (const InputChangeData &data, m_data) {
        data.filterEffect->setInput(data.inputIndex, data.newInput);
    }

    if (m_shape)
        m_shape->update();

    KUndo2Command::redo();
}

void FilterInputChangeCommand::undo()
{
    if (m_shape)
        m_shape->update();

    foreach (const InputChangeData &data, m_data) {
        data.filterEffect->setInput(data.inputIndex, data.oldInput);
    }

    if (m_shape)
        m_shape->update();

    KUndo2Command::undo();
}
