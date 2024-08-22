/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSlideWipeFromLeftStrategy.h"
#include "KPrSlideWipeEffectFactory.h"

#include <QPainter>
#include <QWidget>

KPrSlideWipeFromLeftStrategy::KPrSlideWipeFromLeftStrategy()
    : KPrPageEffectStrategy(KPrSlideWipeEffectFactory::FromLeft, "slideWipe", "fromLeft", false)
{
}

KPrSlideWipeFromLeftStrategy::~KPrSlideWipeFromLeftStrategy() = default;

void KPrSlideWipeFromLeftStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, data.m_widget->width());
}

void KPrSlideWipeFromLeftStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int height = data.m_widget->height();
    int width = data.m_widget->width();
    QRect rect1(currPos, 0, width - currPos, height);
    QRect rect2(width - currPos, 0, currPos, height);
    p.drawPixmap(QPoint(currPos, 0), data.m_oldPage, rect1);
    p.drawPixmap(QPoint(0, 0), data.m_newPage, rect2);
}

void KPrSlideWipeFromLeftStrategy::next(const KPrPageEffect::Data &data)
{
    int currPos = data.m_timeLine.frameForTime(data.m_currentTime);
    data.m_widget->update(0, 0, currPos, data.m_widget->height());
}
