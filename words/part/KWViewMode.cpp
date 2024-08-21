/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010-2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWViewMode.h"

#include <KoViewConverter.h>

#include "KWDocument.h"
#include "KWViewModeNormal.h"
#include "KWViewModePreview.h"

KWViewMode::KWViewMode()
    : m_pageManager(nullptr)
{
}

QRectF KWViewMode::documentToView(const QRectF &rect, KoViewConverter *viewConverter) const
{
    QRectF r;
    QPointF topLeft(documentToView(rect.topLeft(), viewConverter));
    QPointF bottomRight(documentToView(rect.bottomRight(), viewConverter));

    r.setCoords(topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());
    return r;
}

QRectF KWViewMode::viewToDocument(const QRectF &rect, KoViewConverter *viewConverter) const
{
    QRectF r;
    QPointF topLeft(viewToDocument(rect.topLeft(), viewConverter));
    QPointF bottomRight(viewToDocument(rect.bottomRight(), viewConverter));

    r.setCoords(topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());
    return r;
}

void KWViewMode::pageSetupChanged()
{
    updatePageCache();
}

// static
KWViewMode *KWViewMode::create(const QString &viewModeType, KWDocument *document)
{
    KWViewMode *vm = nullptr;
    if (viewModeType == KWViewModePreview::viewMode())
        vm = new KWViewModePreview();
    if (vm == nullptr)
        vm = new KWViewModeNormal();

    vm->setPageManager(document->pageManager());
    return vm;
}
