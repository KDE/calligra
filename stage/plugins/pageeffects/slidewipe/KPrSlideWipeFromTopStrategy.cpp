/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSlideWipeFromTopStrategy.h"
#include "KPrSlideWipeEffectFactory.h"

#include <QPainter>
#include <QWidget>

KPrSlideWipeFromTopStrategy::KPrSlideWipeFromTopStrategy()
    : KPrPageEffectStrategy(KPrSlideWipeEffectFactory::FromTop, "slideWipe", "fromTop", false)
{
}

KPrSlideWipeFromTopStrategy::~KPrSlideWipeFromTopStrategy() = default;

void KPrSlideWipeFromTopStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, data.m_widget->height());
}

void KPrSlideWipeFromTopStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int height = data.m_widget->height();
    int width = data.m_widget->width();
    QRect rect1(0, currPos, width, height - currPos);
    QRect rect2(0, height - currPos, width, currPos);
    p.drawPixmap(QPoint(0, currPos), data.m_oldPage, rect1);
    p.drawPixmap(QPoint(0, 0), data.m_newPage, rect2);
}

void KPrSlideWipeFromTopStrategy::next(const KPrPageEffect::Data &data)
{
    int currPos = data.m_timeLine.frameForTime(data.m_currentTime);
    data.m_widget->update(0, 0, data.m_widget->width(), currPos);
}
