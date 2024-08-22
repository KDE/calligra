/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWVIEWMODENORMAL_H
#define KWVIEWMODENORMAL_H

#include "KWViewMode.h"

#include <QList>
#include <QString>

/**
 * This is one viewmode strategy used by the canvas to map the internal page layout to
 * one that users want to see.
 * The document-internal structure places one page below another, with page-padding in
 * in between. This strategy is mirrored
 * in this viewmode.  Only when there is at least one PageSpread kind of page will the
 * behavior change slightly.  A pagespread type page is what the user will perceive as
 * 2 pages, but Words internally sees as one big page with a special type.  Since it is
 * about twice as wide as normal pages this viewmode will try to layout the rest of
 * the pages side by side.
 * @see KWViewMode
 */
class WORDS_EXPORT KWViewModeNormal : public KWViewMode
{
    Q_OBJECT
public:
    /**
     * Constructor; please use KWViewMode::create()
     */
    KWViewModeNormal();
    ~KWViewModeNormal() override = default;

    using KWViewMode::documentToView;

    QPointF documentToView(const QPointF &point, KoViewConverter *viewConverter) const override;
    QPointF viewToDocument(const QPointF &point, KoViewConverter *viewConverter) const override;
    QSizeF contentsSize() const override
    {
        return m_contents;
    }

    /// return a string identification of this viewMode
    static const QString viewMode()
    {
        return "ModeNormal";
    }
    const QString type() const override
    {
        return KWViewModeNormal::viewMode();
    }
    QVector<ViewMap> mapExposedRects(const QRectF &clipRect, KoViewConverter *viewConverter) const override;

protected:
    void updatePageCache() override;

    // a list with the top of the page location in view-coordinates, in unzoomed-pt.
    QList<qreal> m_pageTops;
    bool m_pageSpreadMode;
    QSizeF m_contents;
};

#endif
