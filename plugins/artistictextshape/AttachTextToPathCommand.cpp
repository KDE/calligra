/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AttachTextToPathCommand.h"
#include "ArtisticTextShape.h"

#include <KLocalizedString>

AttachTextToPathCommand::AttachTextToPathCommand(ArtisticTextShape *textShape, KoPathShape *pathShape, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_textShape(textShape)
    , m_pathShape(pathShape)
{
    setText(kundo2_i18n("Attach Path"));
    m_oldMatrix = m_textShape->transformation();
}

void AttachTextToPathCommand::redo()
{
    KUndo2Command::redo();
    m_textShape->update();
    m_textShape->putOnPath(m_pathShape);
    m_textShape->update();
}

void AttachTextToPathCommand::undo()
{
    m_textShape->update();
    m_textShape->removeFromPath();
    m_textShape->setTransformation(m_oldMatrix);
    m_textShape->update();
    KUndo2Command::undo();
}
