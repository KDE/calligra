/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSlideWipeToLeftStrategy.h"
#include "KPrSlideWipeEffectFactory.h"

#include <QPainter>
#include <QWidget>

KPrSlideWipeToLeftStrategy::KPrSlideWipeToLeftStrategy()
    : KPrPageEffectStrategy(KPrSlideWipeEffectFactory::ToLeft, "slideWipe", "fromRight", true)
{
}

KPrSlideWipeToLeftStrategy::~KPrSlideWipeToLeftStrategy() = default;

void KPrSlideWipeToLeftStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, data.m_widget->width());
}

void KPrSlideWipeToLeftStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int height = data.m_widget->height();
    int width = data.m_widget->width();
    QRect rect1(currPos, 0, width - currPos, height);
    QRect rect2(width - currPos, 0, currPos, height);
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, rect1);
    p.drawPixmap(QPoint(width - currPos, 0), data.m_newPage, rect2);
}

void KPrSlideWipeToLeftStrategy::next(const KPrPageEffect::Data &data)
{
    int lastPos = data.m_timeLine.frameForTime(data.m_lastTime);
    data.m_widget->update(0, 0, data.m_widget->width() - lastPos, data.m_widget->height());
}
