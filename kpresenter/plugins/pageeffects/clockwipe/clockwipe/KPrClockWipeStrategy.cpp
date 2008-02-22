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

KPrClockWipeStrategy::KPrClockWipeStrategy(int startAngle, KPrPageEffect::SubType subType, const char * smilType, const char *smilSubType, bool reverse )
    : KPrPageEffectStrategy( subType, smilType, smilSubType, reverse )
{
    m_startAngle = static_cast<double>(startAngle)/180 * M_PI;
    m_reverse = reverse;
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
    double cornerAngle = 0.25*M_PI;
    double angle = static_cast<double>(currPos)/180 * M_PI;
    if(m_reverse)
        angle *= -1;

    QPainterPath clipPath;
    clipPath.moveTo(QPoint( width/2, height/2));
    clipPath.lineTo(QPoint(width/2 + maxRadius*cos(m_startAngle),height/2 + maxRadius*sin(m_startAngle)));

    while((!m_reverse && cornerAngle < angle) || (m_reverse && cornerAngle > angle)) {
        int cornerX = cos(m_startAngle + cornerAngle) < 0 ? 0 : width;
        int cornerY = sin(m_startAngle + cornerAngle) < 0 ? 0 : height;
        clipPath.lineTo(QPoint( cornerX, cornerY));

        if(m_reverse)
            cornerAngle -= 0.5*M_PI;
        else
            cornerAngle += 0.5*M_PI;
    }
    clipPath.lineTo(QPoint(width/2 + maxRadius*cos(m_startAngle + angle),height/2 + maxRadius*sin(m_startAngle + angle)));
    clipPath.closeSubpath();
    p.setClipPath(clipPath);

    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect );
}

void KPrClockWipeStrategy::next( const KPrPageEffect::Data &data )
{
    int lastPos = data.m_timeLine.frameForTime( data.m_lastTime );
    int currPos = data.m_timeLine.frameForTime( data.m_currentTime );

    int width = data.m_widget->width();
    int height = data.m_widget->height();
    double maxRadius = sqrt(width*width/4 + height*height/4);

    QPoint center( width/2, height/2);
    double oldAngle = static_cast<double>(lastPos)/180 * M_PI;
    double newAngle = static_cast<double>(currPos)/180 * M_PI;
    if(m_reverse) {
        oldAngle *= -1;
        newAngle *= -1;
    }
    QPoint oldEndPoint(center.x() + maxRadius*cos(m_startAngle + oldAngle), center.y() + maxRadius*sin(m_startAngle + oldAngle));
    QPoint newEndPoint(center.x() + maxRadius*cos(m_startAngle + newAngle), center.y() + maxRadius*sin(m_startAngle + newAngle));

    QRect oldRect(center, oldEndPoint);
    QRect newRect(center, newEndPoint);
    data.m_widget->update(oldRect.united(newRect));
}
