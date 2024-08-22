/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrCenterFanWipeStrategy.h"

#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <math.h>

#include "KPrClockWipeSubpathHelper.h"

KPrCenterFanWipeStrategy::KPrCenterFanWipeStrategy(int startAngle, int fanCount, int subType, const char *smilType, const char *smilSubType, bool reverse)
    : KPrPageEffectStrategy(subType, smilType, smilSubType, reverse)
    , m_fanCount(fanCount)
{
    m_startAngle = static_cast<double>(startAngle) / 180 * M_PI;
}

KPrCenterFanWipeStrategy::~KPrCenterFanWipeStrategy() = default;

void KPrCenterFanWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, 180);
}

void KPrCenterFanWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect(0, 0, width, height);
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, rect);

    QPainterPath clipPath;
    for (int i = 0; i < m_fanCount; i++) {
        double fanStartAngle = m_startAngle + (2 * M_PI) / m_fanCount * i;

        double angle = static_cast<double>(currPos) / 180 * M_PI / m_fanCount;

        double startAngle = fanStartAngle - angle;
        double endAngle = fanStartAngle + angle;

        KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, rect, startAngle, endAngle);
    }
    p.setClipPath(clipPath);

    p.drawPixmap(QPoint(0, 0), data.m_newPage, rect);
}

void KPrCenterFanWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}
