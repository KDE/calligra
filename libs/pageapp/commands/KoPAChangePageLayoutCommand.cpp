/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPAChangePageLayoutCommand.h"

#include <KLocalizedString>

#include "KoPADocument.h"
#include "KoPAMasterPage.h"

KoPAChangePageLayoutCommand::KoPAChangePageLayoutCommand(KoPADocument *document,
                                                         KoPAMasterPage *masterPage,
                                                         const KoPageLayout &newPageLayout,
                                                         bool applyToDocument,
                                                         KUndo2Command *parent)
    : KUndo2Command(parent)
    , m_document(document)
    , m_newPageLayout(newPageLayout)
{
    setText(kundo2_i18n("Set Page Layout"));

    if (!applyToDocument) {
        m_oldLayouts.insert(masterPage, masterPage->pageLayout());
    } else {
        QList<KoPAPageBase *> masterPages = m_document->pages(true);
        foreach (KoPAPageBase *page, masterPages) {
            KoPAMasterPage *masterPage = static_cast<KoPAMasterPage *>(page);
            m_oldLayouts.insert(masterPage, masterPage->pageLayout());
        }
    }
}

KoPAChangePageLayoutCommand::~KoPAChangePageLayoutCommand() = default;

void KoPAChangePageLayoutCommand::redo()
{
    QMap<KoPAMasterPage *, KoPageLayout>::const_iterator it = m_oldLayouts.constBegin();
    while (it != m_oldLayouts.constEnd()) {
        it.key()->setPageLayout(m_newPageLayout);
        m_document->emitUpdate(it.key());
        ++it;
    }
}

void KoPAChangePageLayoutCommand::undo()
{
    QMap<KoPAMasterPage *, KoPageLayout>::const_iterator it = m_oldLayouts.constBegin();
    while (it != m_oldLayouts.constEnd()) {
        it.key()->setPageLayout(it.value());
        m_document->emitUpdate(it.key());
        ++it;
    }
}
