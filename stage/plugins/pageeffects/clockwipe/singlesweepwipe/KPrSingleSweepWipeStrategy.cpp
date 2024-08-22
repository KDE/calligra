/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSingleSweepWipeStrategy.h"

#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <math.h>

#include "KPrClockWipeSubpathHelper.h"

KPrSingleSweepWipeStrategy::KPrSingleSweepWipeStrategy(double rotationX,
                                                       double rotationY,
                                                       int startAngle,
                                                       int rotationAngle,
                                                       int subType,
                                                       const char *smilType,
                                                       const char *smilSubType,
                                                       bool reverse)
    : KPrPageEffectStrategy(subType, smilType, smilSubType, reverse)
    , m_rotationX(rotationX)
    , m_rotationY(rotationY)
{
    m_startAngle = static_cast<double>(startAngle) / 180 * M_PI;
    m_rotationAngle = static_cast<double>(rotationAngle) / 180 * M_PI;
}

KPrSingleSweepWipeStrategy::~KPrSingleSweepWipeStrategy() = default;

void KPrSingleSweepWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, 180);
}

void KPrSingleSweepWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect(0, 0, width, height);
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, rect);

    QPoint center(width / 2, height / 2);
    QPoint rotationCenter(width * m_rotationX, height * m_rotationY);

    double startAngle;
    double endAngle;

    if (m_rotationAngle > 0) {
        startAngle = m_startAngle;
        endAngle = startAngle + m_rotationAngle * currPos / 180;
    } else {
        endAngle = m_startAngle;
        startAngle = endAngle + m_rotationAngle * currPos / 180;
    }

    QRect boundingRect(0, 0, width + 2 * abs(rotationCenter.x() - center.x()), height + 2 * abs(rotationCenter.y() - center.y()));
    boundingRect.moveCenter(rotationCenter);

    QPainterPath clipPath;
    KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, boundingRect, startAngle, endAngle);
    p.setClipPath(clipPath);

    p.drawPixmap(QPoint(0, 0), data.m_newPage, rect);
}

void KPrSingleSweepWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}
