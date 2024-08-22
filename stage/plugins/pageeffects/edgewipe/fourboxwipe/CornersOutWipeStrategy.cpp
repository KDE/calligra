/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CornersOutWipeStrategy.h"
#include "FourBoxWipeEffectFactory.h"
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

const int StepCount = 250;

CornersOutWipeStrategy::CornersOutWipeStrategy(bool reverse)
    : KPrPageEffectStrategy(reverse ? FourBoxWipeEffectFactory::CornersOutReverse : FourBoxWipeEffectFactory::CornersOut, "fourBoxWipe", "cornersOut", reverse)
{
}

CornersOutWipeStrategy::~CornersOutWipeStrategy() = default;

void CornersOutWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, StepCount);
}

void CornersOutWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, data.m_widget->rect());
    p.setClipPath(clipPath(currPos, data.m_widget->rect()));
    p.drawPixmap(QPoint(0, 0), data.m_newPage, data.m_widget->rect());
}

void CornersOutWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}

QPainterPath CornersOutWipeStrategy::clipPath(int step, const QRect &area)
{
    int width_2 = area.width() >> 1;
    int height_2 = area.height() >> 1;
    int width_4 = width_2 >> 1;
    int height_4 = height_2 >> 1;

    qreal percent = static_cast<qreal>(step) / static_cast<qreal>(StepCount);
    int stepx = static_cast<int>(width_2 * percent);
    int stepy = static_cast<int>(height_2 * percent);

    QRect templateRect;

    if (reverse())
        templateRect = QRect(QPoint(0, 0), QSize(width_2 - stepx, height_2 - stepy));
    else
        templateRect = QRect(QPoint(0, 0), QSize(stepx, stepy));

    QRect topLeft = templateRect;
    topLeft.moveCenter(QPoint(width_4, height_4));
    QRect topRight = templateRect;
    topRight.moveCenter(QPoint(width_2 + width_4, height_4));
    QRect bottomRight = templateRect;
    bottomRight.moveCenter(QPoint(width_2 + width_4, height_2 + height_4));
    QRect bottomLeft = templateRect;
    bottomLeft.moveCenter(QPoint(width_4, height_2 + height_4));

    QPainterPath path;
    path.addRect(topLeft);
    path.addRect(topRight);
    path.addRect(bottomRight);
    path.addRect(bottomLeft);

    if (reverse()) {
        QPainterPath areaPath;
        areaPath.addRect(area);
        path = areaPath.subtracted(path);
    }

    return path;
}
