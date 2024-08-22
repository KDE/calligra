/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Timothée Lacroix <dakeyras.khan@gmail.com>
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrBarWipeFromBottomStrategy.h"
#include "KPrBarWipeEffectFactory.h"

#include <QPainter>
#include <QWidget>

KPrBarWipeFromBottomStrategy::KPrBarWipeFromBottomStrategy()
    : KPrPageEffectStrategy(KPrBarWipeEffectFactory::FromBottom, "barWipe", "topToBottom", true)
{
}

KPrBarWipeFromBottomStrategy::~KPrBarWipeFromBottomStrategy() = default;

void KPrBarWipeFromBottomStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, data.m_widget->height());
}

void KPrBarWipeFromBottomStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect1(0, 0, width, height - currPos);
    QRect rect2(0, height - currPos, width, currPos);
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, rect1);
    p.drawPixmap(QPoint(0, height - currPos), data.m_newPage, rect2);
}

void KPrBarWipeFromBottomStrategy::next(const KPrPageEffect::Data &data)
{
    int lastPos = data.m_timeLine.frameForTime(data.m_lastTime);
    int currPos = data.m_timeLine.frameForTime(data.m_currentTime);
    data.m_widget->update(0, data.m_widget->height() - currPos, data.m_widget->width(), currPos - lastPos);
}
