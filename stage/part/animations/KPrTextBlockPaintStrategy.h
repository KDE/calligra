/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRTEXTBLOCKPAINTSTRATEGY_H
#define KPRTEXTBLOCKPAINTSTRATEGY_H

#include <KoTextBlockPaintStrategyBase.h>

class KPrAnimationCache;
class QTextBlockUserData;

/**
 * This class is used to control aspects of textblock painting
 */
class KPrTextBlockPaintStrategy : public KoTextBlockPaintStrategyBase
{
public:
    KPrTextBlockPaintStrategy(QTextBlockUserData *blockUserData, KPrAnimationCache *animationCache);
    ~KPrTextBlockPaintStrategy() override;
    void setAnimationCache(KPrAnimationCache *animationCache);
    QBrush background(const QBrush &defaultBackground) const override;
    void applyStrategy(QPainter *painter) override;
    bool isVisible() const override;

    KPrAnimationCache *m_animationCache;
    QTextBlockUserData *m_textBlockData;
};

#endif
