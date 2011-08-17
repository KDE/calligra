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

#include "KPrTextBlockPaintStrategy.h"

#include <QBrush>
#include "KPrAnimationCache.h"
#include <QTransform>
#include <QMatrix>
#include <QPainter>

#include "kdebug.h"
#include "KoTextBlockData.h"
KPrTextBlockPaintStrategy::KPrTextBlockPaintStrategy(KoTextBlockData *blockData, KPrAnimationCache *animationCache)
    : m_animationCache(animationCache)
    , m_textBlockData(blockData)
{
}

KPrTextBlockPaintStrategy::~KPrTextBlockPaintStrategy()
{
}

void KPrTextBlockPaintStrategy::setAnimationCache(KPrAnimationCache *animationCache)
{
    m_animationCache = animationCache;
}

QBrush KPrTextBlockPaintStrategy::background(const QBrush &defaultBackground)
{
    return defaultBackground;
}

void KPrTextBlockPaintStrategy::applyStrategy(QPainter *painter)
{
    QTransform animationTransform = m_animationCache->value(m_textBlockData, "transform", QTransform()).value<QTransform>();
    QTransform transform(painter->matrix());
      if (animationTransform.isScaling()) {
        transform = animationTransform * transform;
    } else {
        transform = transform * animationTransform;
    }
    painter->setTransform(transform);
    painter->setClipping(false);
}

bool KPrTextBlockPaintStrategy::isVisible()
{
    if (m_animationCache) {
        return m_animationCache->value(m_textBlockData, "visibility", true).toBool();
    } else {
        return true;
    }
}
