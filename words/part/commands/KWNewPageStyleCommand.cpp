/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWNewPageStyleCommand.h"
#include "KWDocument.h"
#include "KWPageManager.h"

KWNewPageStyleCommand::KWNewPageStyleCommand(KWDocument *document, const KWPageStyle &newStyle, KUndo2Command *parent)
    : KUndo2Command(kundo2_i18n("Insert Page Style"), parent)
    , m_newStyle(newStyle)
    , m_document(document)
{
    Q_ASSERT(document);
}

void KWNewPageStyleCommand::redo()
{
    KUndo2Command::redo();
    m_document->pageManager()->addPageStyle(m_newStyle);
}

void KWNewPageStyleCommand::undo()
{
    KUndo2Command::undo();
    m_document->pageManager()->removePageStyle(m_newStyle);
}
