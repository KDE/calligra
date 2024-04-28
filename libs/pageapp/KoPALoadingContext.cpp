/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2007-2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPALoadingContext.h"

#include <QMap>

#include "KoPAMasterPage.h"
#include "KoPAPage.h"

class Q_DECL_HIDDEN KoPALoadingContext::Private
{
public:
    QMap<QString, KoPAMasterPage *> masterPages;
    QMap<QString, KoPAPage *> pages;
};

KoPALoadingContext::KoPALoadingContext(KoOdfLoadingContext &context, KoDocumentResourceManager *documentResources)
    : KoShapeLoadingContext(context, documentResources)
    , d(new Private())
{
}

KoPALoadingContext::~KoPALoadingContext()
{
    delete d;
}

KoPAMasterPage *KoPALoadingContext::masterPageByName(const QString &name)
{
    return d->masterPages.value(name, 0);
}

void KoPALoadingContext::addMasterPage(const QString &name, KoPAMasterPage *master)
{
    d->masterPages.insert(name, master);
}

const QMap<QString, KoPAMasterPage *> &KoPALoadingContext::masterPages()
{
    return d->masterPages;
}

KoPAPage *KoPALoadingContext::pageByName(const QString &name)
{
    return d->pages.value(name, 0);
}

void KoPALoadingContext::addPage(const QString &name, KoPAPage *page)
{
    d->pages.insert(name, page);
}
