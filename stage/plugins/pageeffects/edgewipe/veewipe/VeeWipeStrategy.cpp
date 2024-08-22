/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "VeeWipeStrategy.h"
#include "VeeWipeEffectFactory.h"
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

const int StepCount = 250;

VeeWipeStrategy::VeeWipeStrategy(int subtype, const char *smilSubType, bool reverse)
    : KPrPageEffectStrategy(subtype, "veeWipe", smilSubType, reverse)
{
}

VeeWipeStrategy::~VeeWipeStrategy() = default;

void VeeWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, StepCount);
}

void VeeWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, data.m_widget->rect());
    p.setClipPath(clipPath(currPos, data.m_widget->rect()));
    p.drawPixmap(QPoint(0, 0), data.m_newPage, data.m_widget->rect());
}

void VeeWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}

QPainterPath VeeWipeStrategy::clipPath(int step, const QRect &area)
{
    qreal percent = static_cast<qreal>(step) / static_cast<qreal>(StepCount);
    if (reverse())
        percent = static_cast<qreal>(StepCount - step) / static_cast<qreal>(StepCount);

    int stepx = static_cast<int>(2 * area.width() * percent);
    int stepy = static_cast<int>(2 * area.height() * percent);

    int width_2 = area.width() >> 1;
    int height_2 = area.height() >> 1;

    QPainterPath path;
    switch (subType()) {
    case VeeWipeEffectFactory::FromTop: {
        QPoint move(0, stepy - 2 * area.height());
        path.moveTo(move + area.topLeft());
        path.lineTo(move + area.bottomLeft());
        path.lineTo(move + QPoint(width_2, 2 * area.height()));
        path.lineTo(move + area.bottomRight());
        path.lineTo(move + area.topRight());
        break;
    }
    case VeeWipeEffectFactory::FromRight: {
        QPoint move(2 * area.width() - stepx, 0);
        path.moveTo(move + area.topRight());
        path.lineTo(move + area.topLeft());
        path.lineTo(move + QPoint(-area.width(), height_2));
        path.lineTo(move + area.bottomLeft());
        path.lineTo(move + area.bottomRight());
        break;
    }
    case VeeWipeEffectFactory::FromBottom: {
        QPoint move(0, 2 * area.height() - stepy);
        path.moveTo(move + area.bottomLeft());
        path.lineTo(move + area.topLeft());
        path.lineTo(move + QPoint(width_2, -area.height()));
        path.lineTo(move + area.topRight());
        path.lineTo(move + area.bottomRight());
        break;
    }
    case VeeWipeEffectFactory::FromLeft: {
        QPoint move(stepx - 2 * area.width(), 0);
        path.moveTo(move + area.topLeft());
        path.lineTo(move + area.topRight());
        path.lineTo(move + QPoint(2 * area.width(), height_2));
        path.lineTo(move + area.bottomRight());
        path.lineTo(move + area.bottomLeft());
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
