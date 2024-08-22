/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Timothe Lacroix <dakeyras.khan@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrBarWipeFromTopStrategy.h"
#include "KPrBarWipeEffectFactory.h"

#include <QPainter>
#include <QWidget>

KPrBarWipeFromTopStrategy::KPrBarWipeFromTopStrategy()
    : KPrPageEffectStrategy(KPrBarWipeEffectFactory::FromTop, "barWipe", "topToBottom", false)
{
}

KPrBarWipeFromTopStrategy::~KPrBarWipeFromTopStrategy() = default;

void KPrBarWipeFromTopStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, data.m_widget->height());
}

void KPrBarWipeFromTopStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect1(0, 0, width, currPos);
    QRect rect2(0, currPos, width, height);
    p.drawPixmap(QPoint(0, 0), data.m_newPage, rect1);
    p.drawPixmap(QPoint(0, currPos), data.m_oldPage, rect2);
}

void KPrBarWipeFromTopStrategy::next(const KPrPageEffect::Data &data)
{
    int lastPos = data.m_timeLine.frameForTime(data.m_lastTime);
    int currPos = data.m_timeLine.frameForTime(data.m_currentTime);
    data.m_widget->update(0, lastPos, data.m_widget->width(), currPos - lastPos);
}
