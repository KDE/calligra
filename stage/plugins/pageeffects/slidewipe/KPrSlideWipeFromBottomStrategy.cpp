/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSlideWipeFromBottomStrategy.h"
#include "KPrSlideWipeEffectFactory.h"

#include <QPainter>
#include <QWidget>

KPrSlideWipeFromBottomStrategy::KPrSlideWipeFromBottomStrategy()
    : KPrPageEffectStrategy(KPrSlideWipeEffectFactory::FromBottom, "slideWipe", "fromBottom", false)
{
}

KPrSlideWipeFromBottomStrategy::~KPrSlideWipeFromBottomStrategy() = default;

void KPrSlideWipeFromBottomStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, data.m_widget->height());
}

void KPrSlideWipeFromBottomStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int height = data.m_widget->height();
    int width = data.m_widget->width();
    QRect rect1(0, 0, width, height - currPos);
    QRect rect2(0, 0, width, currPos);
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, rect1);
    p.drawPixmap(QPoint(0, height - currPos), data.m_newPage, rect2);
}

void KPrSlideWipeFromBottomStrategy::next(const KPrPageEffect::Data &data)
{
    int currPos = data.m_timeLine.frameForTime(data.m_currentTime);
    data.m_widget->update(0, data.m_widget->height() - currPos, data.m_widget->width(), currPos);
}
