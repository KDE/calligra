/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KWPAGESTYLE_P_H
#define KWPAGESTYLE_P_H

#include <KoColumns.h>
#include <KoShapeBackground.h>
#include <KoText.h>
#include <QSharedData>

class KWPageStylePrivate : public QSharedData
{
public:
    KWPageStylePrivate()
        : fullPageBackground(nullptr)
    {
        clear();
    }
    ~KWPageStylePrivate();
    void clear();

    KWPageStyle::PageUsageType pageUsage;
    KoColumns columns;
    KoPageLayout pageLayout;
    QString name;
    QString displayName;
    qreal headerDistance, footerDistance, footNoteDistance, endNoteDistance;
    qreal headerMinimumHeight, footerMinimumHeight;
    Words::HeaderFooterType headers, footers;
    bool headerDynamicSpacing;
    bool footerDynamicSpacing;
    qreal footNoteSeparatorLineWidth; ///< width of line; so more like 'thickness'
    int footNoteSeparatorLineLength; ///< It's a percentage of page.
    Qt::PenStyle footNoteSeparatorLineType; ///< foot note separate type
    Words::FootNoteSeparatorLinePos footNoteSeparatorLinePos; ///< alignment in page

    // See parag 16.2 for all the ODF features.
    KoText::Direction direction;
    QSharedPointer<KoShapeBackground> fullPageBackground;
    QString nextStyleName;

    // called from the command
    void copyProperties(KWPageStylePrivate *other)
    {
        pageUsage = other->pageUsage;
        columns = other->columns;
        pageLayout = other->pageLayout;
        // name = other->name;
        headerDistance = other->headerDistance;
        footerDistance = other->footerDistance;
        footNoteDistance = other->footNoteDistance;
        endNoteDistance = other->endNoteDistance;
        headerMinimumHeight = other->headerMinimumHeight;
        footerMinimumHeight = other->footerMinimumHeight;
        headers = other->headers;
        footers = other->footers;
        footNoteSeparatorLineWidth = other->footNoteSeparatorLineWidth;
        footNoteSeparatorLineLength = other->footNoteSeparatorLineLength;
        footNoteSeparatorLineType = other->footNoteSeparatorLineType;
        footNoteSeparatorLinePos = other->footNoteSeparatorLinePos;
        direction = other->direction;
        fullPageBackground = other->fullPageBackground;
        nextStyleName = other->nextStyleName;
    }
};

#endif
