/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWChangePageStyleCommand.h"
#include "../KWDocument.h"

KWChangePageStyleCommand::KWChangePageStyleCommand(KWDocument *document, KWPage &page, const KWPageStyle &newStyle, KUndo2Command *parent)
    : KUndo2Command(kundo2_i18n("Set Page Style"), parent)
    , m_document(document)
    , m_newStyle(newStyle)
    , m_oldStyle(page.pageStyle())
    , m_page(page)
{
    Q_ASSERT(m_page.isValid());
}

void KWChangePageStyleCommand::redo()
{
    KUndo2Command::redo();
    m_page.setPageStyle(m_newStyle);
    m_document->updatePagesForStyle(m_newStyle);
}

void KWChangePageStyleCommand::undo()
{
    KUndo2Command::undo();
    m_page.setPageStyle(m_oldStyle);
    m_document->updatePagesForStyle(m_newStyle);
}
