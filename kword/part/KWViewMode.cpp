/* This file is part of the KDE project
 * Copyright (C) 2001 David Faure <faure@kde.org>
 * Copyright (C) 2005-2006 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWViewMode.h"

#include <KoCanvasBase.h>

#include "KWDocument.h"
#include "KWViewModeNormal.h"
#include "KWViewModePreview.h"

KWViewMode::KWViewMode()
    : m_pageManager(0),
    m_viewConverter(0),
    m_drawFrameBorders(false)
{
}

QRectF KWViewMode::documentToView(const QRectF &rect) const
{
    QRectF r;
    QPointF topLeft(documentToView(rect.topLeft()));
    QPointF bottomRight(documentToView(rect.bottomRight()));

    r.setCoords(topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());
    return r;
}

QRectF KWViewMode::viewToDocument(const QRectF &rect) const
{
    QRectF r;
    QPointF topLeft(viewToDocument(rect.topLeft()));
    QPointF bottomRight(viewToDocument(rect.bottomRight()));

    r.setCoords(topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());
    return r;
}

void KWViewMode::pageSetupChanged()
{
    updatePageCache();
}

// static
KWViewMode *KWViewMode::create(const QString &viewModeType, KWDocument *document, KoCanvasBase *canvas)
{
    KWViewMode * vm = 0;
    if (viewModeType == KWViewModePreview::viewMode())
        vm = new KWViewModePreview();
    if (vm == 0)
        vm = new KWViewModeNormal();

    vm->setPageManager(document->pageManager());
    vm->setViewConverter(canvas->viewConverter());
    return vm;
}

