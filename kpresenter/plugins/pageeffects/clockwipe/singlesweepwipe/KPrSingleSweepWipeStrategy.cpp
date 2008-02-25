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

#include "KPrSingleSweepWipeStrategy.h"

#include <math.h>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>

KPrSingleSweepWipeStrategy::KPrSingleSweepWipeStrategy( double rotationX, double rotationY, int startAngle, int rotationAngle, KPrPageEffect::SubType subType, const char * smilType, const char *smilSubType, bool reverse )
    : KPrPageEffectStrategy( subType, smilType, smilSubType, reverse )
    , m_rotationX(rotationX)
    , m_rotationY(rotationY)
{
    m_startAngle = static_cast<double>(startAngle)/180 * M_PI;
    m_rotationAngle = static_cast<double>(rotationAngle)/180 * M_PI;
}

KPrSingleSweepWipeStrategy::~KPrSingleSweepWipeStrategy()
{
}

void KPrSingleSweepWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    timeLine.setFrameRange( 0, 180 );
}

void KPrSingleSweepWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect( 0, 0, width, height );
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect );

    QPoint rotationCenter(width*m_rotationX, height*m_rotationY);
    double maxRadius = sqrt(width*width + height*height);
    double cornerAngle = 0.25*M_PI;
    double angle = m_rotationAngle/180*currPos;

    QPainterPath clipPath;
    clipPath.moveTo(rotationCenter);
    clipPath.lineTo(QPoint( rotationCenter.x() + maxRadius*cos(m_startAngle), rotationCenter.y() + maxRadius*sin(m_startAngle)));

    while((m_rotationAngle > 0 && cornerAngle < angle) || (m_rotationAngle < 0 && cornerAngle > angle)) {
        int cornerX = cos(m_startAngle + cornerAngle) < 0 ? 0 : width;
        int cornerY = sin(m_startAngle + cornerAngle) < 0 ? 0 : height;
        clipPath.lineTo(QPoint( cornerX, cornerY));

        if(m_rotationAngle > 0)
            cornerAngle += 0.5*M_PI;
        else
            cornerAngle -= 0.5*M_PI;
    }
    clipPath.lineTo(QPoint(rotationCenter.x() + maxRadius*cos(m_startAngle + angle), rotationCenter.y() + maxRadius*sin(m_startAngle + angle)));
    clipPath.closeSubpath();
    p.setClipPath(clipPath);

   p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect );
}

void KPrSingleSweepWipeStrategy::next( const KPrPageEffect::Data &data )
{
    int lastPos = data.m_timeLine.frameForTime( data.m_lastTime );
    int currPos = data.m_timeLine.frameForTime( data.m_currentTime );

    int width = data.m_widget->width();
    int height = data.m_widget->height();
    double maxRadius = sqrt(width*width + height*height);

    QPoint rotationCenter(width*m_rotationX, height*m_rotationY);
    double oldAngle = m_rotationAngle/180*lastPos;
    double newAngle = m_rotationAngle/180*currPos;

    QPoint oldEndPoint(QPoint(rotationCenter.x() + maxRadius*cos(m_startAngle + oldAngle), rotationCenter.y() + maxRadius*sin(m_startAngle + oldAngle)));
    QPoint newEndPoint(QPoint(rotationCenter.x() + maxRadius*cos(m_startAngle + newAngle), rotationCenter.y() + maxRadius*sin(m_startAngle + newAngle)));

    QRect oldRect(rotationCenter, oldEndPoint);
    QRect newRect(rotationCenter, newEndPoint);
    data.m_widget->update(oldRect.united(newRect));
}
