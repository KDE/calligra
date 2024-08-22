/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BarnZigZagWipeStrategy.h"
#include "BarnZigZagWipeEffectFactory.h"
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

const int StepCount = 250;

BarnZigZagWipeStrategy::BarnZigZagWipeStrategy(int subtype, const char *smilSubType, bool reverse)
    : KPrPageEffectStrategy(subtype, "BarnZigZagWipe", smilSubType, reverse)
{
}

BarnZigZagWipeStrategy::~BarnZigZagWipeStrategy() = default;

void BarnZigZagWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, StepCount);
}

void BarnZigZagWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, data.m_widget->rect());
    p.setClipPath(clipPath(currPos, data.m_widget->rect()));
    p.drawPixmap(QPoint(0, 0), data.m_newPage, data.m_widget->rect());
}

void BarnZigZagWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}

QPainterPath BarnZigZagWipeStrategy::clipPath(int step, const QRect &area)
{
    const int zigZagCount = 10;
    const qreal zigZagHeight = area.height() / static_cast<qreal>(zigZagCount);
    const qreal zigZagWidth = area.width() / static_cast<qreal>(zigZagCount);

    qreal percent = static_cast<qreal>(step) / static_cast<qreal>(StepCount);
    if (reverse())
        percent = static_cast<qreal>(StepCount - step) / static_cast<qreal>(StepCount);

    qreal width_2 = 0.5 * area.width();
    qreal height_2 = 0.5 * area.height();

    int stepx = static_cast<int>((width_2 + 1.5 * zigZagWidth) * percent);
    int stepy = static_cast<int>((height_2 + 1.5 * zigZagHeight) * percent);

    qreal zigZagHeight_2 = 0.5 * zigZagHeight;
    qreal zigZagWidth_2 = 0.5 * zigZagWidth;

    QPainterPath path;
    switch (subType()) {
    case BarnZigZagWipeEffectFactory::Horizontal:
    case BarnZigZagWipeEffectFactory::HorizontalReversed: {
        qreal zigZagBase = height_2 - stepy - zigZagHeight_2;
        qreal zigZagTip = zigZagBase + zigZagHeight;
        path.moveTo(QPointF(area.left(), height_2));
        path.lineTo(QPointF(area.left(), zigZagBase));
        for (int i = 0; i < zigZagCount; ++i) {
            path.lineTo(QPointF((2 * i + 1) * zigZagWidth_2, zigZagTip));
            path.lineTo(QPointF((i + 1) * zigZagWidth, zigZagBase));
        }
        zigZagBase = height_2 + stepy + zigZagHeight_2;
        zigZagTip = zigZagBase - zigZagHeight;
        path.lineTo(QPointF(area.right(), zigZagTip));
        for (int i = 0; i < zigZagCount; ++i) {
            path.lineTo(QPointF(area.right() - (2 * i + 1) * zigZagWidth_2, zigZagBase));
            path.lineTo(QPointF(area.right() - (i + 1) * zigZagWidth, zigZagTip));
        }
        break;
    }
    case BarnZigZagWipeEffectFactory::Vertical:
    case BarnZigZagWipeEffectFactory::VerticalReversed: {
        qreal zigZagBase = width_2 - stepx - zigZagWidth_2;
        qreal zigZagTip = zigZagBase + zigZagWidth;
        path.moveTo(QPointF(width_2, area.top()));
        path.lineTo(QPointF(zigZagBase, area.top()));
        for (int i = 0; i < zigZagCount; ++i) {
            path.lineTo(QPointF(zigZagTip, (2 * i + 1) * zigZagHeight_2));
            path.lineTo(QPointF(zigZagBase, (i + 1) * zigZagHeight));
        }
        zigZagBase = width_2 + stepx + zigZagWidth_2;
        zigZagTip = zigZagBase - zigZagWidth;
        path.lineTo(QPointF(zigZagTip, area.bottom()));
        for (int i = 0; i < zigZagCount; ++i) {
            path.lineTo(QPointF(zigZagBase, area.bottom() - (2 * i + 1) * zigZagHeight_2));
            path.lineTo(QPointF(zigZagTip, area.bottom() - (i + 1) * zigZagHeight));
        }
        break;
    }
    default:
        return QPainterPath();
    }

    path.closeSubpath();

    if (reverse()) {
        QPainterPath areaPath;
        areaPath.addRect(area);
        path = areaPath.subtracted(path);
    }

    return path;
}
