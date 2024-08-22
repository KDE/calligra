/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSlideWipeToBottomStrategy.h"
#include "KPrSlideWipeEffectFactory.h"

#include <QPainter>
#include <QWidget>

KPrSlideWipeToBottomStrategy::KPrSlideWipeToBottomStrategy()
    : KPrPageEffectStrategy(KPrSlideWipeEffectFactory::ToBottom, "slideWipe", "fromTop", true)
{
}

KPrSlideWipeToBottomStrategy::~KPrSlideWipeToBottomStrategy() = default;

void KPrSlideWipeToBottomStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, data.m_widget->height());
}

void KPrSlideWipeToBottomStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int height = data.m_widget->height();
    int width = data.m_widget->width();
    QRect rect1(0, 0, width, height - currPos);
    QRect rect2(0, 0, width, currPos);
    p.drawPixmap(QPoint(0, currPos), data.m_oldPage, rect1);
    p.drawPixmap(QPoint(0, 0), data.m_newPage, rect2);
}

void KPrSlideWipeToBottomStrategy::next(const KPrPageEffect::Data &data)
{
    int lastPos = data.m_timeLine.frameForTime(data.m_lastTime);
    data.m_widget->update(0, lastPos, data.m_widget->width(), data.m_widget->height() - lastPos);
}
