/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoDragOdfSaveHelper.h"
#include "KoDragOdfSaveHelper_p.h"

KoDragOdfSaveHelper::KoDragOdfSaveHelper()
    : d_ptr(new KoDragOdfSaveHelperPrivate())
{
}

KoDragOdfSaveHelper::KoDragOdfSaveHelper(KoDragOdfSaveHelperPrivate &dd)
    : d_ptr(&dd)
{
}

KoDragOdfSaveHelper::~KoDragOdfSaveHelper()
{
    delete d_ptr;
}

KoShapeSavingContext *KoDragOdfSaveHelper::context(KoXmlWriter *bodyWriter, KoGenStyles &mainStyles, KoEmbeddedDocumentSaver &embeddedSaver)
{
    Q_D(KoDragOdfSaveHelper);
    Q_ASSERT(d->context == nullptr);
    d->context = new KoShapeSavingContext(*bodyWriter, mainStyles, embeddedSaver);
    return d->context;
}
