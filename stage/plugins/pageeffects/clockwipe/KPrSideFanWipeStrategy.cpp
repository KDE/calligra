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

#include "KPrSideFanWipeStrategy.h"

#include <math.h>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>

#include <kdebug.h>

#include "KPrClockWipeSubpathHelper.h"

KPrSideFanWipeStrategy::KPrSideFanWipeStrategy(int positionAngle, int fanCount, int subType, const char * smilType, const char *smilSubType, bool reverse )
    : KPrPageEffectStrategy( subType, smilType, smilSubType, reverse ), m_fanCount(fanCount)
{
    m_positionAngle = static_cast<double>(positionAngle)/180 * M_PI;
    m_startAngle = static_cast<double>(positionAngle)/180 * M_PI + M_PI;
}

KPrSideFanWipeStrategy::~KPrSideFanWipeStrategy()
{
}

void KPrSideFanWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    Q_UNUSED(data);
    timeLine.setFrameRange( 0, 180 );
}

void KPrSideFanWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect( 0, 0, width, height );
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect );

    QPoint center(width/2, height/2);

    for(int i = 0; i < m_fanCount; i++) {

        double fanAnglePositionAngle = m_positionAngle + 2*M_PI/m_fanCount*i;
        QPoint rotationCenter(center.x() + width/2*qRound(cos(fanAnglePositionAngle)),
                              center.y() - height/2*qRound(sin(fanAnglePositionAngle)));

        double fanAngle = fanAnglePositionAngle + M_PI;
        double angle = static_cast<double>(currPos)/90 * 0.25*M_PI;

        QRect boundingRect;
        if(m_fanCount > 1) {
            boundingRect = rect;
            boundingRect.translate(rotationCenter.x() - center.x(), rotationCenter.y() - center.y());
        }
        else {
            boundingRect = QRect( 0, 0, width + 2*abs(rotationCenter.x() - center.x()), height + 2*abs(rotationCenter.y() - center.y() ));
            boundingRect.moveCenter(rotationCenter);
        }

        QPainterPath clipPath;
        if(reverse()) {
            KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, boundingRect, fanAngle - 0.5*M_PI, fanAngle - 0.5*M_PI + angle);
            KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, boundingRect, fanAngle + 0.5*M_PI - angle, fanAngle + 0.5*M_PI);
            p.setClipPath(clipPath);
            p.drawPixmap( rect.intersected(boundingRect), data.m_newPage, rect.intersected(boundingRect) );
        }
        else {
            KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, boundingRect, fanAngle - angle, fanAngle + angle);
            p.setClipPath(clipPath);
            p.drawPixmap( rect.intersected(boundingRect), data.m_newPage, rect.intersected(boundingRect) );
        }
    }
}

void KPrSideFanWipeStrategy::next( const KPrPageEffect::Data &data )
{
    data.m_widget->update();
}
