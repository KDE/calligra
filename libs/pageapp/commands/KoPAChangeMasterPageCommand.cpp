/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPAChangeMasterPageCommand.h"

#include <KLocalizedString>

#include "KoPADocument.h"
#include "KoPAPage.h"

KoPAChangeMasterPageCommand::KoPAChangeMasterPageCommand(KoPADocument *document, KoPAPage *page, KoPAMasterPage *masterPage)
    : m_document(document)
    , m_page(page)
    , m_oldMasterPage(page->masterPage())
    , m_newMasterPage(masterPage)
{
    if (m_page->pageType() == KoPageApp::Slide) {
        setText(kundo2_i18n("Change master slide"));
    } else {
        setText(kundo2_i18n("Change master page"));
    }
}

KoPAChangeMasterPageCommand::~KoPAChangeMasterPageCommand() = default;

void KoPAChangeMasterPageCommand::redo()
{
    m_page->setMasterPage(m_newMasterPage);
    m_document->emitUpdate(m_page);
}

void KoPAChangeMasterPageCommand::undo()
{
    m_page->setMasterPage(m_oldMasterPage);
    m_document->emitUpdate(m_page);
}
