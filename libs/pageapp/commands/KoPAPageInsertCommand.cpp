/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPAPageInsertCommand.h"

#include <KLocalizedString>

#include "KoPADocument.h"
#include "KoPAPageBase.h"

KoPAPageInsertCommand::KoPAPageInsertCommand(KoPADocument *document, KoPAPageBase *page, KoPAPageBase *after, KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_document(document)
    , m_page(page)
    , m_after(after)
    , m_deletePage(true)
{
    Q_ASSERT(document);
    Q_ASSERT(page);
    if (m_page->pageType() == KoPageApp::Slide) {
        setText(kundo2_i18n("Insert slide"));
    } else {
        setText(kundo2_i18n("Insert page"));
    }
}

KoPAPageInsertCommand::~KoPAPageInsertCommand()
{
    if (m_deletePage) {
        delete m_page;
    }
}

void KoPAPageInsertCommand::redo()
{
    m_document->insertPage(m_page, m_after);
    m_deletePage = false;
}

void KoPAPageInsertCommand::undo()
{
    m_document->takePage(m_page);
    m_deletePage = true;
}
