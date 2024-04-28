/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAODFPAGESAVEHELPER_H
#define KOPAODFPAGESAVEHELPER_H

#include <KoDragOdfSaveHelper.h>

#include <QList>

#include "kopageapp_export.h"

class KoPADocument;
class KoPAPageBase;

class KOPAGEAPP_TEST_EXPORT KoPAOdfPageSaveHelper : public KoDragOdfSaveHelper
{
public:
    /**
     * Use only one type of pages e.g. only master pages or only normal pages
     * if you mix the master pages will only be saved if they are needed for a normal page.
     */
    KoPAOdfPageSaveHelper(KoPADocument *doc, QList<KoPAPageBase *> pages);
    ~KoPAOdfPageSaveHelper() override;

    /// reimplemented
    KoShapeSavingContext *context(KoXmlWriter *bodyWriter, KoGenStyles &mainStyles, KoEmbeddedDocumentSaver &embeddedSaver) override;

    /// reimplemented
    bool writeBody() override;

private:
    KoPADocument *m_doc;
    KoShapeSavingContext *m_context;
    QList<KoPAPageBase *> m_pages;
    QList<KoPAPageBase *> m_masterPages;
};

#endif /* KOPAODFPAGESAVEHELPER_H */
