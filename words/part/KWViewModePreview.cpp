/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include "KWViewModePreview.h"

// #include "KWView.h"
// #include "KWDocument.h"
// #include "KWTextFrameSet.h"
// #include "KWFrameSet.h"
// #include "KWTableFrameSet.h"
// #include "KWPageManager.h"
// #include "KWPage.h"
// #include "KWFrameViewManager.h"
// #include "KWFrameView.h"
// #include "KWGUI.h"


KWViewModePreview::KWViewModePreview()
        : m_pagesPerRow(3)
{
}

QSizeF KWViewModePreview::contentsSize() const
{
    return QSizeF();
}

QPointF KWViewModePreview::documentToView(const QPointF & point, KoViewConverter *viewConverter) const
{
    Q_UNUSED(point);
    Q_UNUSED(viewConverter);
    return QPointF();
}

QPointF KWViewModePreview::viewToDocument(const QPointF & point, KoViewConverter *viewConverter) const
{
    Q_UNUSED(point);
    Q_UNUSED(viewConverter);
    return QPointF();
}

void KWViewModePreview::updatePageCache()
{
    // TODO
}

QVector<KWViewMode::ViewMap> KWViewModePreview::mapExposedRects(const QRectF &viewRect, KoViewConverter *viewConverter) const
{
    Q_UNUSED(viewRect);
    Q_UNUSED(viewConverter);
    return QVector<KWViewMode::ViewMap>();
}

void KWViewModePreview::setGap(int gap)
{
    Q_UNUSED(gap);
    // TODO
}
