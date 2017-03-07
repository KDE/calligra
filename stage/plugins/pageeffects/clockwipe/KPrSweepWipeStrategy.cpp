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

#include "KPrSweepWipeStrategy.h"

#include <math.h>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>

#include "KPrClockWipeSubpathHelper.h"


KPrSweepWipeStrategy::KPrSweepWipeStrategy( int subType, const char * smilType, const char *smilSubType, bool reverse )
    : KPrPageEffectStrategy( subType, smilType, smilSubType, reverse )
{
}

KPrSweepWipeStrategy::~KPrSweepWipeStrategy()
{
}

void KPrSweepWipeStrategy::next( const KPrPageEffect::Data &data )
{
    data.m_widget->update();
}

void KPrSweepWipeStrategy::drawSweep( QPainter &p, double angle, double rotationRange, QRect boundingRect, const KPrPageEffect::Data &data )
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect( 0, 0, width, height );

    double startAngle;
    double endAngle;

    if(rotationRange > 0) {
        startAngle = angle;
        endAngle = startAngle + rotationRange;
    }
    else {
        endAngle = angle;
        startAngle = endAngle + rotationRange;
    }

    QPainterPath clipPath;
    KPrClockWipeSubpathHelper::addSubpathForCircularArc(&clipPath, boundingRect, startAngle, endAngle);
    p.setClipPath(clipPath);

    p.drawPixmap( rect.intersected(boundingRect), data.m_newPage, rect.intersected(boundingRect) );
}
