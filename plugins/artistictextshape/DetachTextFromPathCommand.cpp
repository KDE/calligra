/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DetachTextFromPathCommand.h"
#include "ArtisticTextShape.h"
#include <KLocalizedString>

DetachTextFromPathCommand::DetachTextFromPathCommand(ArtisticTextShape *textShape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_textShape(textShape)
    , m_pathShape(nullptr)
{
    setText(kundo2_i18n("Detach Path"));

    Q_ASSERT(m_textShape->layout() != ArtisticTextShape::Straight);

    if (m_textShape->layout() == ArtisticTextShape::OnPath)
        m_path = m_textShape->baseline();
    else
        m_pathShape = m_textShape->baselineShape();
}

void DetachTextFromPathCommand::redo()
{
    KUndo2Command::redo();

    m_textShape->update();
    m_textShape->removeFromPath();
    m_textShape->update();
}

void DetachTextFromPathCommand::undo()
{
    m_textShape->update();

    if (m_pathShape)
        m_textShape->putOnPath(m_pathShape);
    else
        m_textShape->putOnPath(m_path);

    m_textShape->update();

    KUndo2Command::undo();
}
