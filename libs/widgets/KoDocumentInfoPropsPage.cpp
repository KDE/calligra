/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999, 2000 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2004, 2010 David Faure <faure@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoDocumentInfoPropsPage.h"

#include <QIcon>

#include "KoDocumentInfo.h"
#include "KoDocumentInfoDlg.h"
#include "KoOdfReadStore.h"
#include "KoStore.h"
#include <QUrl>
#include <WidgetsDebug.h>

class KoDocumentInfoPropsPage::KoDocumentInfoPropsPagePrivate
{
public:
    KoDocumentInfo *m_info;
    KoDocumentInfoDlg *m_dlg;
    QUrl m_url;
    KoStore *m_src;
    KoStore *m_dst;
};

KoDocumentInfoPropsPage::KoDocumentInfoPropsPage(KPropertiesDialog *props, const QVariantList &)
    : KPropertiesDialogPlugin(props)
    , d(new KoDocumentInfoPropsPagePrivate)
{
    d->m_info = new KoDocumentInfo(this);
    d->m_url = props->item().url();
    d->m_dlg = nullptr;

    if (!d->m_url.isLocalFile())
        return;

    d->m_dst = nullptr;

    d->m_src = KoStore::createStore(d->m_url.toLocalFile(), KoStore::Read);

    if (d->m_src->bad()) {
        return; // the store will be deleted in the dtor
    }

    // OASIS/OOo file format?
    if (d->m_src->hasFile("meta.xml")) {
        KoXmlDocument metaDoc;
        KoOdfReadStore oasisStore(d->m_src);
        QString lastErrorMessage;
        if (oasisStore.loadAndParse("meta.xml", metaDoc, lastErrorMessage)) {
            d->m_info->loadOasis(metaDoc);
        }
    }
    // Old calligra file format?
    else if (d->m_src->hasFile("documentinfo.xml")) {
        if (d->m_src->open("documentinfo.xml")) {
            KoXmlDocument doc;
            if (doc.setContent(d->m_src->device()))
                d->m_info->load(doc);
        }
    }

    d->m_dlg = new KoDocumentInfoDlg(props, d->m_info);
    d->m_dlg->setReadOnly(true);
    // "Steal" the pages from the document info dialog
    Q_FOREACH (KPageWidgetItem *page, d->m_dlg->pages()) {
        KPageWidgetItem *myPage = new KPageWidgetItem(page->widget(), page->header());
        myPage->setIcon(page->icon());
        props->addPage(myPage);
    }
}

KoDocumentInfoPropsPage::~KoDocumentInfoPropsPage()
{
    delete d->m_info;
    delete d->m_src;
    delete d->m_dst;
    delete d->m_dlg;
    delete d;
}

void KoDocumentInfoPropsPage::applyChanges()
{
    // Unused in Calligra
}
