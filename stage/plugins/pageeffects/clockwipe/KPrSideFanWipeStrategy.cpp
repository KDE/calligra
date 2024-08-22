/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSideFanWipeStrategy.h"

#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <math.h>

#include "KPrClockWipeSubpathHelper.h"

KPrSideFanWipeStrategy::KPrSideFanWipeStrategy(int positionAngle, int fanCount, int subType, const char *smilType, const char *smilSubType, bool reverse)
    : KPrPageEffectStrategy(subType, smilType, smilSubType, reverse)
    , m_fanCount(fanCount)
{
    m_positionAngle = static_cast<double>(positionAngle) / 180 * M_PI;
    m_startAngle = static_cast<double>(positionAngle) / 180 * M_PI + M_PI;
}

KPrSideFanWipeStrategy::~KPrSideFanWipeStrategy() = default;

void KPrSideFanWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, 180);
}

void KPrSideFanWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect(0, 0, width, height);
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, rect);

    QPoint center(width / 2, height / 2);

    for (int i = 0; i < m_fanCount; i++) {
        double fanAnglePositionAngle = m_positionAngle + 2 * M_PI / m_fanCount * i;
        QPoint rotationCenter(center.x() + width / 2 * qRound(cos(fanAnglePositionAngle)), center.y() - height / 2 * qRound(sin(fanAnglePositionAngle)));

        double fanAngle = fanAnglePositionAngle + M_PI;
        double angle = static_cast<double>(currPos) / 90 * 0.25 * M_PI;

        QRect boundingRect;
        if (m_fanCount > 1) {
            boundingRect = rect;
            boundingRect.translate(rotationCenter.x() - center.x(), rotationCenter.y() - center.y());
        } else {
            boundingRect = QRect(0, 0, width + 2 * abs(rotationCenter.x() - center.x()), height + 2 * abs(rotationCenter.y() - center.y()));
            boundingRect.moveCenter(rotationCenter);
        }

        QPainterPath clipPath;
        if (reverse()) {
            KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, boundingRect, fanAngle - 0.5 * M_PI, fanAngle - 0.5 * M_PI + angle);
            KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, boundingRect, fanAngle + 0.5 * M_PI - angle, fanAngle + 0.5 * M_PI);
            p.setClipPath(clipPath);
            p.drawPixmap(rect.intersected(boundingRect), data.m_newPage, rect.intersected(boundingRect));
        } else {
            KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, boundingRect, fanAngle - angle, fanAngle + angle);
            p.setClipPath(clipPath);
            p.drawPixmap(rect.intersected(boundingRect), data.m_newPage, rect.intersected(boundingRect));
        }
    }
}

void KPrSideFanWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}
