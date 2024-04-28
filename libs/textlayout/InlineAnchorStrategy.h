/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Matus Hanzes <matus.hanzes@ixonos.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef INLINEANCHORSTRATEGY_H_
#define INLINEANCHORSTRATEGY_H_

#include "AnchorStrategy.h"

class KoTextLayoutRootArea;
class KoTextShapeData;
class QTextBlock;
class QTextLayout;
class KoAnchorInlineObject;

class InlineAnchorStrategy : public AnchorStrategy
{
public:
    InlineAnchorStrategy(KoAnchorInlineObject *anchor, KoTextLayoutRootArea *rootArea);
    ~InlineAnchorStrategy() override;

    bool moveSubject() override;

private:
    inline bool countHorizontalPos(QPointF &newPosition, QTextBlock &block, QTextLayout *layout);
    inline bool countVerticalPos(QPointF &newPosition, KoTextShapeData *data, QTextBlock &block, QTextLayout *layout);

    // check the border of the parent shape an move the shape back to have it inside the parent shape
    inline void checkParentBorder(QPointF &newPosition);
    KoAnchorInlineObject *m_anchorObject;
};

#endif /* INLINEANCHORSTRATEGY_H_ */
