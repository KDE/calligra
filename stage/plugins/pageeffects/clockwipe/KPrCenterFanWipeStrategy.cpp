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

#include "KPrCenterFanWipeStrategy.h"

#include <math.h>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>

#include "KPrClockWipeSubpathHelper.h"

KPrCenterFanWipeStrategy::KPrCenterFanWipeStrategy(int startAngle, int fanCount, int subType, const char * smilType, const char *smilSubType, bool reverse )
    : KPrPageEffectStrategy( subType, smilType, smilSubType, reverse ), m_fanCount(fanCount)
{
    m_startAngle = static_cast<double>(startAngle)/180 * M_PI;
}

KPrCenterFanWipeStrategy::~KPrCenterFanWipeStrategy()
{
}

void KPrCenterFanWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    Q_UNUSED(data);
    timeLine.setFrameRange( 0, 180 );
}

void KPrCenterFanWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect( 0, 0, width, height );
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect );

    QPainterPath clipPath;
    for(int i = 0; i<m_fanCount; i++) {
        double fanStartAngle = m_startAngle + (2*M_PI)/m_fanCount*i;

        double angle = static_cast<double>(currPos)/180 * M_PI/m_fanCount;

        double startAngle = fanStartAngle - angle;
        double endAngle = fanStartAngle + angle;

        KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, rect, startAngle, endAngle);
    }
    p.setClipPath(clipPath);

    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect );
}

void KPrCenterFanWipeStrategy::next( const KPrPageEffect::Data &data )
{
    data.m_widget->update();
}
