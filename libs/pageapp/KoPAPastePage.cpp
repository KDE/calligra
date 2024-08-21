/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPAPastePage.h"

#include "KoPADocument.h"
#include "KoPALoadingContext.h"
#include "KoPAMasterPage.h"
#include "KoPAPage.h"
#include "KoPASavingContext.h"
#include "commands/KoPAPageInsertCommand.h"
#include <KoEmbeddedDocumentSaver.h>
#include <KoGenStyles.h>
#include <KoOdfLoadingContext.h>
#include <KoOdfReadStore.h>
#include <KoXmlWriter.h>
#include <QBuffer>
#include <QString>

KoPAPastePage::KoPAPastePage(KoPADocument *doc, KoPAPageBase *activePage)
    : m_doc(doc)
    , m_activePage(activePage)
{
}

bool KoPAPastePage::process(const KoXmlElement &body, KoOdfReadStore &odfStore)
{
    KoOdfLoadingContext loadingContext(odfStore.styles(), odfStore.store(), m_doc->defaultStylesResourcePath());
    KoPALoadingContext paContext(loadingContext, m_doc->resourceManager());

    QList<KoPAPageBase *> newMasterPages(m_doc->loadOdfMasterPages(odfStore.styles().masterPages(), paContext));
    QList<KoPAPageBase *> newPages(m_doc->loadOdfPages(body, paContext));

    // Check where to start inserting pages
    KoPAPageBase *insertAfterPage = nullptr;
    KoPAPageBase *insertAfterMasterPage = nullptr;
    if (dynamic_cast<KoPAMasterPage *>(m_activePage) || (m_activePage == nullptr && newPages.empty())) {
        insertAfterMasterPage = m_activePage;
        insertAfterPage = m_doc->pages(false).last();
    } else {
        insertAfterPage = m_activePage;
        insertAfterMasterPage = m_doc->pages(true).last();
    }

    if (!newPages.empty()) {
        KoGenStyles mainStyles;
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        KoXmlWriter xmlWriter(&buffer);
        KoEmbeddedDocumentSaver embeddedSaver;
        KoPASavingContext savingContext(xmlWriter, mainStyles, embeddedSaver, 1);
        savingContext.addOption(KoShapeSavingContext::UniqueMasterPages);
        QList<KoPAPageBase *> emptyList;
        QList<KoPAPageBase *> existingMasterPages = m_doc->pages(true);
        savingContext.setClearDrawIds(true);
        m_doc->saveOdfPages(savingContext, emptyList, existingMasterPages);

        QMap<QString, KoPAMasterPage *> masterPageNames;

        foreach (KoPAPageBase *page, existingMasterPages) {
            KoPAMasterPage *masterPage = dynamic_cast<KoPAMasterPage *>(page);
            Q_ASSERT(masterPage);
            if (masterPage) {
                QString masterPageName(savingContext.masterPageName(masterPage));
                if (!masterPageNames.contains(masterPageName)) {
                    masterPageNames.insert(masterPageName, masterPage);
                }
            }
        }

        m_doc->saveOdfPages(savingContext, emptyList, newMasterPages);

        QHash<KoPAMasterPage *, KoPAMasterPage *> masterPagesToUpdate;
        foreach (KoPAPageBase *page, newMasterPages) {
            KoPAMasterPage *masterPage = dynamic_cast<KoPAMasterPage *>(page);
            Q_ASSERT(masterPage);
            if (masterPage) {
                QString masterPageName(savingContext.masterPageName(masterPage));
                QMap<QString, KoPAMasterPage *>::const_iterator existingMasterPage(masterPageNames.constFind(masterPageName));
                if (existingMasterPage != masterPageNames.constEnd()) {
                    masterPagesToUpdate.insert(masterPage, existingMasterPage.value());
                }
            }
        }

        // update pages which have a duplicate master page
        foreach (KoPAPageBase *page, newPages) {
            KoPAPage *p = dynamic_cast<KoPAPage *>(page);
            Q_ASSERT(p);
            if (p) {
                KoPAMasterPage *masterPage(p->masterPage());
                auto pageIt(masterPagesToUpdate.constFind(masterPage));
                if (pageIt != masterPagesToUpdate.constEnd()) {
                    p->setMasterPage(pageIt.value());
                }
            }
        }

        // delete duplicate master pages;
        auto pageIt(masterPagesToUpdate.constBegin());
        for (; pageIt != masterPagesToUpdate.constEnd(); ++pageIt) {
            newMasterPages.removeAll(pageIt.key());
            delete pageIt.key();
        }
    }

    KUndo2Command *cmd = nullptr;
    if (m_doc->pageType() == KoPageApp::Slide) {
        cmd = new KUndo2Command(kundo2_i18np("Paste Slide", "Paste Slides", qMax(newMasterPages.size(), newPages.size())));
    } else {
        cmd = new KUndo2Command(kundo2_i18np("Paste Page", "Paste Pages", qMax(newMasterPages.size(), newPages.size())));
    }

    foreach (KoPAPageBase *masterPage, newMasterPages) {
        new KoPAPageInsertCommand(m_doc, masterPage, insertAfterMasterPage, cmd);
        insertAfterMasterPage = masterPage;
    }

    foreach (KoPAPageBase *page, newPages) {
        new KoPAPageInsertCommand(m_doc, page, insertAfterPage, cmd);
        insertAfterPage = page;
    }

    m_doc->addCommand(cmd);

    return true;
}
