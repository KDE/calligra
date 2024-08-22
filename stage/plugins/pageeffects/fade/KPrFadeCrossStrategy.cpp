/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrFadeCrossStrategy.h"
#include "KPrFadeEffectFactory.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPainter>
#include <QWidget>

KPrFadeCrossStrategy::KPrFadeCrossStrategy()
    : KPrPageEffectStrategy(KPrFadeEffectFactory::CrossFade, "fade", "crossfade", false, true)
{
}

KPrFadeCrossStrategy::~KPrFadeCrossStrategy() = default;

void KPrFadeCrossStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, 1000); // TODO might not be needed
    data.m_oldPageItem->setZValue(1);
    data.m_newPageItem->setZValue(2);
    data.m_newPageItem->setOpacity(0);
    data.m_oldPageItem->show();
    data.m_newPageItem->show();
}

void KPrFadeCrossStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    Q_UNUSED(p);
    Q_UNUSED(currPos);
    Q_UNUSED(data);
}

void KPrFadeCrossStrategy::next(const KPrPageEffect::Data &data)
{
    int frame = data.m_timeLine.frameForTime(data.m_currentTime);
    if (frame >= data.m_timeLine.endFrame()) {
        finish(data);
    } else {
        qreal value = data.m_timeLine.valueForTime(data.m_currentTime);
        data.m_newPageItem->setOpacity(value);
    }
}

void KPrFadeCrossStrategy::finish(const KPrPageEffect::Data &data)
{
    data.m_graphicsView->hide();
}
