/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DoubleBarnDoorWipeStrategy.h"
#include "MiscDiagonalWipeEffectFactory.h"
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

const int StepCount = 250;

DoubleBarnDoorWipeStrategy::DoubleBarnDoorWipeStrategy()
    : KPrPageEffectStrategy(MiscDiagonalWipeEffectFactory::DoubleBarnDoor, "miscDiagonalWipe", "doubleBarnDoor", false)
{
}

DoubleBarnDoorWipeStrategy::~DoubleBarnDoorWipeStrategy() = default;

void DoubleBarnDoorWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, StepCount);
}

void DoubleBarnDoorWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, data.m_widget->rect());
    p.setClipPath(clipPath(currPos, data.m_widget->rect()));
    p.drawPixmap(QPoint(0, 0), data.m_newPage, data.m_widget->rect());
}

void DoubleBarnDoorWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}

QPainterPath DoubleBarnDoorWipeStrategy::clipPath(int step, const QRect &area)
{
    int width_2 = area.width() >> 1;
    int height_2 = area.height() >> 1;

    qreal percent = static_cast<qreal>(step) / static_cast<qreal>(StepCount);
    int stepx = static_cast<int>(width_2 * percent);
    int stepy = static_cast<int>(height_2 * percent);

    QPainterPath path;
    path.lineTo(area.topLeft());
    path.lineTo(area.topLeft() + QPoint(stepx, 0));
    path.lineTo(area.center() - QPoint(0, stepy));
    path.lineTo(area.topRight() - QPoint(stepx, 0));
    path.lineTo(area.topRight());
    path.lineTo(area.topRight() + QPoint(0, stepy));
    path.lineTo(area.center() + QPoint(stepx, 0));
    path.lineTo(area.bottomRight() - QPoint(0, stepy));
    path.lineTo(area.bottomRight());
    path.lineTo(area.bottomRight() - QPoint(stepx, 0));
    path.lineTo(area.center() + QPoint(0, stepy));
    path.lineTo(area.bottomLeft() + QPoint(stepx, 0));
    path.lineTo(area.bottomLeft());
    path.lineTo(area.bottomLeft() - QPoint(0, stepy));
    path.lineTo(area.center() - QPoint(stepx, 0));
    path.lineTo(area.topLeft() + QPoint(0, stepy));
    path.closeSubpath();

    return path;
}
