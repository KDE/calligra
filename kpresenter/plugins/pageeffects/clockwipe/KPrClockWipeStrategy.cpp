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

#include "KPrClockWipeStrategy.h"

#include <math.h>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>

KPrClockWipeStrategy::KPrClockWipeStrategy(int startAngle, int bladeCount, KPrPageEffect::SubType subType, const char * smilType, const char *smilSubType, bool reverse )
    : KPrPageEffectStrategy( subType, smilType, smilSubType, reverse ), m_reverse(reverse), m_bladeCount(bladeCount)
{
    m_startAngle = static_cast<double>(startAngle)/180 * M_PI;
}

KPrClockWipeStrategy::~KPrClockWipeStrategy()
{
}

void KPrClockWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    timeLine.setFrameRange( 0, 360 );
}

void KPrClockWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect( 0, 0, width, height );
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect );

    double maxRadius = sqrt(width*width/4 + height*height/4);
    QPoint center( width/2, height/2);

    QPainterPath clipPath;
    for(int i = 0; i < m_bladeCount; i++) {

        double bladeStartAngle;
        double bladeAngle = static_cast<double>(currPos)/m_bladeCount/180 * M_PI;

        if(!m_reverse) {
            bladeStartAngle = 2*M_PI/m_bladeCount*i + m_startAngle;
        }
        else {
            bladeStartAngle = 2*M_PI/m_bladeCount*(i+1) + m_startAngle;
            bladeAngle *= -1;
        }

        QPoint bladeStartAngleEndPoint(center.x() + maxRadius*cos(bladeStartAngle), center.y() + maxRadius*sin(bladeStartAngle));
        QPoint bladeAngleEndPoint(center.x() + maxRadius*cos(bladeStartAngle + bladeAngle),
                                  center.y() + maxRadius*sin(bladeStartAngle + bladeAngle));

        clipPath.moveTo(center);
        clipPath.lineTo(bladeStartAngleEndPoint);

        double cornerAngle = static_cast<int>((bladeStartAngle)/(0.5*M_PI))*(0.5*M_PI) + 0.25*M_PI - bladeStartAngle;
        if(m_reverse)
            cornerAngle -= 0.5*M_PI;

        while((!m_reverse && cornerAngle < bladeAngle) || (m_reverse && cornerAngle > bladeAngle)) {
            int cornerX = cos(bladeStartAngle + cornerAngle) < 0 ? 0 : width;
            int cornerY = sin(bladeStartAngle + cornerAngle) < 0 ? 0 : height;
            clipPath.lineTo(QPoint( cornerX, cornerY));

            if(m_reverse)
                cornerAngle -= 0.5*M_PI;
            else
                cornerAngle += 0.5*M_PI;
        }
        clipPath.lineTo(bladeAngleEndPoint);
        clipPath.closeSubpath();
    }
    p.setClipPath(clipPath);

    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect );
}

void KPrClockWipeStrategy::next( const KPrPageEffect::Data &data )
{
    data.m_widget->update();
}
