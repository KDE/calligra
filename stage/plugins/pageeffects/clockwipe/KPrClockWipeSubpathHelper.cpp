/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrClockWipeSubpathHelper.h"

#include <QPainterPath>
#include <float.h>
#include <math.h>

KPrClockWipeSubpathHelper::KPrClockWipeSubpathHelper() = default;

KPrClockWipeSubpathHelper::~KPrClockWipeSubpathHelper() = default;

void KPrClockWipeSubpathHelper::addSubpathForCircularArc(QPainterPath *clipPath, QRect &boundingRect, double startAngle, double endAngle)
{
    if (fabs(startAngle - endAngle) < DBL_EPSILON)
        return;

    int width = boundingRect.width();
    int height = boundingRect.height();

    while (startAngle < 0)
        startAngle += 2 * M_PI;

    if (endAngle < startAngle)
        endAngle += 2 * M_PI;

    QPoint center = boundingRect.center();
    double maxRadius = sqrt(double(width * width / 4 + height * height / 4));

    double startAngleInQuadrant = fmod(startAngle, 0.5 * M_PI);
    double quadrantAngle = static_cast<int>(startAngle / (0.5 * M_PI)) * (0.5 * M_PI);

    int cornerX = cos(quadrantAngle + 0.5 * M_PI) < 0 ? -width / 2 : width / 2;
    int cornerY = sin(quadrantAngle + 0.5 * M_PI) < 0 ? -height / 2 : height / 2;

    double cornerAngleInQuadrant;
    if (cos(quadrantAngle + 0.5 * M_PI) * sin(quadrantAngle + 0.5 * M_PI) > 0)
        cornerAngleInQuadrant = fabs(atan(static_cast<double>(cornerY) / cornerX));
    else
        cornerAngleInQuadrant = fabs(atan(static_cast<double>(cornerX) / cornerY));

    double cornerAngle;
    if (startAngleInQuadrant < cornerAngleInQuadrant)
        cornerAngle = quadrantAngle + cornerAngleInQuadrant;
    else
        cornerAngle = quadrantAngle + M_PI - cornerAngleInQuadrant;

    clipPath->moveTo(center);
    clipPath->lineTo(QPoint(center.x() + maxRadius * cos(startAngle), center.y() - maxRadius * sin(startAngle)));

    while (cornerAngle < endAngle) {
        int cornerX = cos(cornerAngle) < 0 ? 0 : width;
        int cornerY = sin(cornerAngle) < 0 ? height : 0;
        clipPath->lineTo(QPoint(boundingRect.x() + cornerX, boundingRect.y() + cornerY));

        quadrantAngle = static_cast<int>(cornerAngle / (0.5 * M_PI)) * (0.5 * M_PI);
        cornerAngleInQuadrant = cornerAngle - quadrantAngle;
        cornerAngle = quadrantAngle + M_PI - cornerAngleInQuadrant;
    }
    clipPath->lineTo(QPoint(center.x() + maxRadius * cos(endAngle), center.y() - maxRadius * sin(endAngle)));

    clipPath->closeSubpath();
}
