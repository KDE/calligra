/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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

QPointF KWViewModePreview::documentToView(const QPointF &point, KoViewConverter *viewConverter) const
{
    Q_UNUSED(point);
    Q_UNUSED(viewConverter);
    return QPointF();
}

QPointF KWViewModePreview::viewToDocument(const QPointF &point, KoViewConverter *viewConverter) const
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
