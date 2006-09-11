/* This file is part of the KDE project
 * Copyright (C) 2001 David Faure <faure@kde.org>
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

#ifndef KWVIEWMODENORMAL_H
#define KWVIEWMODENORMAL_H

#include "KWViewMode.h"

#include <QString>
#include <QList>

/**
 * This is one viewmode strategy used by the canvas to map the internal page layout to
 * one that users want to see.
 * The document-internal structure places one page below another, with page-padding in
 * in between. This strategy is mirrored
 * in this viewmode.  Only when there is at least one PageSpread kind of page will the
 * behavior change slightly.  A pagespread type page is what the user will perceive as
 * 2 pages, but KWord internally sees as one big page with a special type.  Since it is
 * about twice as wide as normal pages this viewmode will try to layout the rest of
 * the pages side by side.
 * @see KWViewMode
 */
class KWViewModeNormal : public KWViewMode {
public:
    ~KWViewModeNormal() {}

    QPointF documentToView( const QPointF & point );
    QPointF viewToDocument( const QPointF & point );
    QSize contentsSize();

    /// return a string identification of this viewMode
    static const QString viewMode() { return "ModeNormal"; }
    const QString type() const { return KWViewModeNormal::viewMode(); }
    QList<ViewMap> clipRectToDocument(const QRect &viewRect) const;

protected:
    friend class KWViewMode; // allow it to instantiate us.
    /**
     * Constructor; please use KWViewMode::create()
     */
    KWViewModeNormal( KWCanvas* canvas );

    void updatePageCache();

private:
    // a list with the top of the page, in unzoomed-pt.
    QList<double> m_pageTops;
    bool m_pageSpreadMode;
    QSizeF m_contents;
};

#endif
