/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrClockWipeStrategy.h"

#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <math.h>

#include "KPrClockWipeSubpathHelper.h"

KPrClockWipeStrategy::KPrClockWipeStrategy(int startAngle, int bladeCount, int subType, const char *smilType, const char *smilSubType, bool reverse)
    : KPrPageEffectStrategy(subType, smilType, smilSubType, reverse)
    , m_bladeCount(bladeCount)
{
    m_startAngle = static_cast<double>(startAngle) / 180 * M_PI;
}

KPrClockWipeStrategy::~KPrClockWipeStrategy() = default;

void KPrClockWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, 360);
}

void KPrClockWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    QRect rect(0, 0, data.m_widget->width(), data.m_widget->height());
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, rect);

    QPainterPath clipPath;
    for (int i = 0; i < m_bladeCount; i++) {
        double bladeStartAngle;
        double bladeEndAngle;

        double angle = static_cast<double>(currPos) / m_bladeCount / 180 * M_PI;

        if (!reverse()) {
            bladeEndAngle = 2 * M_PI / m_bladeCount * i + m_startAngle;
            bladeStartAngle = bladeEndAngle - angle;
        } else {
            bladeStartAngle = 2 * M_PI / m_bladeCount * (i + 1) + m_startAngle;
            bladeEndAngle = bladeStartAngle + angle;
        }
        KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, rect, bladeStartAngle, bladeEndAngle);
    }
    p.setClipPath(clipPath);

    p.drawPixmap(QPoint(0, 0), data.m_newPage, rect);
}

void KPrClockWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}
