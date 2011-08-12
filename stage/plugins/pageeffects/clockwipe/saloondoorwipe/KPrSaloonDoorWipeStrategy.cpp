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

#include "KPrSaloonDoorWipeStrategy.h"
#include "KPrSaloonDoorWipeEffectFactory.h"

#include <math.h>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>

#include "KPrClockWipeSubpathHelper.h"

#include <kdebug.h>

KPrSaloonDoorWipeStrategy::KPrSaloonDoorWipeStrategy( int subType, const char * smilType, const char *smilSubType, bool reverse )
    : KPrSweepWipeStrategy( subType, smilType, smilSubType, reverse )
{
}

KPrSaloonDoorWipeStrategy::~KPrSaloonDoorWipeStrategy()
{
}

void KPrSaloonDoorWipeStrategy::setup( const KPrPageEffect::Data &data, QTimeLine &timeLine )
{
    Q_UNUSED(data);
    timeLine.setFrameRange( 0, 360 );
}

void KPrSaloonDoorWipeStrategy::paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data )
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();
    QRect rect( 0, 0, width, height );
    p.drawPixmap( QPoint( 0, 0 ), data.m_oldPage, rect );

    double startAngle1;
    QRect boundingRect1;

    double startAngle2;
    QRect boundingRect2;

    double rotationRange1 = -0.5*M_PI;
    double rotationRange2 = -0.5*M_PI;

    switch( subType() )
    {
        case KPrSaloonDoorWipeEffectFactory::FromTop:
        case KPrSaloonDoorWipeEffectFactory::ToTop:
            startAngle1 = 0;
            boundingRect1 = QRect( -width/2, -height, width, 2*height);

            startAngle2 = M_PI;
            boundingRect2 = QRect( width/2, -height, width, 2*height);

            rotationRange1 = -0.5*M_PI;
            rotationRange2 = 0.5*M_PI;
            break;
        case KPrSaloonDoorWipeEffectFactory::FromLeft:
        case KPrSaloonDoorWipeEffectFactory::ToLeft:
            startAngle1 = 1.5*M_PI;
            boundingRect1 = QRect( -width, -height/2, 2*width, height);

            startAngle2 = 0.5*M_PI;
            boundingRect2 = QRect( -width, height/2, 2*width, height);

            rotationRange1 = 0.5*M_PI;
            rotationRange2 = -0.5*M_PI;
            break;
        case KPrSaloonDoorWipeEffectFactory::FromBottom:
        case KPrSaloonDoorWipeEffectFactory::ToBottom:
            startAngle1 = 0;
            boundingRect1 = QRect( -width/2, 0, width, 2*height);

            startAngle2 = M_PI;
            boundingRect2 = QRect( width/2, 0, width, 2*height);

            rotationRange1 = 0.5*M_PI;
            rotationRange2 = -0.5*M_PI;
            break;
        case KPrSaloonDoorWipeEffectFactory::FromRight:
        case KPrSaloonDoorWipeEffectFactory::ToRight:
            startAngle1 = 1.5*M_PI;
            boundingRect1 = QRect( 0, -height/2, 2*width, height);

            startAngle2 = 0.5*M_PI;
            boundingRect2 = QRect( 0, height/2, 2*width, height);

            rotationRange1 = -0.5*M_PI;
            rotationRange2 = 0.5*M_PI;
            break;
        default:
            return;
    }

    if(reverse()) {
        startAngle1 = startAngle1 + rotationRange1;
        rotationRange1 *= -1;
        startAngle2 = startAngle2 + rotationRange2;
        rotationRange2 *= -1;
    }

    drawSweep( p, startAngle1, rotationRange1*currPos/360, boundingRect1, data);
    drawSweep( p, startAngle2, rotationRange2*currPos/360, boundingRect2, data);

}
