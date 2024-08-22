/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DiagonalWipeStrategy.h"
#include "DiagonalWipeEffectFactory.h"
#include <QPainter>
#include <QPainterPath>
#include <QWidget>

const int StepCount = 250;

DiagonalWipeStrategy::DiagonalWipeStrategy(int subtype, const char *smilSubType, bool reverse)
    : KPrPageEffectStrategy(subtype, "diagonalWipe", smilSubType, reverse)
{
}

DiagonalWipeStrategy::~DiagonalWipeStrategy() = default;

void DiagonalWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, StepCount);
}

void DiagonalWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, data.m_widget->rect());
    p.setClipPath(clipPath(currPos, data.m_widget->rect()));
    p.drawPixmap(QPoint(0, 0), data.m_newPage, data.m_widget->rect());
}

void DiagonalWipeStrategy::next(const KPrPageEffect::Data &data)
{
    data.m_widget->update();
}

QPainterPath DiagonalWipeStrategy::clipPath(int step, const QRect &area)
{
    qreal percent = static_cast<qreal>(step) / static_cast<qreal>(StepCount);

    QPoint vecx(static_cast<int>(2.0 * area.width() * percent), 0);
    QPoint vecy(0, static_cast<int>(2.0 * area.height() * percent));

    QPainterPath path;

    switch (subType()) {
    case DiagonalWipeEffectFactory::FromTopLeft:
        path.moveTo(area.topLeft());
        path.lineTo(area.topLeft() + vecx);
        path.lineTo(area.topLeft() + vecy);
        break;
    case DiagonalWipeEffectFactory::FromTopRight:
        path.moveTo(area.topRight());
        path.lineTo(area.topRight() - vecx);
        path.lineTo(area.topRight() + vecy);
        break;
    case DiagonalWipeEffectFactory::FromBottomLeft:
        path.moveTo(area.bottomLeft());
        path.lineTo(area.bottomLeft() + vecx);
        path.lineTo(area.bottomLeft() - vecy);
        break;
    case DiagonalWipeEffectFactory::FromBottomRight:
        path.moveTo(area.bottomRight());
        path.lineTo(area.bottomRight() - vecx);
        path.lineTo(area.bottomRight() - vecy);
        break;
    default:
        return QPainterPath();
    }

    path.closeSubpath();

    return path;
}
