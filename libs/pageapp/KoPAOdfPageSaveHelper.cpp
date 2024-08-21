/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPAOdfPageSaveHelper.h"

#include <QSet>

#include "KoPADocument.h"
#include "KoPAMasterPage.h"
#include "KoPAPage.h"
#include "KoPASavingContext.h"
#include <KoXmlWriter.h>

KoPAOdfPageSaveHelper::KoPAOdfPageSaveHelper(KoPADocument *doc, QList<KoPAPageBase *> pages)
    : m_doc(doc)
    , m_context(nullptr)
{
    foreach (KoPAPageBase *page, pages) {
        if (dynamic_cast<KoPAPage *>(page)) {
            m_pages.append(page);
        } else {
            m_masterPages.append(page);
        }
    }

    if (m_pages.size() > 0) {
        m_masterPages.clear();

        // this might result in a different order of master pages when copying to a different document
        QSet<KoPAPageBase *> masterPages;
        foreach (KoPAPageBase *page, m_pages) {
            KoPAPage *p = static_cast<KoPAPage *>(page);
            masterPages.insert(p->masterPage());
        }
        m_masterPages = masterPages.values();
    }
}

KoPAOdfPageSaveHelper::~KoPAOdfPageSaveHelper()
{
    delete m_context;
}

KoShapeSavingContext *KoPAOdfPageSaveHelper::context(KoXmlWriter *bodyWriter, KoGenStyles &mainStyles, KoEmbeddedDocumentSaver &embeddedSaver)
{
    m_context = new KoPASavingContext(*bodyWriter, mainStyles, embeddedSaver, 1);
    return m_context;
}

bool KoPAOdfPageSaveHelper::writeBody()
{
    Q_ASSERT(m_context);
    if (m_context) {
        m_doc->saveOdfDocumentStyles(*(static_cast<KoPASavingContext *>(m_context)));
        KoXmlWriter &bodyWriter = static_cast<KoPASavingContext *>(m_context)->xmlWriter();
        bodyWriter.startElement("office:body");
        bodyWriter.startElement(m_doc->odfTagName(true));

        if (!m_doc->saveOdfPages(*(static_cast<KoPASavingContext *>(m_context)), m_pages, m_masterPages)) {
            return false;
        }

        bodyWriter.endElement(); // office:odfTagName()
        bodyWriter.endElement(); // office:body

        return true;
    }
    return false;
}
