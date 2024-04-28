/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2000-2005 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoDocumentBase.h"

#include "KoOdfLoadingContext.h"
#include "OdfDebug.h"

#include <QtGlobal>

class Q_DECL_HIDDEN KoDocumentBase::Private
{
public:
    Private()
        : storeInternal(false)
        , embeddedDocument(nullptr)
    {
    }

    bool storeInternal; // Store this doc internally even if url is external
    KoDocumentBase *embeddedDocument;
};

KoDocumentBase::KoDocumentBase()
    : d(new Private)
{
}

KoDocumentBase::~KoDocumentBase()
{
    delete d;
}

void KoDocumentBase::setEmbeddedDocument(KoDocumentBase *doc)
{
    d->embeddedDocument = doc;
}

KoDocumentBase *KoDocumentBase::embeddedDocument() const
{
    return d->embeddedDocument;
}

void KoDocumentBase::setStoreInternal(bool i)
{
    d->storeInternal = i;
    // debugMain<<"="<<d->storeInternal<<" doc:"<<url().url();
}

bool KoDocumentBase::storeInternal() const
{
    return d->storeInternal;
}
