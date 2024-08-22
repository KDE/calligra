/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSweepWipeStrategy.h"

#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <math.h>

#include "KPrClockWipeSubpathHelper.h"

KPrSweepWipeStrategy::KPrSweepWipeStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse)
    : KPrPageEffectStrategy(subType, smilType, smilSubType, reverse)
{
}

KPrSweepWipeStrategy::~KPrSweepWipeStrategy() = default;

void KPrSweepWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}

void KPrSweepWipeStrategy::drawSweep(QPainter &p, double angle, double rotationRange, QRect boundingRect, const KPrPageEffect::Data &data)
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect(0, 0, width, height);

    double startAngle;
    double endAngle;

    if (rotationRange > 0) {
        startAngle = angle;
        endAngle = startAngle + rotationRange;
    } else {
        endAngle = angle;
        startAngle = endAngle + rotationRange;
    }

    QPainterPath clipPath;
    KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, boundingRect, startAngle, endAngle);
    p.setClipPath(clipPath);

    p.drawPixmap(rect.intersected(boundingRect), data.m_newPage, rect.intersected(boundingRect));
}
