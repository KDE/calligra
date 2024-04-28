/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2002, 2003 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2003 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAGELAYOUT_H
#define KOPAGELAYOUT_H

#include "KoBorder.h"
#include "KoPageFormat.h"
#include "KoXmlReaderForward.h"

#include "koodf_export.h"

class KoGenStyle;

/**
 * This structure defines the page layout, including
 * its size in points, its format (e.g. A4), orientation, unit, margins etc.
 */
struct KoPageLayout {
    /** Page format */
    KoPageFormat::Format format;
    /** Page orientation */
    KoPageFormat::Orientation orientation;

    /** Page width in points */
    qreal width;
    /** Page height in points */
    qreal height;

    /**
     * margin on left edge
     * Using a margin >= 0 here indicates this is a single page system and the
     *  bindingSide + pageEdge variables should be -1 at the same time.
     */
    qreal leftMargin;
    /**
     * margin on right edge
     * Using a margin >= 0 here indicates this is a single page system and the
     *  bindingSide + pageEdge variables should be -1 at the same time.
     */
    qreal rightMargin;
    /** Top margin in points */
    qreal topMargin;
    /** Bottom margin in points */
    qreal bottomMargin;

    /**
     * margin on page edge
     * Using a margin >= 0 here indicates this is a pageSpread (facing pages) and the
     *  left + right variables should be -1 at the same time.
     */
    qreal pageEdge;
    /**
     * margin on page-binding edge
     * Using a margin >= 0 here indicates this is a pageSpread (facing pages) and the
     *  left + right variables should be -1 at the same time.
     */
    qreal bindingSide;

    /** Left padding in points */
    qreal leftPadding;
    /** Right padding in points */
    qreal rightPadding;
    /** Top padding in points */
    qreal topPadding;
    /** Bottom padding in points */
    qreal bottomPadding;

    /// page border definition
    KoBorder border;

    KOODF_EXPORT bool operator==(const KoPageLayout &l) const;
    KOODF_EXPORT bool operator!=(const KoPageLayout &l) const;

    /**
     * Save this page layout to ODF.
     */
    KOODF_EXPORT KoGenStyle saveOdf() const;

    /**
     * Load this page layout from ODF
     */
    KOODF_EXPORT void loadOdf(const KoXmlElement &style);

    /**
     * Construct a page layout with the default page size depending on the locale settings,
     * default margins (2 cm), and portrait orientation.
     */
    KOODF_EXPORT KoPageLayout();
};

#endif /* KOPAGELAYOUT_H */
