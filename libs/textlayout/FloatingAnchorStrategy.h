/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Matus Hanzes <matus.hanzes@ixonos.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef FLOATINGANCHORSTRATEGY_H
#define FLOATINGANCHORSTRATEGY_H

#include "AnchorStrategy.h"

class KoTextLayoutRootArea;
class KoTextShapeData;
class QTextBlock;
class QTextLayout;
class KoTextLayoutObstruction;
class KoAnchorTextRange;

class FloatingAnchorStrategy : public AnchorStrategy
{
public:
    FloatingAnchorStrategy(KoAnchorTextRange *anchor, KoTextLayoutRootArea *rootArea);
    ~FloatingAnchorStrategy() override;

    /**
     * This moves the subject (i.e. shape when used with flake) of the anchor.
     *
     * @return true if subject was moved
     */
    bool moveSubject() override;

private:
    inline bool countHorizontalRel(QRectF &anchorBoundingRect, const QRectF &containerBoundingRect, QTextBlock &block, QTextLayout *layout);
    inline void countHorizontalPos(QPointF &newPosition, const QRectF &anchorBoundingRect);
    inline bool
    countVerticalRel(QRectF &anchorBoundingRect, const QRectF &containerBoundingRect, KoTextShapeData *data, QTextBlock &block, QTextLayout *layout);
    inline void countVerticalPos(QPointF &newPosition, const QRectF &anchorBoundingRect);

    // check the layout environment and move the shape back to have it within
    inline void checkLayoutEnvironment(QPointF &newPosition, KoTextShapeData *data);
    // check the border of page and move the shape back to have it visible
    inline void checkPageBorder(QPointF &newPosition);
    // check stacking and reorder to proper position objects according to there z-index
    inline void checkStacking(QPointF &newPosition);

    void updateObstruction(qreal documentOffset);

    KoTextLayoutObstruction *m_obstruction; // the obstruction representation of the subject
    KoAnchorTextRange *m_anchorRange;
};

#endif // FLOATINGANCHORSTRATEGY_H
