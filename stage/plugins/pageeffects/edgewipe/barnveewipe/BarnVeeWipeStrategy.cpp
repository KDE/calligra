/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "BarnVeeWipeStrategy.h"
#include "BarnVeeWipeEffectFactory.h"
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

const int StepCount = 250;

BarnVeeWipeStrategy::BarnVeeWipeStrategy(int subtype, const char *smilSubType, bool reverse)
    : KPrPageEffectStrategy(subtype, "barnVeeWipe", smilSubType, reverse)
{
}

BarnVeeWipeStrategy::~BarnVeeWipeStrategy() = default;

void BarnVeeWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, StepCount);
}

void BarnVeeWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, data.m_widget->rect());
    p.setClipPath(clipPath(currPos, data.m_widget->rect()));
    p.drawPixmap(QPoint(0, 0), data.m_newPage, data.m_widget->rect());
}

void BarnVeeWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}

QPainterPath BarnVeeWipeStrategy::clipPath(int step, const QRect &area)
{
    qreal percent = static_cast<qreal>(step) / static_cast<qreal>(StepCount);
    if (reverse())
        percent = static_cast<qreal>(StepCount - step) / static_cast<qreal>(StepCount);

    int stepx = static_cast<int>(area.width() * percent);
    int stepy = static_cast<int>(area.height() * percent);

    int width_2 = area.width() >> 1;
    int height_2 = area.height() >> 1;

    int stepx_2 = static_cast<int>(width_2 * percent);
    int stepy_2 = static_cast<int>(height_2 * percent);

    QPainterPath path;
    switch (subType()) {
    case BarnVeeWipeEffectFactory::FromTop:
        path.moveTo(area.topLeft());
        path.lineTo(area.topLeft() + QPoint(0, stepy));
        path.lineTo(QPoint(width_2, area.height()) - QPoint(stepx_2, 0));
        path.lineTo(QPoint(width_2, area.height()) + QPoint(stepx_2, 0));
        path.lineTo(area.topRight() + QPoint(0, stepy));
        path.lineTo(area.topRight());
        path.lineTo(area.topRight() - QPoint(stepx_2, 0));
        path.lineTo(QPoint(width_2, area.height()) - QPoint(0, stepy));
        path.lineTo(area.topLeft() + QPoint(stepx_2, 0));
        break;
    case BarnVeeWipeEffectFactory::FromRight:
        path.moveTo(area.topRight());
        path.lineTo(area.topRight() - QPoint(stepx, 0));
        path.lineTo(QPoint(0, height_2) - QPoint(0, stepy_2));
        path.lineTo(QPoint(0, height_2) + QPoint(0, stepy_2));
        path.lineTo(area.bottomRight() - QPoint(stepx, 0));
        path.lineTo(area.bottomRight());
        path.lineTo(area.bottomRight() - QPoint(0, stepy_2));
        path.lineTo(QPoint(0, height_2) + QPoint(stepx, 0));
        path.lineTo(area.topRight() + QPoint(0, stepy_2));
        break;
    case BarnVeeWipeEffectFactory::FromBottom:
        path.moveTo(area.bottomLeft());
        path.lineTo(area.bottomLeft() - QPoint(0, stepy));
        path.lineTo(QPoint(width_2, 0) - QPoint(stepx_2, 0));
        path.lineTo(QPoint(width_2, 0) + QPoint(stepx_2, 0));
        path.lineTo(area.bottomRight() - QPoint(0, stepy));
        path.lineTo(area.bottomRight());
        path.lineTo(area.bottomRight() - QPoint(stepx_2, 0));
        path.lineTo(QPoint(width_2, 0) + QPoint(0, stepy));
        path.lineTo(area.bottomLeft() + QPoint(stepx_2, 0));
        break;
    case BarnVeeWipeEffectFactory::FromLeft:
        path.moveTo(area.topLeft());
        path.lineTo(area.topLeft() + QPoint(stepx, 0));
        path.lineTo(QPoint(area.width(), height_2) - QPoint(0, stepy_2));
        path.lineTo(QPoint(area.width(), height_2) + QPoint(0, stepy_2));
        path.lineTo(area.bottomLeft() + QPoint(stepx, 0));
        path.lineTo(area.bottomLeft());
        path.lineTo(area.bottomLeft() - QPoint(0, stepy_2));
        path.lineTo(QPoint(area.width(), height_2) - QPoint(stepx, 0));
        path.lineTo(area.topLeft() + QPoint(0, stepy_2));
        break;
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
