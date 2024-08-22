/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Timothe Lacroix <dakeyras.khan@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrBarWipeFromLeftStrategy.h"
#include "KPrBarWipeEffectFactory.h"

#include <QPainter>
#include <QWidget>

KPrBarWipeFromLeftStrategy::KPrBarWipeFromLeftStrategy()
    : KPrPageEffectStrategy(KPrBarWipeEffectFactory::FromLeft, "barWipe", "leftToRight", false)
{
}

KPrBarWipeFromLeftStrategy::~KPrBarWipeFromLeftStrategy() = default;

void KPrBarWipeFromLeftStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, data.m_widget->width());
}

void KPrBarWipeFromLeftStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int height = data.m_widget->height();
    int width = data.m_widget->width();
    QRect rect1(0, 0, currPos, height);
    QRect rect2(currPos, 0, width, height);
    p.drawPixmap(QPoint(0, 0), data.m_newPage, rect1);
    p.drawPixmap(QPoint(currPos, 0), data.m_oldPage, rect2);
}

void KPrBarWipeFromLeftStrategy::next(const KPrPageEffect::Data &data)
{
    int lastPos = data.m_timeLine.frameForTime(data.m_lastTime);
    int currPos = data.m_timeLine.frameForTime(data.m_currentTime);
    data.m_widget->update(lastPos, 0, currPos - lastPos, data.m_widget->height());
}
