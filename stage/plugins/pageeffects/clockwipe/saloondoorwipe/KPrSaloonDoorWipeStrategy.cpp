/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrSaloonDoorWipeStrategy.h"
#include "KPrSaloonDoorWipeEffectFactory.h"

#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <math.h>

#include "KPrClockWipeSubpathHelper.h"

KPrSaloonDoorWipeStrategy::KPrSaloonDoorWipeStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse)
    : KPrSweepWipeStrategy(subType, smilType, smilSubType, reverse)
{
}

KPrSaloonDoorWipeStrategy::~KPrSaloonDoorWipeStrategy() = default;

void KPrSaloonDoorWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, 360);
}

void KPrSaloonDoorWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect(0, 0, width, height);
    p.drawPixmap(QPoint(0, 0), data.m_oldPage, rect);

    double startAngle1;
    QRect boundingRect1;

    double startAngle2;
    QRect boundingRect2;

    double rotationRange1 = -0.5 * M_PI;
    double rotationRange2 = -0.5 * M_PI;

    switch (subType()) {
    case KPrSaloonDoorWipeEffectFactory::FromTop:
    case KPrSaloonDoorWipeEffectFactory::ToTop:
        startAngle1 = 0;
        boundingRect1 = QRect(-width / 2, -height, width, 2 * height);

        startAngle2 = M_PI;
        boundingRect2 = QRect(width / 2, -height, width, 2 * height);

        rotationRange1 = -0.5 * M_PI;
        rotationRange2 = 0.5 * M_PI;
        break;
    case KPrSaloonDoorWipeEffectFactory::FromLeft:
    case KPrSaloonDoorWipeEffectFactory::ToLeft:
        startAngle1 = 1.5 * M_PI;
        boundingRect1 = QRect(-width, -height / 2, 2 * width, height);

        startAngle2 = 0.5 * M_PI;
        boundingRect2 = QRect(-width, height / 2, 2 * width, height);

        rotationRange1 = 0.5 * M_PI;
        rotationRange2 = -0.5 * M_PI;
        break;
    case KPrSaloonDoorWipeEffectFactory::FromBottom:
    case KPrSaloonDoorWipeEffectFactory::ToBottom:
        startAngle1 = 0;
        boundingRect1 = QRect(-width / 2, 0, width, 2 * height);

        startAngle2 = M_PI;
        boundingRect2 = QRect(width / 2, 0, width, 2 * height);

        rotationRange1 = 0.5 * M_PI;
        rotationRange2 = -0.5 * M_PI;
        break;
    case KPrSaloonDoorWipeEffectFactory::FromRight:
    case KPrSaloonDoorWipeEffectFactory::ToRight:
        startAngle1 = 1.5 * M_PI;
        boundingRect1 = QRect(0, -height / 2, 2 * width, height);

        startAngle2 = 0.5 * M_PI;
        boundingRect2 = QRect(0, height / 2, 2 * width, height);

        rotationRange1 = -0.5 * M_PI;
        rotationRange2 = 0.5 * M_PI;
        break;
    default:
        return;
    }

    if (reverse()) {
        startAngle1 = startAngle1 + rotationRange1;
        rotationRange1 *= -1;
        startAngle2 = startAngle2 + rotationRange2;
        rotationRange2 *= -1;
    }

    drawSweep(p, startAngle1, rotationRange1 * currPos / 360, boundingRect1, data);
    drawSweep(p, startAngle2, rotationRange2 * currPos / 360, boundingRect2, data);
}
