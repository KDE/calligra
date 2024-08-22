/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Timothée Lacroix <dakeyras.khan@gmail.com>
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrBarWipeFromRightStrategy.h"
#include "KPrBarWipeEffectFactory.h"

#include <QPainter>
#include <QWidget>

KPrBarWipeFromRightStrategy::KPrBarWipeFromRightStrategy()
    : KPrPageEffectStrategy(KPrBarWipeEffectFactory::FromRight, "barWipe", "leftToRight", true)
{
}

KPrBarWipeFromRightStrategy::~KPrBarWipeFromRightStrategy() = default;

void KPrBarWipeFromRightStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, data.m_widget->width());
}

void KPrBarWipeFromRightStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int height = data.m_widget->height();
    int width = data.m_widget->width();
    QRect rect1(0, 0, width - currPos, height);
    QRect rect2(width - currPos, 0, currPos, height);
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, rect1);
    p.drawPixmap(QPoint(width - currPos, 0), data.m_newPage, rect2);
}

void KPrBarWipeFromRightStrategy::next(const KPrPageEffect::Data &data)
{
    int lastPos = data.m_timeLine.frameForTime(data.m_lastTime);
    int currPos = data.m_timeLine.frameForTime(data.m_currentTime);
    data.m_widget->update(data.m_widget->width() - currPos, 0, currPos - lastPos, data.m_widget->height());
}
