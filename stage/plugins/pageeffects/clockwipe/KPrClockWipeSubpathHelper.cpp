/* This file is part of the KDE project
   Copyright (C) 2008 Sven Langkamp <sven.langkamp@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrClockWipeSubpathHelper.h"

#include <math.h>
#include <float.h>
#include <QPainterPath>


KPrClockWipeSubpathHelper::KPrClockWipeSubpathHelper()
{
}

KPrClockWipeSubpathHelper::~KPrClockWipeSubpathHelper()
{
}

void KPrClockWipeSubpathHelper::addSubpathForCircularArc(QPainterPath* clipPath, QRect& boundingRect, double startAngle, double endAngle)
{
    if(fabs(startAngle - endAngle) < DBL_EPSILON)
        return;

    int width = boundingRect.width();
    int height = boundingRect.height();

    while(startAngle < 0)
        startAngle += 2*M_PI;

    if(endAngle < startAngle)
        endAngle += 2*M_PI;

    QPoint center = boundingRect.center();
    double maxRadius = sqrt(double(width*width/4 + height*height/4));

    double startAngleInQuadrant = fmod(startAngle, 0.5*M_PI);
    double quadrantAngle = static_cast<int>(startAngle / (0.5*M_PI))*(0.5*M_PI);

    int cornerX = cos(quadrantAngle + 0.5*M_PI) < 0 ? -width/2 : width/2;
    int cornerY = sin(quadrantAngle + 0.5*M_PI) < 0 ? -height/2 : height/2;

    double cornerAngleInQuadrant;
    if(cos(quadrantAngle + 0.5*M_PI)*sin(quadrantAngle + 0.5*M_PI) > 0)
        cornerAngleInQuadrant = fabs(atan(static_cast<double>(cornerY)/cornerX));
    else 
        cornerAngleInQuadrant = fabs(atan(static_cast<double>(cornerX)/cornerY));

    double cornerAngle;
    if( startAngleInQuadrant < cornerAngleInQuadrant )
        cornerAngle = quadrantAngle + cornerAngleInQuadrant;
    else
        cornerAngle = quadrantAngle + M_PI - cornerAngleInQuadrant;

    clipPath->moveTo(center);
    clipPath->lineTo(QPoint(center.x() + maxRadius*cos(startAngle), center.y() - maxRadius*sin(startAngle)));

    while(cornerAngle < endAngle) {

        int cornerX = cos(cornerAngle) < 0 ?  0 : width;
        int cornerY = sin(cornerAngle) < 0 ?  height : 0;
        clipPath->lineTo(QPoint( boundingRect.x() + cornerX, boundingRect.y() + cornerY));

        quadrantAngle = static_cast<int>(cornerAngle / (0.5*M_PI))*(0.5*M_PI);
        cornerAngleInQuadrant = cornerAngle - quadrantAngle;
        cornerAngle = quadrantAngle + M_PI - cornerAngleInQuadrant;
    }
    clipPath->lineTo(QPoint(center.x() + maxRadius*cos(endAngle), center.y() - maxRadius*sin(endAngle)));


    clipPath->closeSubpath();
}

