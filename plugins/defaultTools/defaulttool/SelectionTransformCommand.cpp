/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SelectionTransformCommand.h"
#include <KoSelection.h>

SelectionTransformCommand::SelectionTransformCommand(KoSelection *selection,
                                                     const QTransform &oldTransformation,
                                                     const QTransform &newTransformation,
                                                     KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_selection(selection)
    , m_oldTransformation(oldTransformation)
    , m_newTransformation(newTransformation)
{
    Q_ASSERT(m_selection);
    m_selectedShapes = m_selection->selectedShapes();
}

void SelectionTransformCommand::redo()
{
    KUndo2Command::redo();

    m_selection->blockSignals(true);

    m_selection->deselectAll();
    foreach (KoShape *shape, m_selectedShapes)
        m_selection->select(shape, false);

    m_selection->setTransformation(m_newTransformation);

    m_selection->blockSignals(false);
}

void SelectionTransformCommand::undo()
{
    m_selection->blockSignals(true);

    m_selection->deselectAll();
    foreach (KoShape *shape, m_selectedShapes)
        m_selection->select(shape, false);

    m_selection->setTransformation(m_oldTransformation);

    m_selection->blockSignals(false);

    KUndo2Command::undo();
}
