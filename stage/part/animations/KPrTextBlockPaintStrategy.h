/* This file is part of the KDE project
 * Copyright (C) 2010 Casper Boemann <cbo@boemann.dk>
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

#ifndef KPRTEXTBLOCKPAINTSTRATEGY_H
#define KPRTEXTBLOCKPAINTSTRATEGY_H

#include <KoTextBlockPaintStrategyBase.h>

class KPrAnimationCache;
class KoTextBlockData;

/**
 * This class is used to control aspects of textblock painting
 */
class KPrTextBlockPaintStrategy : public KoTextBlockPaintStrategyBase
{
public:
    KPrTextBlockPaintStrategy(KoTextBlockData *blockData, KPrAnimationCache *animationCache);
    virtual ~KPrTextBlockPaintStrategy();
    void setAnimationCache(KPrAnimationCache *animationCache);
    virtual QBrush background(const QBrush &defaultBackground);
    virtual void applyStrategy(QPainter *painter);
    virtual bool isVisible();

    KPrAnimationCache *m_animationCache;
    KoTextBlockData *m_textBlockData;
};

#endif
