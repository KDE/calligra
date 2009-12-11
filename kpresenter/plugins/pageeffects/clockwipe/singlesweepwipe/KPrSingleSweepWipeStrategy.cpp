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

#include "KPrClockWipeSubpathHelper.h"

#include <kdebug.h>

KPrSingleSweepWipeStrategy::KPrSingleSweepWipeStrategy( double rotationX, double rotationY, int startAngle, int rotationAngle, int subType, const char * smilType, const char *smilSubType, bool reverse )
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
    Q_UNUSED(data);
    timeLine.setFrameRange( 0, 180 );
}

void KPrSingleSweepWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect( 0, 0, width, height );
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect );

    QPoint center(width/2, height/2);
    QPoint rotationCenter(width*m_rotationX, height*m_rotationY);

    double startAngle;
    double endAngle;

    if(m_rotationAngle > 0) {
        startAngle = m_startAngle;
        endAngle = startAngle + m_rotationAngle*currPos/180;
    }
    else {
        endAngle = m_startAngle;
        startAngle = endAngle + m_rotationAngle*currPos/180;
    }

    QRect boundingRect( 0, 0, width + 2*abs(rotationCenter.x() - center.x()), height + 2*abs(rotationCenter.y() - center.y() ));
    boundingRect.moveCenter(rotationCenter);

    QPainterPath clipPath;
    KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, boundingRect, startAngle, endAngle);
    p.setClipPath(clipPath);

    p.drawPixmap( QPoint( 0, 0 ), data.m_newPage, rect );
}

void KPrSingleSweepWipeStrategy::next( const KPrPageEffect::Data &data )
{
    data.m_widget->update();
}
