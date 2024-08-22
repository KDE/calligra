/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrTextBlockPaintStrategy.h"

#include <QBrush>
#include <QPainter>
#include <QTransform>

#include "KoTextBlockData.h"

#include "KPrAnimationCache.h"
#include "StageDebug.h"

KPrTextBlockPaintStrategy::KPrTextBlockPaintStrategy(QTextBlockUserData *blockUserData, KPrAnimationCache *animationCache)
    : m_animationCache(animationCache)
    , m_textBlockData(blockUserData)
{
}

KPrTextBlockPaintStrategy::~KPrTextBlockPaintStrategy() = default;

void KPrTextBlockPaintStrategy::setAnimationCache(KPrAnimationCache *animationCache)
{
    m_animationCache = animationCache;
}

QBrush KPrTextBlockPaintStrategy::background(const QBrush &defaultBackground) const
{
    return defaultBackground;
}

void KPrTextBlockPaintStrategy::applyStrategy(QPainter *painter)
{
    QTransform animationTransform = m_animationCache->value(m_textBlockData, "transform", QTransform()).value<QTransform>();
    QTransform transform(painter->transform());
    if (animationTransform.isScaling()) {
        transform = animationTransform * transform;
    } else {
        transform = transform * animationTransform;
    }
    painter->setTransform(transform);
    painter->setClipping(false);
}

bool KPrTextBlockPaintStrategy::isVisible() const
{
    if (m_animationCache) {
        return m_animationCache->value(m_textBlockData, "visibility", true).toBool();
    } else {
        return true;
    }
}
