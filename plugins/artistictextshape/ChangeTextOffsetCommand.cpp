/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChangeTextOffsetCommand.h"
#include "ArtisticTextShape.h"

#include <KLocalizedString>

ChangeTextOffsetCommand::ChangeTextOffsetCommand(ArtisticTextShape *textShape, qreal oldOffset, qreal newOffset, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_textShape(textShape)
    , m_oldOffset(oldOffset)
    , m_newOffset(newOffset)
{
    setText(kundo2_i18n("Change Text Offset"));
}

void ChangeTextOffsetCommand::redo()
{
    KUndo2Command::redo();
    m_textShape->update();
    m_textShape->setStartOffset(m_newOffset);
    m_textShape->update();
}

void ChangeTextOffsetCommand::undo()
{
    m_textShape->update();
    m_textShape->setStartOffset(m_oldOffset);
    m_textShape->update();
    KUndo2Command::undo();
}
